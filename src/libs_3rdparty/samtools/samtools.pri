include( ../../ugene_globals.pri )
UGENE_RELATIVE_DESTDIR = ''

TEMPLATE = lib
CONFIG +=thread debug_and_release staticlib warn_off
INCLUDEPATH += src src/samtools ../../include
win32 : INCLUDEPATH += src/samtools/win32
DEFINES+="_FILE_OFFSET_BITS=64" _LARGEFILE64_SOURCE _USE_KNETFILE
win32 : DEFINES += _USE_MATH_DEFINES "__func__=__FUNCTION__" "R_OK=4" "atoll=_atoi64" "alloca=_alloca"
LIBS += -L../../_release

use_bundled_zlib() {
    LIBS += -lzlib
} else {
    LIBS += -lz
}

macx {
    DEFINES+="_CURSES_LIB=1"
    LIBS += -lcurses
}

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        TARGET = samtoolsd
        DEFINES+=_DEBUG
        CONFIG +=console
        DESTDIR=../../_debug/
        MOC_DIR=_tmp/moc/debug
        OBJECTS_DIR=_tmp/obj/debug
        LIBS -= -L../../_release
        LIBS += -L../../_debug
        use_bundled_zlib() {
            LIBS -= -lzlib
            LIBS += -lzlibd
        }
    }

    CONFIG(release, debug|release) {
        TARGET = samtools
        DEFINES+=NDEBUG
        DESTDIR=../../_release/
        MOC_DIR=_tmp/moc/release
        OBJECTS_DIR=_tmp/obj/release
    }
}


win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_MSVC_PROJECT_NAME=lib_3rd_samtools
}

win32-msvc2013 {
    DEFINES += NOMINMAX _XKEYCHECK_H
}

win32 {
    !win32-msvc2015 {
        DEFINES += "inline=__inline"
    }
}

macx {
    QMAKE_RPATHDIR += @executable_path
    QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
}

#unix {
#    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
#    INSTALLS += target
#}
