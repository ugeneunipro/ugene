project(QSpec)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Test)

add_definitions(-DBUILDING_QSPEC_DLL)

include_directories(src)
include_directories(../../include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)

list(FILTER SRCS EXCLUDE REGEX "src/primitives/GTWeb*")
list(FILTER HDRS EXCLUDE REGEX "src/primitives/GTWeb*")

add_library(QSpec SHARED ${HDRS} ${SRCS})

target_link_libraries(QSpec
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test
        Xtst)

