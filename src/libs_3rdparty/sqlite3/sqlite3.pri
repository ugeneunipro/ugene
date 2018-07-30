# include (sqlite.pri)

include( ../../ugene_globals.pri )
UGENE_RELATIVE_DESTDIR = ''

TARGET = ugenedb
TEMPLATE = lib
CONFIG +=thread debug_and_release warn_off
INCLUDEPATH += src
DEFINES+=SQLITE_ENABLE_COLUMN_METADATA
DEFINES+=SQLITE_ENABLE_RTREE
unix:DEFINES+=SQLITE_OMIT_LOAD_EXTENSION
DEFINES+=THREADSAFE
LIBS += -L../../_release

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        TARGET = ugenedbd
        DEFINES+=_DEBUG
        CONFIG +=console
        DESTDIR=../../_debug/
        OBJECTS_DIR=_tmp/obj/debug
        LIBS -= -L../../_release 
        LIBS += -L../../_debug
    }

    CONFIG(release, debug|release) {
        TARGET = ugenedb
        DEFINES+=NDEBUG
        DESTDIR=../../_release/
        OBJECTS_DIR=_tmp/obj/release
    }
}


win32 {
    DEF_FILE=$$PWD/src/sqlite3.def

    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_MSVC_PROJECT_NAME=lib_3rd_sqlite3
}


unix {
    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
    INSTALLS += target
}

macx {
    QMAKE_RPATHDIR += @executable_path
    QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
}
