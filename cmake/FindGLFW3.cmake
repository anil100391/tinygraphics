find_package(glfw3 CONFIG QUIET)

if (NOT glfw3_FOUND)
  message(STATUS "glfw not found... fetching from github")

  include(FetchContent)
  FetchContent_Declare(glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git)
  FetchContent_MakeAvailable(glfw3)
endif()
