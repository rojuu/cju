#include <iostream>
#include <fstream>
#include <vector>

#include <cassert>

struct Token
{
};

std::vector<Token> tokenizeFile(const std::string &fileContents)
{
    (void)fileContents;
    // Tokenize the file here...
    return {};
}

bool readEntireFile(const std::string &filename, std::string &fileContentsOut)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        fileContentsOut = "";
        return false;
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    fileContentsOut = fileContents;
    return true;
}

int main(int argc, char **argv)
{
    assert(argc == 2);

    std::string fileContents;
    if (!readEntireFile(argv[1], fileContents)) {
        std::cerr << "Failed to read file" << std::endl;
        return EXIT_FAILURE;
    }

    tokenizeFile(fileContents);
    std::cout << fileContents << std::endl;

    return EXIT_SUCCESS;
}