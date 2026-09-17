set(UGENE_PLUGIN_ID ball)
set(UGENE_PLUGIN_NAME "BALL library")

include_directories(src/include)
include_directories(src/source)

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

if(MSVC)
    # BALL uses standard-library compatibility types removed in C++17.
    target_compile_definitions(ball PRIVATE _HAS_AUTO_PTR_ETC=1)
endif()
