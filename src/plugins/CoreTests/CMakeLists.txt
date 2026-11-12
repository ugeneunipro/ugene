set(UGENE_PLUGIN_ID CoreTests)
set(UGENE_PLUGIN_NAME "Tests for core UGENE functionality")

set(CMAKE_AUTOMOC_MOC_OPTIONS -nn) # Do not display notes.

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

if (WIN32)
    target_link_libraries(CoreTests psapi)
    add_executable(CoreTestsDump helpers/TimeoutDump.cpp)
    set_target_properties(CoreTestsDump PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)
    target_link_libraries(CoreTestsDump dbghelp)
    add_dependencies(CoreTests CoreTestsDump)
endif ()
