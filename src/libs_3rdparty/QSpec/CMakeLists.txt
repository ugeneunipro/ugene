project(QSpec)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Test)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

add_definitions(-DBUILDING_QSPEC_DLL)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(QSpec SHARED ${HDRS} ${SRCS})

target_link_libraries(QSpec Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test)

if (UNIX AND NOT APPLE)
    target_link_libraries(QSpec
            Xtst
            X11
            stdc++fs)
endif ()

if (APPLE)
    target_link_libraries(QSpec
            "-framework AppKit"
            "-framework ApplicationServices")
endif ()
