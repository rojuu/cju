#include "cju.h"

namespace cju
{

struct ExprAST {
    ExprAST() = default;
    virtual ~ExprAST() = default;
    ExprAST(const ExprAST &) = delete;
    ExprAST &operator=(const ExprAST &) = delete;

    virtual void print() {
        std::cout << "print not implemented" << std::endl;
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

    PrototypeAST(const std::string &name, const std::vector<Argument> &arguments)
        : name(name)
        , arguments(arguments)
    {
    }

    std::string name;
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
