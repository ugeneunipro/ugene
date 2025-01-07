set(UGENE_PLUGIN_ID biostruct3d_view)
set(UGENE_PLUGIN_NAME "3D structure viewer")
set(UGENE_PLUGIN_MODE ui)

file(GLOB SRCS src/*.cpp src/*.c src/*.h src/gl2ps/*)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

if (APPLE)
    # Use the OpenGL framework on macOS.
    target_link_libraries(biostruct3d_view PRIVATE "-framework OpenGL")
elseif (MSVC)
    # Use OpenGL and GLU on Windows.
    target_link_libraries(biostruct3d_view PRIVATE opengl32 glu32)
elseif (UNIX)
    # Use OpenGL and GLU on Unix/Linux.
    find_package(OpenGL REQUIRED)
    target_link_libraries(biostruct3d_view OpenGL::GL GLU)
endif ()