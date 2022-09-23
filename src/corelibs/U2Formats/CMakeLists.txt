project(U2Formats)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets)

add_definitions(-DBUILDING_U2FORMATS_DLL)

include_directories(src)
include_directories(../../include)
include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)
qt5_add_resources(RCC_SRCS U2Formats.qrc)

add_library(U2Formats SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Formats
        Qt5::Core Qt5::Gui Qt5::Widgets
        samtools ugenedb
        U2Core U2Algorithm)

