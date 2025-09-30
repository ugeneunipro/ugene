project(zlib)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

if (CMAKE_COMPILER_IS_GNUCXX)
    # Disable all warning from 3rd party code.
    string(CONCAT GCC_WARNING_FLAGS " -w")
endif ()

if (WIN32)
    add_definitions(
            -W3
            -D_CRT_SECURE_NO_WARNINGS
            -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
    )
endif ()

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.c)
file(GLOB_RECURSE HDRS src/*.h)

add_library(zlib STATIC ${HDRS} ${SRCS})

if(APPLE)
    # _POSIX_C_SOURCE=200809L: Enable POSIX.1-2008 functionality required for zlib compilation on macOS
    target_compile_definitions(zlib PRIVATE
        _POSIX_C_SOURCE=200809L
    )
endif()

set_property(TARGET zlib PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(zlib)
