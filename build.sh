#!/bin/sh

export cwd=`pwd`
cc=clang++

# Getting LLVM flags
llvm_flags="`llvm-config --cxxflags --ldflags --system-libs --libs core`"

# Specifying the compile command
src_file="src/main.cpp"

compiler_flags_generic="-std=c++17 -Wall -Wextra -pedantic -Werror"
compiler_flags_release="-O3"
compiler_flags_debug="-g -Og"

if [ "$1" == "Release" ]
then
  compilation_type="release"
  compiler_flags="${compiler_flags_release} ${compiler_flags_generic}"
else
  compilation_type="debug"
  compiler_flags="${compiler_flags_debug} ${compiler_flags_generic}"
fi
export compile_command="${cc} ${compiler_flags} ${src_file} ${llvm_flags} -o cju"

echo Starting ${compilation_type} compilation
# Compiling the actual program
${compile_command}

echo Exporting compile_commands.json
# Exporting a compile_commands.json for editors, so they can have better intellisense features etc.

export_file_to_compile_commands()
{
  compile_commands_json="{
    \"directory\": \"${cwd}\",
    \"command\": \"${compile_command}\",
    \"file\": \""$0"\"
  },"
  echo ${compile_commands_json} >> compile_commands.json
}
export -f export_file_to_compile_commands

echo "[" > compile_commands.json
find src -type f -iname "*" -exec bash -c 'export_file_to_compile_commands "$0"' {} \;
sed -i '$ s/.$//' compile_commands.json
echo "]" >> compile_commands.json
echo Done
