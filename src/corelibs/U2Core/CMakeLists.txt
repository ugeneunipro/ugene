project(U2Core)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Network Xml Core5Compat)

add_definitions(-DBUILDING_U2CORE_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Core SHARED ${HDRS} ${SRCS}
        src/util/CollectionUtils.h)

set_source_files_properties(
    src/globals/Version.cpp PROPERTIES COMPILE_DEFINITIONS "U2_APP_VERSION=${UGENE_VERSION}"
)

target_link_libraries(U2Core
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Xml Qt6::Core5Compat
        ugenedb zlib
)

if (APPLE)
    target_link_libraries(U2Core "-framework CoreFoundation")
endif ()
