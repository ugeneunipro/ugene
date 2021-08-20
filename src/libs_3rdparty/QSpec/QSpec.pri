include( ../../ugene_globals.pri )

TARGET = QSpec$$D
QMAKE_PROJECT_NAME = QSpec
TEMPLATE = lib
CONFIG += thread debug_and_release warn_off qt dll
INCLUDEPATH += src _tmp
QT += testlib network xml svg sql widgets printsupport

DEFINES += BUILDING_QSPEC_DLL
DEFINES += QT_DLL

DESTDIR = ../../$$out_dir()

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG
        CONFIG +=console
        MOC_DIR=_tmp/moc/debug
        OBJECTS_DIR=_tmp/obj/debug
    }

    CONFIG(release, debug|release) {
        DEFINES+=NDEBUG
        MOC_DIR=_tmp/moc/release
        OBJECTS_DIR=_tmp/obj/release
    }
}


unix {
    !macx {
        LIBS += -lXtst
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
    macx {
        QMAKE_LFLAGS += -framework ApplicationServices
        LIBS += -framework AppKit
        QMAKE_RPATHDIR += @executable_path
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    }
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
}

win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_MSVC_PROJECT_NAME=lib_3rd_QSpec

    LIBS += User32.lib Gdi32.lib Advapi32.lib psapi.lib
    DEFINES += "PSAPI_VERSION=1"
}
