# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(simpleShader "simpleShader.cpp")
target_link_libraries(simpleShader PRIVATE tinygraphics glad)

add_executable(instances "instances.cpp")
target_link_libraries(instances PRIVATE tinygraphics glad)

add_executable(planets "planets.cpp")
target_link_libraries(planets PRIVATE tinygraphics glad)

# copy assets directory to output directory after build
add_custom_command(TARGET planets POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/assets
    $<TARGET_FILE_DIR:planets>/assets)

add_custom_command(TARGET planets POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:planets>/shaders)

add_subdirectory(game)
