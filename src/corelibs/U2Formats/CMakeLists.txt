project(U2Formats)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Core5Compat)

add_definitions(-DBUILDING_U2FORMATS_DLL)

include(../../libs_3rdparty/samtools/Samtools-common.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Common.cmake)

include_directories(src)
include_directories(../../include)
include_directories(../../libs_3rdparty/samtools/src)
include_directories(../../libs_3rdparty/samtools/src/samtools_core/htslib)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS src/*.h)
qt_add_resources(RCC_SRCS U2Formats.qrc)

add_library(U2Formats SHARED ${HDRS} ${SRCS} ${RCC_SRCS})

# BAMUtils embeds legacy Samtools C sources that use writable char pointers
# for string literals. Keep the compatibility behavior for this target only.
if (MSVC)
    target_compile_options(U2Formats PRIVATE /Zc:strictStrings-)
endif ()

target_link_libraries(U2Formats
        Qt6::Core Qt6::Core5Compat Qt6::Gui Qt6::Widgets
        samtools ugenedb
        U2Core U2Algorithm)

