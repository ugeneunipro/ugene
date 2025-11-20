project(${UGENE_PLUGIN_ID})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED Core Gui Widgets Xml Network PrintSupport Test)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/plugins)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist/plugins)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist/plugins)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/dist/plugins)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist/plugins)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist/plugins)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/dist/plugins)
endif ()

include_directories(src)
include_directories(../../include)

add_definitions(
        -DPLUGIN_ID="${UGENE_PLUGIN_ID}${UGENE_BUILD_MODE_SUFFIX}"
)

if (NOT DEFINED SRCS)
    file(GLOB_RECURSE SRCS src/*.cpp src/*.c src/*.h)
endif ()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_ID}.qrc)
    qt6_add_resources(RCC_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${UGENE_PLUGIN_ID}.qrc)
endif ()

add_library(${UGENE_PLUGIN_ID} SHARED ${SRCS} ${RCC_SRCS})

set(UGENE_PLUGIN_LIBS
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Network Qt6::PrintSupport Qt6::Test
        U2Core U2Algorithm U2Formats U2Gui U2View U2Lang U2Designer QSpec U2Script)

target_link_libraries(${UGENE_PLUGIN_ID} ${UGENE_PLUGIN_LIBS})

include(${CMAKE_SOURCE_DIR}/cmake/PluginDescriptor.cmake)
