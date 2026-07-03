add_executable(viewer "viewer.cpp")
target_link_libraries(viewer PRIVATE tinygraphics glad)

# copy assets directory to output directory after build
add_custom_command(TARGET viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/viewer/assets
    $<TARGET_FILE_DIR:viewer>/assets)
