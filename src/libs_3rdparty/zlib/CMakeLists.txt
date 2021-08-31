project(zlib)

find_package(Qt5 REQUIRED Core)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.c)
file(GLOB_RECURSE HDRS src/*.h)

add_library(zlib STATIC ${HDRS} ${SRCS})

target_link_libraries(zlib
        Qt5::Core)

