#!/bin/sh

cwd=`pwd`
cc=clang++

# Getting LLVM flags
llvm_flags="`llvm-config --cxxflags --ldflags --system-libs --libs core`"

# Specifying the compile command
src_file="src/main.cpp"
compile_command="${cc} -g -Og ${src_file} -std=c++17 -Wall -Wextra -pedantic -Werror ${llvm_flags} -o cju"

echo Starting compilation
# Compiling the actual program
${compile_command}

echo Exporting compile_commands.json
# Exporting a compile_commands.json for editors, so they can have better syntax highlighting etc.
compile_commands_json="[
{
  \"directory\": \"${cwd}\",
  \"command\": \"${compile_command}\",
  \"file\": \"${src_file}\"
}
]"
echo ${compile_commands_json} > compile_commands.json

echo Done
