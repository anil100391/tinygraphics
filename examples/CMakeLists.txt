# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(simpleShader "simpleShader.cpp")
target_link_libraries(simpleShader PRIVATE tinygraphics glad)

add_executable(instances "instances.cpp")
target_link_libraries(instances PRIVATE tinygraphics glad)

add_executable(light "light.cpp")
target_link_libraries(light PRIVATE tinygraphics glad)
