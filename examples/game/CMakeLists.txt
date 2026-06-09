# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(game "main.cpp" "game.cpp")
target_link_libraries(game PRIVATE tinygraphics glad)

target_include_directories(game PRIVATE .)
# copy assets directory to output directory after build
# add_custom_command(TARGET planets POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/examples/assets
#     $<TARGET_FILE_DIR:planets>/assets)
#
#
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:game>/shaders)
