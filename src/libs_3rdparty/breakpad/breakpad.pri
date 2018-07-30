include( ../../ugene_globals.pri )
UGENE_RELATIVE_DESTDIR = ''

TEMPLATE = lib
CONFIG += thread debug_and_release warn_off
INCLUDEPATH += src
TARGET = breakpad
DESTDIR = ../../_release
QT -= gui

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = breakpadd
        DEFINES += _DEBUG
        CONFIG += console
        DESTDIR = ../../_debug
        OBJECTS_DIR = _tmp/obj/debug
    }

    CONFIG(release, debug|release) {
        DEFINES += NDEBUG
        OBJECTS_DIR = _tmp/obj/release
    }
}

unix {
    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
    INSTALLS += target
}

macx {
    LIBS += -framework CoreServices
    DEFINES += __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__=1050
    QMAKE_RPATHDIR += @executable_path
    QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
}
