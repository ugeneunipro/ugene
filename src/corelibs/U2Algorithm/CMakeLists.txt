project(U2Algorithm)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets)

add_definitions(-DBUILDING_U2ALGORITHM_DLL)

include_directories(src)
include_directories(../../include)
include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

add_library(U2Algorithm SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

target_link_libraries(U2Algorithm
        Qt5::Core Qt5::Gui Qt5::Widgets
        samtools
        U2Core)

