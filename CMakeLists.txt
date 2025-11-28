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

add_subdirectory(src)
target_include_directories(Vug PRIVATE src)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
execute_process(
        COMMAND llvm-config --targets-built
        OUTPUT_VARIABLE llvm_targets
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE " " ";" llvm_targets "${llvm_targets}")
llvm_map_components_to_libnames(llvm_libs
        core
        support
        analysis
        passes
        ${llvm_targets}
)

target_link_libraries(Vug PRIVATE ${llvm_libs})
