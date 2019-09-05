all:
	clang++ -g -Og src/main.cpp -std=c++17 -Wall -Wextra -pedantic -Werror `llvm-config --cxxflags --ldflags --system-libs --libs core` -o cju