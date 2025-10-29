# for intellisense
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(app "testapp.cpp")
target_link_libraries(app PRIVATE tinygraphics)
