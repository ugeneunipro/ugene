# include (biostruct3d_view.pri)

PLUGIN_ID=biostruct3d_view
PLUGIN_NAME=3D structure viewer
PLUGIN_VENDOR=Unipro
PLUGIN_MODE=ui

include( ../../ugene_plugin_common.pri )

win32-msvc2013 {
    LIBS += opengl32.lib
}

win32 : LIBS += -lGLU32
unix_not_mac() : LIBS += -lGLU
