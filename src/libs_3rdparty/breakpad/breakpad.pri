include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG += thread debug_and_release warn_off
INCLUDEPATH += src
TARGET = breakpad$$D
DESTDIR = ../../$$out_dir()
QMAKE_PROJECT_NAME = breakpad
QT -= gui

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG
        CONFIG += console
        OBJECTS_DIR = _tmp/obj/debug
    }

    CONFIG(release, debug|release) {
        DEFINES += NDEBUG
        OBJECTS_DIR = _tmp/obj/release
    }
}

unix {
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
    !macx: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

macx {
    LIBS += -framework CoreServices
    DEFINES += __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__=1050
    QMAKE_RPATHDIR += @executable_path/
    QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
}

win32 {
    LIBS += psapi.lib
}
