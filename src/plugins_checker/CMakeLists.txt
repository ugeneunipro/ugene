project(plugins_checker)

find_package(Qt5 REQUIRED Core Gui Widgets)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../include)
include_directories(../corelibs/U2Private/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_executable(plugins_checker ${HDRS} ${SRCS})
set_target_properties(plugins_checker PROPERTIES DEBUG_POSTFIX "d")

if (WIN32)
    add_ugene_rc_file(plugins_checker)
endif ()


target_link_libraries(plugins_checker
        Qt5::Core Qt5::Gui Qt5::Widgets
        U2Core U2Lang U2Private
)
