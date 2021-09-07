include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG +=thread debug_and_release warn_off
INCLUDEPATH += src

# RTree index is used by UGENE to optimize access to annotations & NGS reads.
# See https://www.sqlite.org/rtree.html
DEFINES+=SQLITE_ENABLE_RTREE

# This option disables the entire extension loading mechanism from SQLite.
# No sqlite extensions are used by UGENE today.
DEFINES+=SQLITE_OMIT_LOAD_EXTENSION

LIBS += -L../../$$out_dir()
DESTDIR = ../../$$out_dir()
TARGET = ugenedb$$D

CONFIG(debug, debug|release) {
    DEFINES+=_DEBUG
    CONFIG +=console
    OBJECTS_DIR=_tmp/obj/debug
}

CONFIG(release, debug|release) {
    DEFINES+=NDEBUG
    OBJECTS_DIR=_tmp/obj/release
}

win32 {
    DEF_FILE=$$PWD/src/sqlite3.def

    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_MSVC_PROJECT_NAME=lib_3rd_sqlite3
}


unix {
    macx {
        QMAKE_RPATHDIR += @executable_path/
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
}
