# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(simpleShader "simpleShader.cpp")
target_link_libraries(simpleShader PRIVATE tinygraphics)

add_executable(instances "instances.cpp")
target_link_libraries(instances PRIVATE tinygraphics)
