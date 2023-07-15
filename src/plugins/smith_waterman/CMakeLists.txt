set(UGENE_PLUGIN_NAME smith_waterman)

include(../../Plugin.cmake)

if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    list(REMOVE_ITEM SRCS "src/SmithWatermanAlgorithmSSE2.h")
    list(REMOVE_ITEM SRCS "src/SmithWatermanAlgorithmSSE2.cpp")
endif ()

