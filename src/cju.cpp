#include "cju.h"

#define LEXER_IMPLEMENTATION
#include "lexer.h"

// NOTE: Right now our strategy is to leak all the memory allocated when generating our AST
// It's just not worth iterating through the tree to deallocate everything, when the system
// will release all our resources anyway. If there will ever be a memory pool for the AST,
// then maybe we can release that pool when we're done with it.
#include "ast.cpp"

namespace cju
{

std::string tokenTypeToString(lexer_token_type type)
{
    switch(type) {
    case LEXER_TOKEN_STRING:
        return "LEXER_TOKEN_STRING";
    case LEXER_TOKEN_LITERAL:
        return "LEXER_TOKEN_LITERAL";
    case LEXER_TOKEN_NUMBER:
        return "LEXER_TOKEN_NUMBER";
    case LEXER_TOKEN_NAME:
        return "LEXER_TOKEN_NAME";
    case LEXER_TOKEN_PUNCTUATION:
        return "LEXER_TOKEN_PUNCTUATION";
    }
}

void lexerLogCallback(void*, enum lexer_log_level logLevel, lexer_size line, const char *msg)
{
    std::string errorOutput;
    switch(logLevel) {
    case LEXER_WARNING:
        errorOutput+="LEXER WARNING ";
    break;
    case LEXER_ERROR:
        errorOutput+="LEXER ERROR ";
    break;
    }
    errorOutput+="on line ";
    errorOutput+=line;
    errorOutput+=msg;
    std::cerr << errorOutput << std::endl;
}

bool tokenizeFile(const std::string &fileContents, std::vector<lexer_token> &tokens)
{
    lexer lexer;
    lexer_init(&lexer, fileContents.c_str(), fileContents.size(), nullptr, nullptr, nullptr);

    lexer_token tok;
    while(lexer_expect_any(&lexer, &tok)) {
        tokens.push_back(tok);
    }

    if (lexer.error) {
        return false;
    }

    return true;
}

std::string toString(const lexer_token &token)
{
    std::string result = std::string(token.str, token.len);
    return result;
}

void logUnexpectedTokenAndExit(const lexer_token &token)
{
    std::cerr << "Unexpected token \"" << toString(token)
              << "\" on line: " << token.line
              << std::endl;
    exit(EXIT_FAILURE);
}

bool tokenTypeEq(const lexer_token &token, lexer_token_type tokenType)
{
    bool result = token.type == tokenType;
    return result;
}

void expectTokenTypeEq(const lexer_token &token, lexer_token_type tokenType)
{
    if (!tokenTypeEq(token, tokenType)) {
        logUnexpectedTokenAndExit(token);
    }
}

bool tokenEq(const lexer_token &token, const std::string &str)
{
    std::string cmp = toString(token);
    bool result = cmp == str;
    return result;
}

void expectTokenEq(const lexer_token &token, const std::string &str)
{
    if (!tokenEq(token, str)) {
        logUnexpectedTokenAndExit(token);
    }
}

PrototypeAST *buildPrototypeAST(const std::vector<lexer_token> &tokens, int &index)
{
    // Type
    auto *token = &tokens[index];
    expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_NAME);
    std::string type = toString(*token);

    // Name
    token = &tokens[++index];
    expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_NAME);
    std::string name = toString(*token);

    // Open paren
    token = &tokens[++index];
    expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION);
    expectTokenEq(*token, "(");

    std::vector<PrototypeAST::Argument> arguments;
    for (;;) {
        token = &tokens[++index];
        if (token->type == lexer_token_type::LEXER_TOKEN_PUNCTUATION && token->str[0] == ')') {
            ++index;
            break;
        }

        PrototypeAST::Argument arg;

        // Param
        expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_NAME);
        if (tokenEq(*token, "int") ||
            tokenEq(*token, "float")) {
            arg.type = toString(*token);
        } else {
            logUnexpectedTokenAndExit(*token);
        }

        token = &tokens[++index];
        expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_NAME);
        arg.name = toString(*token);

        arguments.push_back(arg);

        token = &tokens[index + 1];
        expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION);
        if (tokenEq(*token, ",")) {
            ++index;
        }
    }

    PrototypeAST *proto = new PrototypeAST(name, type, arguments);
    return proto;
}

FunctionAST *buildFunctionAST(const std::vector<lexer_token> &tokens, int &index)
{
    PrototypeAST *proto = buildPrototypeAST(tokens, index);

    auto *token = &tokens[index];
    expectTokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION);
    expectTokenEq(*token, "{");

    BinaryOpAST *cur = new BinaryOpAST();
    BinaryOpAST *root = cur;
    for (;;) {
        token = &tokens[++index];
        if (tokenTypeEq(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION) && tokenEq(*token, ";")) {
            auto *nextToken = &tokens[index + 1];
            if (tokenTypeEq(*nextToken, lexer_token_type::LEXER_TOKEN_PUNCTUATION) && tokenEq(*nextToken, "}")) {
                ++index;
                break;
            } else {
                continue;
            }
        }
    }

    assert(root && root->lhs && root->rhs && !"root cannot have null members");
    FunctionAST *func = new FunctionAST(proto, root);
    return func;
}

ExprAST *buildAST(const std::vector<lexer_token> &tokens)
{
    if (tokens.size() == 0) {
        std::cerr << "Cannot build ast, found no tokens" << std::endl;
        return nullptr;
    }

    int it = 0;
    FunctionAST *ast = buildFunctionAST(tokens, it);

    return ast;
}

void printUsage(const char* programName)
{
    std::cout << "Usage: " << programName << "file" << std::endl;
}

int run(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "ERROR: Wrong number or arguments" << std::endl;
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << argv[1];
        return EXIT_FAILURE;
    }

    std::string fileContents;

    file.seekg(0, std::ios::end);
    fileContents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    fileContents.assign((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    std::vector<lexer_token> tokens;
    if (!tokenizeFile(fileContents, tokens)) {
        std::cerr << "Failed to tokenize file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    // for (auto &token : tokens) {
    //     std::cout << tokenTypeToString(token.type) << " "
    //               << std::string(token.str, token.str + token.len) << " "
    //               << std::endl;
    // }

    ExprAST *ast = buildAST(tokens);
    if (!ast) {
        std::cerr << "Failed to build ast for file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    auto json = ast->toJson();
    std::cout << json << std::endl;

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}

} // namespace cju
