if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UGENE_BUILD_MODE_SUFFIX "d")
else ()
    set(UGENE_BUILD_MODE_SUFFIX "")
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/CollectTranslations.cmake)
