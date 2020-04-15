# include (qscore.pri)
include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG +=qt thread debug_and_release staticlib warn_off
QT += network xml script
INCLUDEPATH += src _tmp ../../include

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D
DESTDIR = ../../$$out_dir()
TARGET = qscore$$D
QMAKE_PROJECT_NAME = qscore

!debug_and_release|build_pass {

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
    }
}


UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_MSVC_PROJECT_NAME=lib_3rd_qscore

    LIBS += psapi.lib
}

unix: {
    macx: {
        QMAKE_RPATHDIR += @executable_path/
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
}

#unix {
#    target.path = $$UGENE_INSTALL_DIR/
#    INSTALLS += target
#}
