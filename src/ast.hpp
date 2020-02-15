#include "common.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#pragma GCC diagnostic pop

namespace cju
{

static llvm::LLVMContext llvmContext;
static llvm::IRBuilder<> llvmBuilder(llvmContext);
static llvm::Module *llvmModule;
static std::unordered_map<std::string, llvm::Value *> llvmNamedValues; // TODO: Support scopes instead of having this be global

struct ExprAST {
    ExprAST() = default;
    virtual ~ExprAST() = default;
    ExprAST(const ExprAST &) = delete;
    ExprAST &operator=(const ExprAST &) = delete;
    ExprAST(const ExprAST &&) = delete;
    ExprAST &operator=(const ExprAST &&) = delete;

    virtual nlohmann::json toJson() = 0;
    virtual llvm::Value *codeGen() = 0;

    virtual void logError(const std::string &msg)
    {
        std::cerr << "[ERROR] " << typeid(*this).name() << ": " << msg << std::endl;
    }
};

struct NumberAST : public ExprAST {
    NumberAST(float value)
        : value(value)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["value"] = value;

        return json;
    }

    virtual llvm::Value *codeGen() override
    {
        llvm::Value *result = llvm::ConstantFP::get(llvmContext, llvm::APFloat(value));
        return result;
    }

    float value;
};

struct VariableAST : public ExprAST {
    VariableAST(const std::string &name, const std::string type)
        : name(name)
        , type(type)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["name"] = name;
        json["type"] = type;

        return json;
    }

    llvm::Value *genUsageCode()
    {
        llvm::Value *result = llvmNamedValues[name];
        if (!result) {
            logError("Unknown variable name: " + name);
        }
        return result;
    }

    llvm::Value *genAssignmentCode()
    {
        return nullptr;
    }

    virtual llvm::Value *codeGen() override
    {
        if (type.empty()) {
            return genUsageCode();
        } else {
            return genAssignmentCode();
        }
    }

    std::string name;
    std::string type; // If type is empty, we should've declared the variable already and it is unkown in this context
};

struct BinaryOpAST : public ExprAST {
    BinaryOpAST()
    {
    }
    BinaryOpAST(std::string op, ExprAST *lhs, ExprAST *rhs)
        : op(op)
        , lhs(lhs)
        , rhs(rhs)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["op"] = op;
        json["lhs"] = lhs->toJson();
        json["rhs"] = rhs->toJson();

        return json;
    }

    llvm::Value *handleAssignment(VariableAST *l, ExprAST *r)
    {
        if (l->type == "float") {
            if (llvmNamedValues.find(l->name) != llvmNamedValues.end()) {
                logError("Named value " + l->name + "already exists");
                return nullptr;
            }
            llvm::Value *val = r->codeGen();
            llvmNamedValues[l->name] = val;
            return val;
        } else {
            logError("Unsupported variable type " + l->type);
            return nullptr;
        }
    }

    virtual llvm::Value *codeGen() override
    {
        llvm::Value *l = lhs->codeGen();
        llvm::Value *r = rhs->codeGen();
        if (!l || !r) {
            return nullptr;
        }

        llvm::Value *result {};
        if (op == "+") {
            result = llvmBuilder.CreateFAdd(l, r, "addtmp");
        } else if (op == "-") {
            result = llvmBuilder.CreateFSub(l, r, "subtmp");
        } else if (op == "*") {
            result = llvmBuilder.CreateFMul(l, r, "multmp");
        } else if (op == "/") {
            result = llvmBuilder.CreateFDiv(l, r, "divtmp");
        } else if (op == "=") {
            auto *ls = dynamic_cast<VariableAST *>(lhs);
            if (!ls || !rhs) {
                return nullptr;
            }
            return handleAssignment(ls, rhs);
        } else {
            logError("Unsupported op " + op);
            return nullptr;
        }

        return result;
    }

    std::string op;
    ExprAST *lhs;
    ExprAST *rhs;
};

struct StatementAST : public ExprAST {
    StatementAST(std::string statement, ExprAST *rhs)
        : statement(statement)
        , rhs(rhs)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["statement"] = statement;
        json["rhs"] = rhs->toJson();

        return json;
    }

    virtual llvm::Value* codeGen() override
    {
        if (statement == "return") {
            return rhs->codeGen();
        } else {
            logError("Invalid statement: " + statement);
            return nullptr;
        }
    }

    std::string statement;
    ExprAST *rhs;
};

struct CallAST : public ExprAST {
    CallAST(std::string callee, std::vector<ExprAST *> args)
        : callee(callee)
        , args(args)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["callee"] = callee;

        auto &jsonArgs = json["args"];
        for (auto &arg : args) {
            jsonArgs.push_back(arg->toJson());
        }

        return json;
    }

    virtual llvm::Value *codeGen() override
    {
        llvm::Function *func = llvmModule->getFunction(callee);
        if (!func) {
            logError("Unknown function referenced");
            return nullptr;
        }

        if (func->arg_size() != args.size()) {
            logError("Incorrect number of arguments passed");
            return nullptr;
        }

        std::vector<llvm::Value *> argsv;
        for (unsigned i = 0, e = args.size(); i != e; ++i) {
            argsv.push_back(args[i]->codeGen());
            if (!argsv.back()) {
                return nullptr;
            }
        }

        return llvmBuilder.CreateCall(func, argsv, "calltmp");
    }

    std::string callee;
    std::vector<ExprAST *> args;
};

struct PrototypeAST : public ExprAST {
    struct Argument {
        std::string name;
        std::string type;
    };

    PrototypeAST(const std::string &name, const std::string &type, const std::vector<Argument> &args)
        : name(name)
        , type(type)
        , args(args)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["name"] = name;
        json["type"] = type;

        auto &argsJson = json["arguments"];
        for (auto &arg : args) {
            nlohmann::json argJson;
            argJson["name"] = arg.name;
            argJson["type"] = arg.type;
            argsJson.push_back(argJson);
        }

        return json;
    }

    virtual llvm::Function *codeGen() override
    {
        if (type != "float") {
            logError("Unsupported function return type");
            return nullptr;
        }

        for (auto &arg : args) {
            if (arg.type != "float") {
                logError("Unsupported function arg type");
                return nullptr;
            }
        }

        std::vector<llvm::Type *> argTypes(args.size(), llvm::Type::getFloatTy(llvmContext));
        llvm::FunctionType *funcType =
            llvm::FunctionType::get(llvm::Type::getFloatTy(llvmContext), argTypes, false);

        llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, llvmModule);

        int i = 0;
        for (auto &arg : func->args()) {
            arg.setName(args[i++].name);
        }

        return func;
    }

    std::string name;
    std::string type;
    std::vector<Argument> args;
};

struct BlockAST : public ExprAST {
    BlockAST()
    {
    }

    void push(ExprAST *expr)
    {
        exprs.push_back(expr);
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        auto &exprsJson = json["exprs"];
        for (auto *expr : exprs) {
            nlohmann::json exprJson;
            exprJson = expr->toJson();
            exprsJson.push_back(exprJson);
        }

        return json;
    }

    virtual llvm::Value *codeGen() override
    {
        llvm::Value *lastValue = nullptr;
        for (auto *expr : exprs) {
            lastValue = expr->codeGen();
        }
        return lastValue;
    }

    std::vector<ExprAST *> exprs;
};

struct FunctionAST : public ExprAST {
    FunctionAST(PrototypeAST *proto, ExprAST *body)
        : proto(proto)
        , body(body)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["proto"] = proto->toJson();
        json["body"] = body->toJson();

        return json;
    }

    virtual llvm::Value *codeGen() override
    {
        // First, check for an existing function from a previous 'extern' declaration.
        llvm::Function *function = llvmModule->getFunction(proto->name);

        if (!function) {
            function = proto->codeGen();
        }

        if (!function) {
            return nullptr;
        }

        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(llvmContext, "entry", function);
        llvmBuilder.SetInsertPoint(basicBlock);

        llvmNamedValues.clear();
        for (auto &Arg : function->args())
            llvmNamedValues[Arg.getName()] = &Arg;

        if (llvm::Value *retVal = body->codeGen()) {
            llvmBuilder.CreateRet(retVal);
            verifyFunction(*function);
            return function;
        }

        function->eraseFromParent();
        return nullptr;
    }

    PrototypeAST *proto;
    ExprAST *body;
};

} // namespace cju
