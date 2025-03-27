cmake_minimum_required(VERSION 3.27)
project(Vug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
        -Wall
        -Werror
        -Wextra
        -Wpedantic
        -Wdouble-promotion
        -Wno-unused-parameter
        -Wimplicit-fallthrough
        -fstack-usage
)

add_executable(Vug src/Main.cpp)
add_subdirectory(src)

target_precompile_headers(Vug PRIVATE src/pch.hpp)
target_include_directories(Vug PRIVATE src)