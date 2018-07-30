cmake_minimum_required(VERSION 3.4)
project(U2Core)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Script Network Xml)

add_definitions(
        -DBUILDING_U2CORE_DLL
        -DUGENE_MIN_VERSION_SQLITE=${UGENE_MIN_VERSION_SQLITE}
        -DUGENE_MIN_VERSION_MYSQL=${UGENE_MIN_VERSION_MYSQL}
)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Core SHARED ${HDRS} ${SRCS})

target_link_libraries(U2Core
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Script Qt5::Xml
        ugenedb zlib)
