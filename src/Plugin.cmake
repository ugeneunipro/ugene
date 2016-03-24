project(${UGENE_PLUGIN_NAME})

cmake_minimum_required(VERSION 3.4)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml Network PrintSupport Test)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_NAME}.qrc)
    qt5_add_resources(RCC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_NAME}.qrc)
endif ()

add_library(${UGENE_PLUGIN_NAME} SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

set(UGENE_PLUGIN_LIBS
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Network Qt5::PrintSupport Qt5::Test
        U2Core U2Algorithm U2Formats U2Gui U2View U2Remote U2Lang U2Designer)

target_link_libraries(${UGENE_PLUGIN_NAME} ${UGENE_PLUGIN_LIBS})

