set(UGENE_PLUGIN_NAME biostruct3d_view)

file(GLOB SRCS src/*.cpp src/*.c src/*.h src/gl2ps/*)

include(../../Plugin.cmake)

if (APPLE)
    target_link_libraries(biostruct3d_view "-framework OpenGL")
elseif (MSVC)
    target_link_libraries(biostruct3d_view opengl32 GLU32)
endif ()
