cmake_minimum_required(VERSION 3.28)

project(glad LANGUAGES C)

add_library(glad STATIC "src/glad.c")

target_include_directories(glad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# Export glad as part of tinygraphicsTargets since it's a public dependency
install(TARGETS glad EXPORT tinygraphicsTargets ARCHIVE DESTINATION lib)
install(DIRECTORY include/glad DESTINATION include)
install(DIRECTORY include/KHR DESTINATION include)
