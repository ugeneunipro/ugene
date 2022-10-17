project(zlib)


if (CMAKE_COMPILER_IS_GNUCXX)
    # Disable all warning from 3rd party code.
    string(CONCAT GCC_WARNING_FLAGS " -w")
endif ()

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.c)
file(GLOB_RECURSE HDRS src/*.h)

add_library(zlib STATIC ${HDRS} ${SRCS})
set_property(TARGET zlib PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(zlib)
