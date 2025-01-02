set(UGENE_PLUGIN_ID biostruct3d_view)
set(UGENE_PLUGIN_NAME "3D structure viewer")
set(UGENE_PLUGIN_MODE ui)

file(GLOB SRCS src/*.cpp src/*.c src/*.h src/gl2ps/*)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

if (APPLE)
    target_link_libraries(biostruct3d_view "-framework OpenGL")
elseif (MSVC)
    target_link_libraries(biostruct3d_view opengl32 GLU32)
endif ()
