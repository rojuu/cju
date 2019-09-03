cmake_minimum_required(VERSION 3.0)

project(cju)

include_directories(${PROJECT_NAME} src)
add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_options(${PROJECT_NAME} ${CXX_FLAGS} PUBLIC "-std=c++17" "-Wall" "-Wextra" "-pedantic" "-Werror")
