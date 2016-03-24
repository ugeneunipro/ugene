# CMake build is not officially supported by UGENE and
# can only be used for developement (for example to enable CLion IDE integration)
#
# For CLion developers:
#   * add Qt5_DIR variable to File->Settings->Build,Execution,Deployment->CMake pointing to valid QT dir.
#   * add UGENE_TRANSLATION_FILE environment variable and point it to *.qm file produced by qmake build

cmake_minimum_required(VERSION 3.4)
project(ugene)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(UGENE_VERSION 1.22.0-dev)
set(UGENE_VER_MAJOR 1)
set(UGENE_VER_MINOR 22)
set(UGENE_VER_PATCH 0)

set(UGENE_MIN_VERSION_SQLITE 1.13.0)
set(UGENE_MIN_VERSION_MYSQL 1.16.0)

add_definitions(
        -DUGENE_VERSION=${UGENE_VERSION}
        -DUGENE_VER_MAJOR=${UGENE_VER_MAJOR}
        -DUGENE_VER_MINOR=${UGENE_VER_MINOR}
        -DUGENE_VER_PATCH=${UGENE_VER_PATCH}

        -DUGENE_USE_BUNDLED_ZLIB

        -DQT_FATAL_ASSERT
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

add_subdirectory(src/libs_3rdparty/breakpad)
add_subdirectory(src/libs_3rdparty/gtest)
add_subdirectory(src/libs_3rdparty/samtools)
add_subdirectory(src/libs_3rdparty/sqlite3)
add_subdirectory(src/libs_3rdparty/zlib)

add_subdirectory(src/corelibs/U2Core)
add_subdirectory(src/corelibs/U2Algorithm)
add_subdirectory(src/corelibs/U2Formats)
add_subdirectory(src/corelibs/U2Lang)
add_subdirectory(src/corelibs/U2Private)
add_subdirectory(src/corelibs/U2Gui)
add_subdirectory(src/corelibs/U2Remote)
add_subdirectory(src/corelibs/U2Test)
add_subdirectory(src/corelibs/U2Designer)
add_subdirectory(src/corelibs/U2View)

add_subdirectory(src/ugeneui)

add_subdirectory(src/plugins/annotator)
add_subdirectory(src/plugins/api_tests)
add_subdirectory(src/plugins/biostruct3d_view)
add_subdirectory(src/plugins/browser_support)
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
#add_subdirectory(src/plugins/external_tool_support)
add_subdirectory(src/plugins/genome_aligner)
#add_subdirectory(src/plugins/GUITestBase)
add_subdirectory(src/plugins/linkdata_support)
add_subdirectory(src/plugins/orf_marker)
add_subdirectory(src/plugins/pcr)
add_subdirectory(src/plugins/perf_monitor)
add_subdirectory(src/plugins/query_designer)
add_subdirectory(src/plugins/remote_blast)
add_subdirectory(src/plugins/remote_service)
add_subdirectory(src/plugins/repeat_finder)
add_subdirectory(src/plugins/smith_waterman)
add_subdirectory(src/plugins/test_runner)
add_subdirectory(src/plugins/weight_matrix)
add_subdirectory(src/plugins/workflow_designer)
