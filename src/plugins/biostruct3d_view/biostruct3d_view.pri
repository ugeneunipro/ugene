# include (biostruct3d_view.pri)

defineTest( use_deprecated_view ) {
    !macx: return (true);
    equals(QT_MAJOR_VERSION, 5) : lessThan(QT_MINOR_VERSION, 4) {
        return (true)
    }
    lessThan(QT_MAJOR_VERSION, 5) {
        return (true)
    }
    return (false)
}

use_deprecated_view() {
    QT += opengl
}

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
