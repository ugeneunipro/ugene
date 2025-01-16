project(qscore)

find_package(Qt5 REQUIRED Core)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(qscore STATIC ${HDRS} ${SRCS})

target_link_libraries(qscore
        Qt5::Core
        U2Core zlib)
