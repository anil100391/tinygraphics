cmake_minimum_required(VERSION 3.13)
project(tinygraphics CXX)
set (CMAKE_CXX_STANDARD 20)

# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
# find_package(GLEW REQUIRED)
# find_package(glfw3 CONFIG REQUIRED)

include(FetchContent)

# glm
FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  FIND_PACKAGE_ARGS NAMES glm)

FetchContent_MakeAvailable(glm)

# glew
FetchContent_Declare(glew
  URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
  FIND_PACKAGE_ARGS NAMES GLEW)

FetchContent_GetProperties(glew)
if (NOT glew_POPULATED)
  FetchContent_Populate(glew)
  add_subdirectory(${glew_SOURCE_DIR}/build/cmake ${glew_BINARY_DIR})
endif()

# glfw
FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  FIND_PACKAGE_ARGS NAMES glfw3)

FetchContent_MakeAvailable(glfw)

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

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${glew_SOURCE_DIR}/include")
include_directories("${glfw_SOURCE_DIR}/include")
include_directories("${glm_SOURCE_DIR}")

add_library (tinygraphics STATIC ${utils_src} ${core_src})
target_link_libraries (tinygraphics PRIVATE glfw glew_s OpenGL::GL glm::glm-header-only)

if (WITH_EXAMPLES)
  add_subdirectory("examples")
endif()

install(TARGETS tinygraphics
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
