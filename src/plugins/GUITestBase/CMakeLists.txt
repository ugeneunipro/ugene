set(UGENE_PLUGIN_ID GUITestBase)
set(UGENE_PLUGIN_NAME "GUI Test Base")

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()

include_directories(../../libs_3rdparty/QSpec/src)
include_directories(../../libs_3rdparty/QSpec/src/primitives)
include_directories(../../libs_3rdparty/QSpec/src/utils)
include_directories(src)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
