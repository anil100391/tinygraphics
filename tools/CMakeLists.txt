cmake_minimum_required(VERSION 3.28)
project(asset2cpp VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(asset2cpp "${PROJECT_SOURCE_DIR}/asset2cpp.cpp")
