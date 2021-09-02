PLUGIN_ID=biostruct3d_view
PLUGIN_NAME=3D structure viewer
PLUGIN_VENDOR=Unipro
PLUGIN_MODE=ui

include( ../../ugene_plugin_common.pri )

win32 : LIBS += opengl32.lib -lGLU32
unix_not_mac() : LIBS += -lGLU
