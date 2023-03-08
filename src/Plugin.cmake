project(${UGENE_PLUGIN_NAME})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Xml Network PrintSupport Test ScriptTools)

include_directories(src)
include_directories(../../include)

add_definitions(
        -DPLUGIN_ID="${UGENE_PLUGIN_NAME}"
)

if (NOT DEFINED SRCS)
    file(GLOB_RECURSE SRCS src/*.cpp src/*.c src/*.h)
endif ()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_NAME}.qrc)
    qt5_add_resources(RCC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_NAME}.qrc)
endif ()

add_library(${UGENE_PLUGIN_NAME} SHARED ${SRCS} ${RCC_SRCS})

set(UGENE_PLUGIN_LIBS
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Network Qt5::PrintSupport Qt5::Test Qt5::ScriptTools
        U2Core U2Algorithm U2Formats U2Gui U2View U2Lang U2Designer QSpec)

target_link_libraries(${UGENE_PLUGIN_NAME} ${UGENE_PLUGIN_LIBS})

