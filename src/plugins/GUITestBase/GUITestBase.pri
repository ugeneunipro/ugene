#include (GUITestBase.pri)

PLUGIN_ID=GUITestBase
PLUGIN_NAME=GUI Test Base
PLUGIN_VENDOR=Unipro
include( ../../ugene_plugin_common.pri )

QT += testlib
useWebKit() {
    QT += webkitwidgets
} else {
    QT += webenginewidgets
}


INCLUDEPATH += ../../corelibs/U2View/_tmp/ ../../libs_3rdparty/QSpec/src
LIBS += -lQSpec$$D

unix {
    !macx {
    LIBS += -lXtst
    }
    macx {
    QMAKE_LFLAGS += -framework ApplicationServices
    }
}

win32 {
    LIBS += User32.lib Gdi32.lib
    QMAKE_CXXFLAGS += -bigobj
}

macx {
    LIBS += -framework AppKit
}



