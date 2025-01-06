project(ugeneui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml Network NetworkAuth WebSockets Script)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../include)
include_directories(../corelibs/U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

if (APPLE)
    file(GLOB_RECURSE OBJC_SRCS src/*.mm)
endif ()

qt5_add_resources(RCC_SRCS ugeneui.qrc)

add_executable(ugeneui ${HDRS} ${SRCS} ${OBJC_SRCS} ${QM_FILES} ${RCC_SRCS})
set_target_properties(ugeneui PROPERTIES DEBUG_POSTFIX "d")

target_link_libraries(ugeneui
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Network Qt5::NetworkAuth Qt5::WebSockets Qt5::Script
        U2Core U2Designer U2Algorithm U2Formats U2Gui U2View U2Test U2Lang U2Private breakpad)

