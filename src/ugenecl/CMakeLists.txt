project(ugenecl)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../include)
include_directories(../corelibs/U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_executable(ugenecl ${HDRS} ${SRCS})
set_target_properties(ugenecl PROPERTIES DEBUG_POSTFIX "d")

if (WIN32)
    add_ugene_rc_file(ugenecl)
endif ()

target_link_libraries(ugenecl
        Qt5::Core Qt5::Gui
        ugenedb breakpad samtools
        U2Core U2Algorithm U2Formats U2Lang U2Gui U2Designer U2View U2Test)

