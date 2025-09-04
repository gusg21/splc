cmake_minimum_required(VERSION 3.21)
project(splc)

add_executable(splc)
target_compile_options(splc PUBLIC -Wall -Wpedantic -Werror)

add_subdirectory(src)