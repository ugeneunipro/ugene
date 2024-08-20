set(UGENE_PLUGIN_NAME api_tests)

include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools_core/htslib)

include(../../Plugin.cmake)

target_link_libraries(api_tests samtools)



