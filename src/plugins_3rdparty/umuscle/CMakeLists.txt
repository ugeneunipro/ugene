set(UGENE_PLUGIN_ID umuscle)
set(UGENE_PLUGIN_NAME "Muscle3")

include(${CMAKE_SOURCE_DIR}/cmake/Plugin.cmake)

target_link_libraries(${UGENE_PLUGIN_ID} qscore)
