# cju, a toy compiler

A toy compiler written in C++

## Compiling

Make sure you have LLVM development libraries installed on your system, then run the provided build script. Currently this code is built against LLVM10. Only tested on Linux, but might work on Mac. If lucky, maybe even on Windows with MinGW or WSL or something like that.

To test the output you can also use test.sh, which builds the compiler and runs it, then builds tester.c including the outputted .o from the test.c that is compiled with cju. It then runs the final test and prints the result from test.o function to verify it actually works.
