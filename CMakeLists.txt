cmake_minimum_required(VERSION 3.26)
project(tinygraphics VERSION 0.1 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)

# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# opengl
find_package(OpenGL REQUIRED)

# glm glew glfw
include(cmake/FindGLM.cmake)
include(cmake/FindGLEW.cmake)
include(cmake/FindGLFW3.cmake)

if (ENABLE_UTILS)
  find_package(assimp CONFIG REQUIRED)
endif()

#if (ENABLE_UTILS)
if (1)
  file (GLOB utils_src "${PROJECT_SOURCE_DIR}/utils/*.cpp" )
else()
  file (GLOB utils_src "" )
endif()

file (GLOB core_src "${PROJECT_SOURCE_DIR}/src/app.cpp"
                    "${PROJECT_SOURCE_DIR}/src/camera.cpp"
                    "${PROJECT_SOURCE_DIR}/src/cubemap.cpp"
                    "${PROJECT_SOURCE_DIR}/src/framebuffer.cpp"
                    "${PROJECT_SOURCE_DIR}/src/indexbuffer.cpp"
                    "${PROJECT_SOURCE_DIR}/src/renderer.cpp"
                    "${PROJECT_SOURCE_DIR}/src/shader.cpp"
                    "${PROJECT_SOURCE_DIR}/src/texture.cpp"
                    "${PROJECT_SOURCE_DIR}/src/vertexarray.cpp"
                    "${PROJECT_SOURCE_DIR}/src/vertexbuffer.cpp"
                    "${PROJECT_SOURCE_DIR}/src/stb_image.cpp")

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

if (DEFINED glew_SOURCE_DIR AND (NOT glew_SOURCE_DIR STREQUAL ""))
  include_directories("${glew_SOURCE_DIR}/include")
endif()

add_library (tinygraphics STATIC ${utils_src} ${core_src})
target_link_libraries (tinygraphics PRIVATE glfw glew_s OpenGL::GL glm::glm)
target_include_directories(tinygraphics PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

if (WITH_EXAMPLES)
  add_subdirectory("examples")
endif()

install(TARGETS tinygraphics
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION include/tinygraphics
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN ".git" EXCLUDE
        PATTERN "CMakeLists.txt" EXCLUDE
        PATTERN "src" EXCLUDE
        PATTERN "examples" EXCLUDE
        PATTERN "build" EXCLUDE)
