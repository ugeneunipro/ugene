project(U2Designer)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED Core Gui Widgets Xml Svg)

add_definitions(-DBUILDING_U2DESIGNER_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

qt6_add_resources(RCC_SRCS U2Designer.qrc)

add_library(U2Designer SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Designer
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Svg
        U2Core U2Lang U2Gui)

