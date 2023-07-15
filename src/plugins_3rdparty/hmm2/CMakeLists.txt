set(UGENE_PLUGIN_NAME hmm2)

include(../../Plugin.cmake)

if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    list(REMOVE_ITEM SRCS "src/u_search/uhmmsearch_sse.h")
    list(REMOVE_ITEM SRCS "src/u_search/uhmmsearch_sse.cpp")
endif ()
