if (WIN32)
    set(CMAKE_PREFIX_PATH APPEND ${PROJECT_SOURCE_DIR}/tools/bin/win64)
elseif(UNIX)
    set(CMAKE_PREFIX_PATH APPEND ${PROJECT_SOURCE_DIR}/tools/bin/linux64)
endif()

find_program(ASSET2CPP asset2cpp REQUIRED)

set(DEFAULT_FONT_TTF ${CMAKE_SOURCE_DIR}/fonts/JetBrainsMonoNLNerdFontMono_Thin.ttf)
set(DEFAULT_FONT_CPP ${CMAKE_BINARY_DIR}/generated/default_font.cpp)

add_custom_command(
    OUTPUT ${DEFAULT_FONT_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/generated
    COMMAND ${ASSET2CPP} ${DEFAULT_FONT_TTF} ${DEFAULT_FONT_CPP}
    DEPENDS ${DEFAULT_FONT_TTF}
    COMMENT "Generating default_font.cpp"
    VERBATIM)

set(utils_src
    "${PROJECT_SOURCE_DIR}/src/utils/mesh.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils/meshbufferobjects.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils/objio.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils/meshgl.cpp")

set(core_src "${DEFAULT_FONT_CPP}"
             "${PROJECT_SOURCE_DIR}/src/app.cpp"
             "${PROJECT_SOURCE_DIR}/src/camera.cpp"
             "${PROJECT_SOURCE_DIR}/src/cubemap.cpp"
             "${PROJECT_SOURCE_DIR}/src/framebuffer.cpp"
             "${PROJECT_SOURCE_DIR}/src/indexbuffer.cpp"
             "${PROJECT_SOURCE_DIR}/src/renderer.cpp"
             "${PROJECT_SOURCE_DIR}/src/textrenderer.cpp"
             "${PROJECT_SOURCE_DIR}/src/shader.cpp"
             "${PROJECT_SOURCE_DIR}/src/texture.cpp"
             "${PROJECT_SOURCE_DIR}/src/vertexarray.cpp"
             "${PROJECT_SOURCE_DIR}/src/vertexbuffer.cpp")

set(imgui_src
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp")

set(stb_src
    "${PROJECT_SOURCE_DIR}/src/stb/stb_image.cpp"
    "${PROJECT_SOURCE_DIR}/src/stb/stb_image_write.cpp"
    "${PROJECT_SOURCE_DIR}/src/stb/stb_truetype.cpp")

# Embed GLFW and dearimgui object files into tinygraphics
add_library (tinygraphics STATIC ${utils_src} ${core_src} ${imgui_src} ${stb_src} $<TARGET_OBJECTS:glfw>)

#################################################################################
# Debug Symbols
#################################################################################
if (MSVC)
    target_compile_options(tinygraphics PRIVATE
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/Zi>)
    target_link_options(tinygraphics PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
        $<$<CONFIG:Release>:/DEBUG>)
else()
    # GCC / Clang
    target_compile_options(tinygraphics PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Release>:-g>)
endif()
#################################################################################

target_link_libraries (tinygraphics PRIVATE OpenGL::GL glad)

# Link against GLFW's platform dependencies (not GLFW itself, since its
# object files are already embedded). Use plain library names instead of
# imported targets so the export set doesn't reference them.
if (WIN32)
    target_link_libraries(tinygraphics PRIVATE gdi32)
elseif (APPLE)
    target_link_libraries(tinygraphics PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation")
elseif (UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(tinygraphics PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
endif()


# Include GLFW headers privately (not exported to users)
target_include_directories(tinygraphics PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/glfw/include>)

# Include stb headers privately (not exported to users)
target_include_directories(tinygraphics PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/stb>)

target_include_directories(tinygraphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/imgui>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/imgui/backends>
    $<INSTALL_INTERFACE:include/imgui>
    $<INSTALL_INTERFACE:include/imgui/backends>)

target_include_directories(tinygraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/glm>
        $<INSTALL_INTERFACE:include>)

target_include_directories(tinygraphics PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/tinygraphics>"
  "$<INSTALL_INTERFACE:include>"
  "$<INSTALL_INTERFACE:include/tinygraphics>")

target_sources(tinygraphics PUBLIC FILE_SET HEADERS
  BASE_DIRS "${PROJECT_SOURCE_DIR}/include"
  FILES "${PROJECT_SOURCE_DIR}/include/tinygraphics/app.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/camera.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/cubemap.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/framebuffer.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/indexbuffer.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/light.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/renderer.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/shader.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/texture.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/vertexarray.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/vertexbuffer.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/vertexbufferlayout.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/events/event.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/events/keyevent.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/events/mouseevent.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/events/windowevent.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/utils/bbox.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/utils/mesh.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/utils/meshbufferobjects.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/utils/meshgl.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/stb/stb_image.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/stb/stb_image_write.h"
        "${PROJECT_SOURCE_DIR}/include/tinygraphics/stb/stb_truetype.h")

include(GNUInstallDirs)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/third_party/glm/glm
    DESTINATION include
)

install(TARGETS tinygraphics
        EXPORT tinygraphicsTargets
        ARCHIVE DESTINATION lib
        FILE_SET HEADERS)

install(EXPORT tinygraphicsTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinygraphics
        NAMESPACE tinygraphics::)

# install imgui headers skip examples docs etc
set(imgui_core_headers
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imconfig.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imgui_internal.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imstb_rectpack.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imstb_textedit.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/imstb_truetype.h")
set(imgui_backend_headers
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.h"
    "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.h")

install(FILES ${imgui_core_headers}
    DESTINATION include/imgui)
install(FILES ${imgui_backend_headers}
    DESTINATION include/imgui/backends)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
    DESTINATION .)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/fonts"
    DESTINATION .)

install(FILES "tinygraphicsConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinygraphics)
