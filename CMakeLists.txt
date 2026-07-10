cmake_minimum_required(VERSION 3.28)
project(tinygraphics VERSION 0.2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# opengl
find_package(OpenGL REQUIRED)

# Find Threads for GLFW's dependencies
find_package(Threads REQUIRED)

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory("third_party/glad")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "OBJECT" CACHE STRING "" FORCE)
add_subdirectory("third_party/glfw")

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory("third_party/glm")

add_subdirectory("src")

if (NOT SKIP_EXAMPLES)
  add_subdirectory("examples")
endif()

# packaging
set(CPACK_PACKAGE_NAME "tinygraphics")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")

include(CPack)
