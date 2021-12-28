project(samtools)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core LinguistTools)

include_directories(src)
include_directories(src/samtools)
include_directories(../../include)

add_definitions(
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE64_SOURCE
)

file(GLOB_RECURSE SRCS src/*.cpp src/*.c)
file(GLOB_RECURSE HDRS src/*.h)
file(GLOB TS_FILES transl/*.ts)

qt5_add_translation(QM_FILES ${TS_FILES})

# Suppress original samtools warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
endif (CMAKE_COMPILER_IS_GNUCC)

add_library(samtools STATIC ${HDRS} ${SRCS} ${QM_FILES})

target_link_libraries(samtools
        Qt5::Core
        zlib)
