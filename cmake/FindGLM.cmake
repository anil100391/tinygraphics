find_package(glm CONFIG QUIET)

if (NOT glm_FOUND)
  message(STATUS "glm not found... downloading and building")

  include(FetchContent)
  FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git)
  FetchContent_MakeAvailable(glm)
endif()
