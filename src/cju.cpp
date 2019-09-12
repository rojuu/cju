#include <iostream>
#include <fstream>
#include <vector>

#include <cassert>

namespace cju
{

enum class TokenType
{
    PAREN,
    NAME,
};

struct Token
{
};

char *eatWhiteSpace()
{
    return 0;
}

std::vector<Token> tokenizeFile(std::ifstream &file)
{
    file.seekg(0);

    std::vector<Token> tokens;

    std::string line;
    while (std::getline(file, line)) {
        // char* ptr = &line[0];
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
