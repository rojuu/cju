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

void expectTokenType(const lexer_token &token, lexer_token_type tokenType)
{
    if (token.type != tokenType) {
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

ExprAST *buildFunctionAST(const std::vector<lexer_token> &tokens, int &index)
{
    ExprAST *ast = nullptr;

    // Function name
    auto token = &tokens[index];
    expectTokenType(*token, lexer_token_type::LEXER_TOKEN_NAME);
    std::string type = toString(*token);

    // Function name
    token = &tokens[++index];
    expectTokenType(*token, lexer_token_type::LEXER_TOKEN_NAME);
    std::string name = toString(*token);

    // Open paren
    token = &tokens[++index];
    expectTokenType(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION);
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
        expectTokenType(*token, lexer_token_type::LEXER_TOKEN_NAME);
        if (tokenEq(*token, "int") ||
            tokenEq(*token, "float")) {
            arg.type = toString(*token);
        } else {
            logUnexpectedTokenAndExit(*token);
        }

        token = &tokens[++index];
        expectTokenType(*token, lexer_token_type::LEXER_TOKEN_NAME);
        arg.name = toString(*token);

        arguments.push_back(arg);

        token = &tokens[index + 1];
        expectTokenType(*token, lexer_token_type::LEXER_TOKEN_PUNCTUATION);
        if (tokenEq(*token, ",")) {
            ++index;
        }
    }

    ast = new PrototypeAST(name, type, arguments);

    return ast;
}

ExprAST *buildAST(const std::vector<lexer_token> &tokens)
{
    if (tokens.size() == 0) {
        std::cerr << "Cannot build ast, found no tokens" << std::endl;
        return nullptr;
    }

    int it = 0;
    ExprAST *ast = buildFunctionAST(tokens, it);

    return ast;
}

int run(int argc, char **argv)
{
    assert(argc == 2);

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << argv[1];
        return 1;
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
        return 1;
    }

    // for (auto &token : tokens) {
    //     std::cout << tokenTypeToString(token.type) << " "
    //               << std::string(token.str, token.str + token.len) << " "
    //               << std::endl;
    // }

    ExprAST *ast = buildAST(tokens);
    if (!ast) {
        std::cerr << "Failed to build ast for file: " << argv[1] << std::endl;
        return 1;
    }

    auto json = ast->toJson();
    std::cout << json << std::endl;

    std::cout << "Done" << std::endl;

    return 0;
}

} // namespace cju
