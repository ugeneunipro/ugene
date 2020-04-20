project(U2Designer)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt5 REQUIRED Core Gui Widgets Xml Svg Script WebKit WebKitWidgets)

add_definitions(-DBUILDING_U2DESIGNER_DLL)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

qt5_add_resources(RCC_SRCS U2Designer.qrc)

add_library(U2Designer SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Designer
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Svg Qt5::Script Qt5::WebKit Qt5::WebKitWidgets
        U2Core U2Lang U2Gui)

