#include <iostream>
#include <fstream>
#include <vector>

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
    IDENTIFIER,
    FRACTION,
    NUMBER,
    OPERATOR,
    PAREN,
    SCOPE,
};

struct Token
{
    TokenType type;
    std::string value;
};

void eatWhiteSpace(char *&ptr)
{
    while (*ptr == ' ') {
        ptr++;
    }
}

std::vector<Token> tokenizeFile(std::ifstream &file)
{
    file.seekg(0);

    std::vector<Token> tokens;

    std::string line;
    while (std::getline(file, line)) {
        char* ptr = &line[0];
        eatWhiteSpace(ptr);
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

    tokenizeFile(file);

    std::cout << "Done" << std::endl;

    return 0;
}

} // namespace cju
