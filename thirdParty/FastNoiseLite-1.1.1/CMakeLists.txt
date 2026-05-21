cmake_minimum_required(VERSION 3.16)
project(fastNoiseLite)

add_library(fastNoiseLite INTERFACE)
target_include_directories(fastNoiseLite INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Cpp")