project(breakpad)

find_package(Qt5 REQUIRED Core)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)

file(GLOB COMMON_SRCS src/common/*.cc src/common/*.c src/client/*.cc)
file(GLOB COMMON_HDRS src/common/*.h src/client/*.h)

if (WIN32)
    file(GLOB_RECURSE SRCS src/client/windows/*.cc src/common/windows/*.cc)
    file(GLOB_RECURSE HDRS src/client/windows/*.h src/common/windows/*.h)
elseif (APPLE)
    file(GLOB_RECURSE SRCS src/client/mac/*.cc src/common/mac/*.cc src/common/mac/*.mm)
    file(GLOB_RECURSE HDRS src/client/mac/*.h src/common/mac/*.h)
elseif (UNIX)
    file(GLOB_RECURSE SRCS src/client/linux/*.cc src/common/linux/*.cc)
    file(GLOB_RECURSE HDRS src/client/linux/*.h src/common/linux/*.h)
endif ()

add_library(breakpad SHARED ${COMMON_HDRS} ${HDRS} ${COMMON_SRCS} ${SRCS})

target_link_libraries(breakpad Qt5::Core)

if (APPLE)
    target_link_libraries(breakpad "-framework CoreFoundation")
    target_link_libraries(breakpad "-framework Security")
endif ()
