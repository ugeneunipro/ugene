set(UGENE_PLUGIN_ID dbi_bam)
set(UGENE_PLUGIN_NAME "BAM file based DBI implementation")

include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools_core/htslib)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
