project(U2View)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml Svg PrintSupport)

add_definitions(-DBUILDING_U2VIEW_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2View SHARED ${HDRS} ${SRCS})

target_link_libraries(U2View
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Svg Qt5::PrintSupport
        U2Core U2Algorithm U2Formats U2Lang U2Gui)

