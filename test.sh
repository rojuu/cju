#!/bin/bash

./build.sh
./cju test.c
gcc output.o tester.c -o tester.o
./tester.o
