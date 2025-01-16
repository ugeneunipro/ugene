set(UGENE_PLUGIN_ID hmm2)
set(UGENE_PLUGIN_NAME HMM2)

file(GLOB_RECURSE SRCS src/*.cpp src/*.c src/*.h)

#if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
#    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/u_search/uhmmsearch_sse.h")
#    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/u_search/uhmmsearch_sse.cpp")
#endif ()

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)
