#include <iostream>
#include <fstream>
#include <vector>

#include <cassert>

struct Token
{
};

std::vector<std::string> splitIntoLines(const std::string &fileContents)
{
    std::vector<std::string> result;

    size_t lineStartIndex = 0;
    size_t lineEndIndex = 0;
    while (lineEndIndex != fileContents.npos) {
        lineEndIndex = fileContents.find("\n", lineStartIndex); // TODO: Make work with CRLF as well
        std::string line(fileContents, lineStartIndex, lineEndIndex - lineStartIndex);
        if (line.size() > 0) {
            result.push_back(line);
        }
        lineStartIndex = lineEndIndex + 1;
    }

    return result;
}

std::vector<Token> tokenizeFile(const std::string &fileContents)
{
    auto lines = splitIntoLines(fileContents);

    std::cout << "line count " << lines.size() << std::endl;
    for (auto &line : lines) {
        std::cout << line << std::endl;
    }
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

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}