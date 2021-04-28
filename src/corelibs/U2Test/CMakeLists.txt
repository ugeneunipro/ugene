project(U2Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml)

add_definitions(-DBUILDING_U2TEST_DLL)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Test SHARED ${HDRS} ${SRCS})

target_link_libraries(U2Test
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml
        U2Core)
