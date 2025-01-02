set(UGENE_PLUGIN_ID api_tests)
set(UGENE_PLUGIN_NAME "Tests for UGENE API")

include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools_core/htslib)

message(STATUS "Resolved path: ${CMAKE_SOURCE_DIR}/cmake/PluginDescriptor.cmake")

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

target_link_libraries(api_tests samtools)



