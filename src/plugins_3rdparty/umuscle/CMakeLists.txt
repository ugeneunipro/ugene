set(UGENE_PLUGIN_NAME umuscle)

# TODO: this are the real UGENE bugs -> fix them
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion-null")

include(../../Plugin.cmake)
