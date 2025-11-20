project(U2Private)

set(CMAKE_AUTOMOC ON)

if (APPLE)
    find_package(Qt6 REQUIRED Core Gui Widgets Xml)
else ()
    find_package(Qt6 REQUIRED Core Gui Widgets)
endif ()

add_definitions(-DBUILDING_U2PRIVATE_DLL)

include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)
include_directories(../../libs_3rdparty/breakpad/src)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

if (WIN32)
    file(GLOB_RECURSE WIN_IGNORE_FILES src/*Mac.* src/*UnixNotMac.*)
    list(REMOVE_ITEM SRCS ${WIN_IGNORE_FILES})
    list(REMOVE_ITEM HDRS ${WIN_IGNORE_FILES})
elseif (APPLE)
    file(GLOB_RECURSE MAC_IGNORE_FILES src/*Win.* src/*UnixNotMac.*)
    list(REMOVE_ITEM SRCS ${MAC_IGNORE_FILES})
    list(REMOVE_ITEM HDRS ${MAC_IGNORE_FILES})
elseif (UNIX)
    file(GLOB_RECURSE LINUX_IGNORE_FILES src/*[er]Mac.* src/*Win.*)
    list(REMOVE_ITEM SRCS ${LINUX_IGNORE_FILES})
    list(REMOVE_ITEM HDRS ${LINUX_IGNORE_FILES})
endif ()

add_library(U2Private SHARED ${HDRS} ${SRCS})

target_link_libraries(U2Private
        Qt6::Core Qt6::Gui Qt6::Widgets
        breakpad
        U2Core U2Algorithm U2Formats)

if (APPLE)
    target_link_libraries(U2Private Qt6::Xml "-framework CoreFoundation -framework IOKit")
endif ()
