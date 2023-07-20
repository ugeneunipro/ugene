set(UGENE_PLUGIN_NAME umuscle)

include(../../Plugin.cmake)

target_link_libraries(${UGENE_PLUGIN_NAME} qscore)
