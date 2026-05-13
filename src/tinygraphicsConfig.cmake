include(CMakeFindDependencyMacro)

find_dependency(OpenGL)
if (UNIX AND NOT APPLE)
    find_dependency(Threads)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/tinygraphicsTargets.cmake")
