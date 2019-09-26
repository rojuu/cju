#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cctype>

#include <cassert>

#define LEXER_IMPLEMENTATION
#define LEXER_USE_ASSERT
#define LEXER_SIZE_TYPE size_t
#include "lexer.h"

namespace cju
{

std::string tokenTypeToString(lexer_token_type type) {
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
    std::cout << errorOutput << std::endl;
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
        std::cout << "Failed to tokenize file: " << argv[1] << std::endl;
        return 1;
    }

    for (auto &token : tokens) {
        std::cout << tokenTypeToString(token.type) << " "
                  << std::string(token.str, token.str + token.len) << " "
                  << std::endl;
    }

    std::cout << "Done" << std::endl;

    return 0;
}

} // namespace cju
