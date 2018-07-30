#include (GUITestBase.pri)

PLUGIN_ID=GUITestBase
PLUGIN_NAME=GUI Test Base
PLUGIN_VENDOR=Unipro
include( ../../ugene_plugin_common.pri )

QT += testlib webkitwidgets

INCLUDEPATH += ../../corelibs/U2View/_tmp/ ../../libs_3rdparty/QSpec/src
LIBS +=-L../../_release -lQSpec

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        LIBS -= -L../../_release -lQSpec
        LIBS += -L../../_debug -lQSpecd
    }
}

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



