# CMake build is not officially supported by UGENE and
# can only be used for developement (for example to enable CLion IDE integration)
#
# For CLion developers:
#   * add Qt5_DIR variable to File->Settings->Build,Execution,Deployment->CMake pointing to valid QT dir.
#   * add UGENE_TRANSLATION_FILE environment variable to the RUN configuration and point it to *.qm file produced by qmake build

cmake_minimum_required(VERSION 3.9)
project(ugene)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(UGENE_VERSION 33.0-dev)
set(UGENE_VER_MAJOR 33)
set(UGENE_VER_MINOR 0)

set(UGENE_MIN_VERSION_SQLITE 1.25.0)
set(UGENE_MIN_VERSION_MYSQL 1.25.0)

add_definitions(
        -DUGENE_VERSION=${UGENE_VERSION}
        -DUGENE_VER_MAJOR=${UGENE_VER_MAJOR}
        -DUGENE_VER_MINOR=${UGENE_VER_MINOR}

        -DUGENE_USE_BUNDLED_ZLIB
        -DUGENE_USE_BUNDLED_SQLITE
        -DUGENE_WEB_KIT

        -DQT_FATAL_ASSERT
)

set (CMAKE_CXX_STANDARD 11)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

# 3rd party libs
add_subdirectory(src/libs_3rdparty/breakpad)
add_subdirectory(src/libs_3rdparty/samtools)
add_subdirectory(src/libs_3rdparty/sqlite3)
add_subdirectory(src/libs_3rdparty/zlib)

# Core libs
add_subdirectory(src/corelibs/U2Core)
add_subdirectory(src/corelibs/U2Algorithm)
add_subdirectory(src/corelibs/U2Formats)
add_subdirectory(src/corelibs/U2Lang)
add_subdirectory(src/corelibs/U2Private)
add_subdirectory(src/corelibs/U2Gui)
add_subdirectory(src/corelibs/U2Test)
add_subdirectory(src/corelibs/U2Designer)
add_subdirectory(src/corelibs/U2View)

# launchers
add_subdirectory(src/ugeneui)
add_subdirectory(src/ugenecl)

# local plugins
add_subdirectory(src/plugins/annotator)
add_subdirectory(src/plugins/api_tests)
add_subdirectory(src/plugins/biostruct3d_view)
add_subdirectory(src/plugins/chroma_view)
add_subdirectory(src/plugins/circular_view)
add_subdirectory(src/plugins/CoreTests)
add_subdirectory(src/plugins/dbi_bam)
add_subdirectory(src/plugins/dna_export)
add_subdirectory(src/plugins/dna_flexibility)
add_subdirectory(src/plugins/dna_graphpack)
add_subdirectory(src/plugins/dna_stat)
add_subdirectory(src/plugins/dotplot)
add_subdirectory(src/plugins/enzymes)
add_subdirectory(src/plugins/external_tool_support)
add_subdirectory(src/plugins/genome_aligner)
add_subdirectory(src/plugins/GUITestBase)
add_subdirectory(src/plugins/linkdata_support)
add_subdirectory(src/plugins/orf_marker)
add_subdirectory(src/plugins/pcr)
add_subdirectory(src/plugins/perf_monitor)
add_subdirectory(src/plugins/query_designer)
add_subdirectory(src/plugins/remote_blast)
add_subdirectory(src/plugins/repeat_finder)
add_subdirectory(src/plugins/smith_waterman)
add_subdirectory(src/plugins/test_runner)
add_subdirectory(src/plugins/weight_matrix)
add_subdirectory(src/plugins/workflow_designer)

# 3rd party plugins
add_subdirectory(src/plugins_3rdparty/ball)
add_subdirectory(src/plugins_3rdparty/gor4)
add_subdirectory(src/plugins_3rdparty/hmm2)
add_subdirectory(src/plugins_3rdparty/kalign)
add_subdirectory(src/plugins_3rdparty/phylip)
add_subdirectory(src/plugins_3rdparty/primer3)
add_subdirectory(src/plugins_3rdparty/psipred)
add_subdirectory(src/plugins_3rdparty/ptools)
add_subdirectory(src/plugins_3rdparty/sitecon)
add_subdirectory(src/plugins_3rdparty/umuscle)
add_subdirectory(src/plugins_3rdparty/variants)
