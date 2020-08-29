#!/bin/bash

echo ----Building cju:
echo ./build.sh
./build.sh
echo
echo ----Running cju:
echo ./cju test.c
./cju test.c

echo
echo ----test.c
cat test.c
echo
echo ----tester.c
cat tester.c

echo
echo ----Compiling cju result with tester.c
echo gcc output.o tester.c -o tester.out
gcc output.o tester.c -o tester.out

echo
echo ----Running cju test program:
echo ./tester.out
./tester.out
