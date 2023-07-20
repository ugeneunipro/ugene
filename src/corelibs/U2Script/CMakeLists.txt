project(U2Script)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Script)

add_definitions(-DBUILDING_U2SCRIPT_DLL)

include_directories(src)
include_directories(../../include)
include_directories(../U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

file(GLOB_RECURSE IGNORE_FILES src/js/*)
list(REMOVE_ITEM SRCS ${IGNORE_FILES})
list(REMOVE_ITEM HDRS ${IGNORE_FILES})

add_library(U2Script SHARED ${HDRS} ${SRCS})

target_link_libraries(U2Script
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Script
        U2Core U2Algorithm U2Formats U2Lang U2Private U2Gui U2Test)
