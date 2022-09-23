include (../../ugene_globals.pri)


PLUGIN_ID=smith_waterman
PLUGIN_NAME=Smith-Waterman
PLUGIN_VENDOR=Unipro

include( ../../ugene_plugin_common.pri )

INCLUDEPATH += ../../corelibs/U2View/_tmp

win32-msvc2015 : DEFINES+=LAME_MSC
