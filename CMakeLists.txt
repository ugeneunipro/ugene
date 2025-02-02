# CMake is the default build system for UGENE.
# Ensure you add the Qt5_DIR variable, example: Qt5_DIR=/home/yalgaer/tools/qt/5.15.6/gcc_64.
#
# For CLion developers:
#   Add Qt5_DIR variable in File → Settings → Build, Execution, Deployment → CMake.

cmake_minimum_required(VERSION 3.20)
project(ugene)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)
set(UGENE_VERSION 53.0-dev)

message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "CMake build architecture: ${CMAKE_SYSTEM_PROCESSOR}")

add_definitions(
        -DUGENE_VERSION=${UGENE_VERSION}
        -DQT_FATAL_ASSERT
        -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_DEBUG_POSTFIX "d")

# Set a default build type for single-configuration
# CMake generators if no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

if (APPLE)
    include_directories(/users/apple/work/qtscript/script/api)
elseif (WIN32)
    include_directories(C:/work/qtscript/script/api)
else ()
    include_directories(/home/yalgaer/work/qtscript/script/api)
endif ()

if (WIN32)
    add_definitions(
            -D_UNICODE
            -DUNICODE
            -DUGENE_USE_BUNDLED_ZLIB
            -DNOMINMAX
            -D_XKEYCHECK_H
            -D_USE_MATH_DEFINES
    )
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

    # Add map files for release binaries
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /MAP /MAPINFO:EXPORTS /DEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /MAP /MAPINFO:EXPORTS /DEBUG")
endif ()

# Use to disable -isystem support and work in qmake mode.
# set(CMAKE_NO_SYSTEM_FROM_IMPORTED True)

if (CMAKE_COMPILER_IS_GNUCXX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        message(FATAL_ERROR "GCC version must be at least 9. Detected version: ${CMAKE_CXX_COMPILER_VERSION}")
    endif ()
    message(STATUS "Using GCC version: ${CMAKE_CXX_COMPILER_VERSION}")
    # Common warning flags for GCC
    string(CONCAT GCC_WARNING_FLAGS
            " -Wall"
            " -Werror=deprecated-copy"
            " -Werror=maybe-uninitialized"
            " -Werror=parentheses"
            " -Werror=return-type"
            " -Werror=sequence-point"
            " -Werror=switch"
            " -Werror=uninitialized"
            " -Werror=unused-parameter"
            " -Werror=unused-value"
            " -Werror=unused-variable"
            " -Wno-deprecated-declarations"
            # TODO: "-Werror=shadow=local"
    )
    string(APPEND GCC_WARNING_FLAGS)

    # Apply the warning flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_WARNING_FLAGS}")
    LIST(APPEND COMPILER_FLAGS -Wl,--no-undefined)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist)
endif ()

# 3rd party libraries.
add_subdirectory(src/libs_3rdparty/QSpec)
add_subdirectory(src/libs_3rdparty/breakpad)
add_subdirectory(src/libs_3rdparty/qscore)
add_subdirectory(src/libs_3rdparty/samtools)
add_subdirectory(src/libs_3rdparty/sqlite3)
add_subdirectory(src/libs_3rdparty/zlib)

# Core libraries.
add_subdirectory(src/corelibs/U2Algorithm)
add_subdirectory(src/corelibs/U2Core)
add_subdirectory(src/corelibs/U2Designer)
add_subdirectory(src/corelibs/U2Formats)
add_subdirectory(src/corelibs/U2Gui)
add_subdirectory(src/corelibs/U2Lang)
add_subdirectory(src/corelibs/U2Private)
add_subdirectory(src/corelibs/U2Script)
add_subdirectory(src/corelibs/U2Test)
add_subdirectory(src/corelibs/U2View)

# Executables.
add_subdirectory(src/plugins_checker)
add_subdirectory(src/ugenecl)
add_subdirectory(src/ugenem)
add_subdirectory(src/ugeneui)

# Plugins.
add_subdirectory(src/plugins/CoreTests)
add_subdirectory(src/plugins/GUITestBase)
add_subdirectory(src/plugins/annotator)
add_subdirectory(src/plugins/api_tests)
add_subdirectory(src/plugins/biostruct3d_view)
add_subdirectory(src/plugins/chroma_view)
add_subdirectory(src/plugins/circular_view)
add_subdirectory(src/plugins/dbi_bam)
add_subdirectory(src/plugins/dna_export)
add_subdirectory(src/plugins/dna_flexibility)
add_subdirectory(src/plugins/dna_graphpack)
add_subdirectory(src/plugins/dna_stat)
add_subdirectory(src/plugins/dotplot)
add_subdirectory(src/plugins/enzymes)
add_subdirectory(src/plugins/external_tool_support)
add_subdirectory(src/plugins/genecut)
add_subdirectory(src/plugins/genome_aligner)
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

# Third party plugins.
add_subdirectory(src/plugins_3rdparty/ball)
add_subdirectory(src/plugins_3rdparty/gor4)
add_subdirectory(src/plugins_3rdparty/hmm2)
add_subdirectory(src/plugins_3rdparty/phylip)
add_subdirectory(src/plugins_3rdparty/primer3)
add_subdirectory(src/plugins_3rdparty/psipred)
add_subdirectory(src/plugins_3rdparty/ptools)
add_subdirectory(src/plugins_3rdparty/sitecon)
add_subdirectory(src/plugins_3rdparty/umuscle)
add_subdirectory(src/plugins_3rdparty/variants)

# Generate translations.
find_package(Qt5LinguistTools REQUIRED)
set(LANGUAGES "RU" "TR")
foreach (LANG IN LISTS LANGUAGES)
    string(TOLOWER ${LANG} LANG_LOWER)
    set(TRANSLATIONS_VAR GLOBAL_TRANSLATIONS_${LANG})
    set(QM_FILE ${CMAKE_BINARY_DIR}/dist/transl_${LANG_LOWER}.qm)
    add_custom_command(
            OUTPUT ${QM_FILE}
            COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${${TRANSLATIONS_VAR}} -qm ${QM_FILE}
            DEPENDS ${${TRANSLATIONS_VAR}}
            COMMENT "Generating translation file: ${QM_FILE}"
    )
    add_custom_target(${LANG}_translation ALL DEPENDS ${QM_FILE})
endforeach ()

include(${CMAKE_SOURCE_DIR}/cmake/Environment.cmake)

# UGENE shell script
file(COPY "${CMAKE_SOURCE_DIR}/etc/shared/ugene${UGENE_BUILD_MODE_SUFFIX}" DESTINATION "${CMAKE_BINARY_DIR}/dist")