PLUGIN_ID=GUITestBase
PLUGIN_NAME=GUI Test Base
PLUGIN_VENDOR=Unipro
include( ../../ugene_plugin_common.pri )

QT += testlib
INCLUDEPATH += ../../corelibs/U2View/_tmp/ ../../libs_3rdparty/QSpec/src
LIBS += -lQSpec$$D

unix {
    macx {
        QMAKE_LFLAGS += -framework ApplicationServices
    }
}

win32 {
    LIBS += User32.lib Gdi32.lib -lole32 -lShell32
    QMAKE_CXXFLAGS += -bigobj
}

macx {
    LIBS += -framework AppKit
}

