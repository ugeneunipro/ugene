project(U2Lang)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED Core Xml)

add_definitions(-DBUILDING_U2LANG_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)
qt6_add_resources(RCC_SRCS U2Lang.qrc)

add_library(U2Lang SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Lang
        Qt6::Core Qt6::Xml
        U2Core)

