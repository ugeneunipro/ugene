set(UGENE_PLUGIN_ID smith_waterman)
set(UGENE_PLUGIN_NAME "Smith-Waterman")

file(GLOB_RECURSE SRCS src/*.cpp src/*.c src/*.h)

#if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
#    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/SmithWatermanAlgorithmSSE2.h")
#    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/SmithWatermanAlgorithmSSE2.cpp")
#endif ()

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
