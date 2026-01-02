# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(simpleShader "simpleShader.cpp")
target_link_libraries(simpleShader PRIVATE tinygraphics glm::glm glfw)
target_include_directories(simpleShader PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

add_executable(instances "instances.cpp")
target_link_libraries(instances PRIVATE tinygraphics glm::glm glfw)
target_include_directories(instances PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")

add_executable(light "light.cpp")
target_link_libraries(light PRIVATE tinygraphics glm::glm glfw)
target_include_directories(light PRIVATE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>")
