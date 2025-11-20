project(U2Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Xml)

add_definitions(-DBUILDING_U2TEST_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Test SHARED ${HDRS} ${SRCS})

target_link_libraries(U2Test
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml
        U2Core)
