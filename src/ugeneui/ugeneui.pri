include( ../ugene_globals.pri )

use_opencl(){
    DEFINES += OPENCL_SUPPORT
}

QT += xml network script widgets

TEMPLATE = app
CONFIG +=qt dll thread debug_and_release
macx : CONFIG -= app_bundle
unix:!macx:!clang:g++: QMAKE_LFLAGS += --no-pie
unix:!macx:clang: QMAKE_LFLAGS += -fno-pie
DEFINES+= QT_DLL QT_FATAL_ASSERT
INCLUDEPATH += src _tmp ../include ../corelibs/U2Private/src
macx : INCLUDEPATH += /System/Library/Frameworks/Security.framework/Headers

LIBS += -L../$$out_dir()
LIBS += -lU2Core$$D -lU2Designer$$D -lU2Algorithm$$D -lU2Formats$$D -lU2Gui$$D -lU2View$$D -lU2Test$$D -lU2Lang$$D -lU2Private$$D -lbreakpad$$D
LIBS += $$add_sqlite_lib()

macx {
    exists( /System/Library/Frameworks/Security.framework/Security ) {
        LIBS += -framework Foundation /System/Library/Frameworks/Security.framework/Security
    } else {
        LIBS += -framework Foundation
    }
}

DESTDIR = ../$$out_dir()
TARGET = ugeneui$$D
QMAKE_PROJECT_NAME = ugeneui

CONFIG(debug, debug|release) {
    DEFINES+=_DEBUG
    CONFIG +=console
    MOC_DIR=_tmp/moc/debug
    OBJECTS_DIR=_tmp/obj/debug
}

CONFIG(release, debug|release) {
    DEFINES+=NDEBUG
    MOC_DIR=_tmp/moc/release
    OBJECTS_DIR=_tmp/obj/release

    FORCE_CONSOLE = $$(UGENE_BUILD_WITH_CONSOLE)
    !isEmpty( FORCE_CONSOLE ) : CONFIG +=console
}

UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    LIBS += -luser32    # to import CharToOemA with nmake build
    LIBS += -lole32     # to import CoCreateInstance with nmake build
    LIBS += -lshell32   # to import SHGetSpecialFolderPathA with nmake build
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
    RC_FILE = ugeneui.rc
}

macx {
    RC_FILE = images/ugeneui.icns
    QMAKE_RPATHDIR += @executable_path/
}

unix {
    !macx: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
