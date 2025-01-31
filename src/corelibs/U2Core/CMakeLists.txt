project(U2Core)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Network Xml)

add_definitions(-DBUILDING_U2CORE_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Core SHARED ${HDRS} ${SRCS}
        src/util/CollectionUtils.h)

target_link_libraries(U2Core
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Xml
        ugenedb zlib
)

if (APPLE)
    target_link_libraries(U2Core "-framework CoreFoundation")
elseif (WIN32)
else ()
    target_link_libraries(U2Core
            ${CMAKE_SOURCE_DIR}/libqtscript.so
    )
endif ()
