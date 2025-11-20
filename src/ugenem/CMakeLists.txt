project(ugenem)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED Core Gui Network Widgets)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_executable(ugenem ${HDRS} ${SRCS})
set_target_properties(ugenem PROPERTIES DEBUG_POSTFIX "d")

if (WIN32)
    add_ugene_rc_file(ugenem)
endif ()

target_link_libraries(ugenem
        Qt6::Core Qt6::Gui Qt6::Network Qt6::Widgets
        U2Core U2Gui)

