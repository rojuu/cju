#include "common.h"

namespace cju
{

struct ExprAST {
    ExprAST() = default;
    virtual ~ExprAST() = default;
    ExprAST(const ExprAST &) = delete;
    ExprAST &operator=(const ExprAST &) = delete;

    virtual nlohmann::json toJson()
    {
        std::cerr << "toJson not implemented" << std::endl;
        return {};
    }
};

struct VariableAST : ExprAST {
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

    std::string name;
    std::string type; // If type is empty, we should've declared the variable already and it is unkown in this context
};

struct BinaryOpAST : ExprAST {
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

    std::string op;
    ExprAST *lhs;
    ExprAST *rhs;
};

struct StatementAST : ExprAST {
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

    std::string statement;
    ExprAST *rhs;
};

struct NumberAST : ExprAST {
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

    float value;
};

struct PrototypeAST : ExprAST {
    struct Argument {
        std::string name;
        std::string type;
    };

    PrototypeAST(const std::string &name, const std::string &type, const std::vector<Argument> &arguments)
        : name(name)
        , type(type)
        , arguments(arguments)
    {
    }

    virtual nlohmann::json toJson() override
    {
        nlohmann::json json;

        json["name"] = name;
        json["type"] = type;

        auto &args = json["arguments"];
        for (auto &arg : arguments) {
            nlohmann::json argJson;
            argJson["name"] = arg.name;
            argJson["type"] = arg.type;
            args.push_back(argJson);
        }

        return json;
    }

    std::string name;
    std::string type;
    std::vector<Argument> arguments;
};

struct BlockAST : ExprAST {
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

    std::vector<ExprAST *> exprs;
};

struct FunctionAST : ExprAST {
    FunctionAST(PrototypeAST *proto, BlockAST *body)
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

    PrototypeAST *proto;
    BlockAST *body;
};

} // namespace cju
