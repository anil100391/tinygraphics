find_package(GLEW CONFIG QUIET)

if (NOT GLEW_FOUND)
  message(STATUS "glew not found... downloading and building")

  include(FetchContent)
  FetchContent_Declare(GLEW
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz)

  FetchContent_GetProperties(GLEW)
  if (NOT GLEW_POPULATED)
    FetchContent_Populate(GLEW)
    add_subdirectory(${glew_SOURCE_DIR}/build/cmake ${glew_BINARY_DIR})
  endif()
endif()
