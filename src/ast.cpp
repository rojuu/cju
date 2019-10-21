#include "cju.h"

namespace cju
{

struct ExprAST {
    ExprAST() = default;
    virtual ~ExprAST() = default;
    ExprAST(const ExprAST &) = delete;
    ExprAST &operator=(const ExprAST &) = delete;

    virtual nlohmann::json toJson() {
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

    std::string name;
    std::string type;
};

struct BinaryOpAST : ExprAST {
    BinaryOpAST(char op, ExprAST *lhs, ExprAST *rhs)
        : op(op)
        , lhs(lhs)
        , rhs(rhs)
    {
    }
    char op;
    ExprAST *lhs;
    ExprAST *rhs;
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
            nlohmann::json json_arg;
            json_arg["name"] = arg.name;
            json_arg["type"] = arg.type;
            args.push_back(json_arg);
        }

        return json;
    }

    std::string name;
    std::string type;
    std::vector<Argument> arguments;
};

struct FunctionAST : ExprAST {
    FunctionAST(PrototypeAST *proto, ExprAST *body)
        : proto(proto)
        , body(body)
    {
    }

    PrototypeAST *proto;
    ExprAST *body;
};

} // namespace cju
