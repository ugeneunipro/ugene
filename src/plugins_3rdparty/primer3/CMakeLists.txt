set(UGENE_PLUGIN_ID primer3)
set(UGENE_PLUGIN_NAME "Primer3")

file(GLOB_RECURSE SRCS src/*.cpp src/*.cc src/*.c src/*.h)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
