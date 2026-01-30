project(mtdna)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../include)
include_directories(../corelibs/U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

if (APPLE)
    file(GLOB_RECURSE OBJC_SRCS src/*.mm)
endif ()

set(EXTRA_FLAGS "")
if (WIN32 AND NOT DEFINED ENV{UGENE_BUILD_WITH_CONSOLE})
    set(EXTRA_FLAGS "WIN32") # Build with SUBSYSTEM:Windows
endif ()

add_executable(mtdna ${EXTRA_FLAGS} ${HDRS} ${SRCS} ${OBJC_SRCS} ${QM_FILES})
set_target_properties(mtdna PROPERTIES DEBUG_POSTFIX "d")

target_link_libraries(mtdna
        Qt5::Core Qt5::Gui Qt5::Widgets
        U2Core U2Algorithm U2Gui U2View U2Private)

if (APPLE)
    target_link_libraries(mtdna
            "-framework AppKit"
            "-framework Foundation"
            "-framework Security"
            "-framework CoreFoundation"
            "-framework IOKit"
    )
endif ()

