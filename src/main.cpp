#include <iostream>
#include <fstream>
#include <vector>

#include <cassert>

struct Token
{
};

std::vector<Token> tokenizeFile(std::ifstream &file)
{
    file.seekg(0);

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line.size() << " " << line << std::endl;
    }

    return {};
}

int main(int argc, char **argv)
{
    assert(argc == 2);

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << argv[1];
    }

    tokenizeFile(file);

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}