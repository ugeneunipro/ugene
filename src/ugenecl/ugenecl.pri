include( ../ugene_globals.pri )

QT += xml network script widgets
TEMPLATE = app
CONFIG +=qt dll thread debug_and_release console
macx : CONFIG -= app_bundle
unix:!macx:!clang:g++: QMAKE_LFLAGS += --no-pie
unix:!macx:clang: QMAKE_LFLAGS += -fno-pie
DEFINES+= QT_DLL QT_FATAL_ASSERT
INCLUDEPATH += src _tmp ../include ../corelibs/U2Private/src

LIBS += -L../$$out_dir()
LIBS += -lU2Core$$D -lU2Algorithm$$D -lU2Designer$$D -lU2Formats$$D -lU2Gui$$D -lU2Test$$D -lU2Lang$$D -lU2Private$$D -lbreakpad$$D
LIBS += $$add_sqlite_lib()

DESTDIR = ../$$out_dir()
TARGET = ugenecl$$D
QMAKE_PROJECT_NAME = ugenecl

CONFIG(debug, debug|release) {
    DEFINES+=_DEBUG
    MOC_DIR=_tmp/moc/debug
    OBJECTS_DIR=_tmp/obj/debug
}

CONFIG(release, debug|release) {
    DEFINES+=NDEBUG
    MOC_DIR=_tmp/moc/release
    OBJECTS_DIR=_tmp/obj/release
}

UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    LIBS += -luser32     # to import CharToOemA with nmake build

    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
    RC_FILE = ugenecl.rc
}

macx {
    RC_FILE = images/ugenecl_mac.icns
    QMAKE_RPATHDIR += @executable_path/
}

unix {
    !macx: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
