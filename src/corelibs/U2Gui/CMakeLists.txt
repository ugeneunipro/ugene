project(U2Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml Network PrintSupport Svg)

add_definitions(-DBUILDING_U2GUI_DLL)

include_directories(src)
include_directories(../../include)
include_directories(../U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

qt5_add_resources(RCC_SRCS U2Gui.qrc)

add_library(U2Gui SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Gui
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Network Qt5::PrintSupport Qt5::Svg
        U2Core U2Algorithm U2Formats U2Private)

