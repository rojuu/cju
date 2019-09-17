#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>

#include <cassert>

namespace cju
{

/**
 * letter = [a-zA-Z]
 * digit = [0-9]
 * digits = digit digit*
 * identifier = [_ | letter](letter | digit | _).*
 * number = (digits)+ | ((digits)+\.(digits)*)
 * operator = + | - | * | / | > | >= | < | <= | = | == | | | || | & | && | << | >> | %
 * parenthesis = ( | )
 * scope = { | }
 */
enum class TokenType
{
    NONE,
    IDENTIFIER,
    FRACTION,
    NUMBER,
    STRING_LITERAL,
    OPERATOR,
    PAREN,
    SEPARATOR,
    SCOPE,
};

std::string tokenTypeToString(TokenType type) {
    switch(type) {
    case TokenType::NONE:
        return "NONE";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::FRACTION:
        return "FRACTION";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::STRING_LITERAL:
        return "STRING_LITERAL";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::PAREN:
        return "PAREN";
    case TokenType::SEPARATOR:
        return "SEPARATOR";
    case TokenType::SCOPE:
        return "SCOPE";
    }
}

struct Token
{
    TokenType type;
    std::string value;
};

void findNextWord(char *&beginPtr, char *&endPtr)
{
    while (*beginPtr == ' ') {
        ++beginPtr;
    }
    endPtr = beginPtr;
    while (*endPtr != ' ' && *endPtr != 0) {
        ++endPtr;
    }
}

std::vector<Token> tokenizeFile(std::ifstream &file)
{
    file.seekg(0);

    std::vector<Token> tokens;

    std::string line;
    while (std::getline(file, line)) {
        // First split line by whitespace
        std::istringstream iss(line);
        std::vector<std::string> tokenStrings(
            (std::istream_iterator<std::string>(iss)),
            (std::istream_iterator<std::string>()));
        for (auto tokenStr : tokenStrings) {
            auto begin = tokenStr.begin();
            auto end = tokenStr.end();
            // Tokenize each part accordingly
            while (begin != end) {
        /*
        * number = (digits)+ | ((digits)+\.(digits)*)
        */
        /*
            FRACTION,
            NUMBER,
            STRING_LITERAL,
            */
           /*
            This stuff would sound like a precompiler sort of thing:
            comment blocks,
            line comments
            # defines /etc.
           */

                std::string curStr(begin, end);
                Token token{};
                std::smatch baseMatch;
                if (std::regex_match(curStr, baseMatch, std::regex("([a-zA-Z]+[a-zA-Z0-9_]*)(.*)"))) {
                    auto matchLenght = baseMatch[1].length();
                    token.type = TokenType::IDENTIFIER;
                    token.value = std::string(begin, begin + matchLenght);
                    begin += matchLenght;
                } else if (*begin == '(' || *begin == ')') {
                    token.type = TokenType::PAREN;
                    token.value = std::string(begin, begin + 1);
                    begin += 1;
                } else if (*begin == '{' || *begin == '}') {
                    token.type = TokenType::SCOPE;
                    token.value = std::string(begin, begin + 1);
                    begin += 1;
                } else if(*begin == ',' || *begin == ';') {
                    token.type = TokenType::SEPARATOR;
                    token.value = std::string(begin, begin + 1);
                    begin += 1;
                } else if(*begin == '+' ||
                          *begin == '-' ||
                          *begin == '*' ||
                          *begin == '/' ||
                          *begin == '>' ||
                          *begin == '<' ||
                          *begin == '=' ||
                          *begin == '|' ||
                          *begin == '&') {
                    token.type = TokenType::OPERATOR;
                    token.value = std::string(begin, begin +1);
                    begin += 1;
                } else if((begin[0] == '>' && begin[1] == '=') ||
                          (begin[0] == '<' && begin[1] == '=') ||
                          (begin[0] == '=' && begin[1] == '=') ||
                          (begin[0] == '|' && begin[1] == '|') ||
                          (begin[0] == '&' && begin[1] == '&') ||
                          (begin[0] == '<' && begin[1] == '<') ||
                          (begin[0] == '>' && begin[1] == '>')) {
                    token.type = TokenType::OPERATOR;
                    token.value = std::string(begin, begin + 2);
                    begin += 2;
                } else {
                    token.type = TokenType::NONE;
                    ++begin;
                }

                if (token.type != TokenType::NONE) {
                    tokens.push_back(token);
                }
            }
        }
    }

    return tokens;
}

int run(int argc, char **argv)
{
    assert(argc == 2);

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << argv[1];
        return 1;
    }

    auto tokens = tokenizeFile(file);

    for (auto token : tokens) {
        std::cout << tokenTypeToString(token.type) << " " << token.value << std::endl;
    }

    std::cout << "Done" << std::endl;

    return 0;
}

} // namespace cju
