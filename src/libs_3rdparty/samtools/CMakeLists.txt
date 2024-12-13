project(samtools)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core LinguistTools)

include_directories(src)
include_directories(src/samtools_core/htslib)
include_directories(../../include)

if (WIN32)
    include_directories(src/samtools/win32)
endif ()

add_definitions(
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE64_SOURCE
)

file(GLOB SRCS src/*.cpp src/samtools_core/*.c src/samtools_core/htslib/*.c src/samtools_core/htslib/cram/*.c src/samtools_core/htslib/htscodecs/htscodecs/*.c src/samtools_core/htslib/htslib/*.c src/samtools_core/htslib/os/*.c)
file(GLOB HDRS src/*.h   src/samtools_core/*.h src/samtools_core/htslib/*.h src/samtools_core/htslib/cram/*.h src/samtools_core/htslib/htscodecs/htscodecs/*.h src/samtools_core/htslib/htslib/*.h src/samtools_core/htslib/os/*.h)
file(GLOB TS_FILES transl/*.ts)

if (WIN32)
    file(GLOB_RECURSE SRCS src/samtools_core/htslib/win/*.c)
    file(GLOB_RECURSE HDRS src/samtools_core/htslib/win/*.h)
endif ()

qt5_add_translation(QM_FILES ${TS_FILES})

# Suppress original samtools warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
endif ()

add_library(samtools STATIC ${HDRS} ${SRCS} ${QM_FILES})

target_link_libraries(samtools
        Qt5::Core
        zlib)
