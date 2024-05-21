include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG +=thread debug_and_release staticlib warn_off
INCLUDEPATH += src src/samtools_core/htslib ../../include
# samtools specific
DEFINES += "_FILE_OFFSET_BITS=64" PCRE2_CODE_UNIT_WIDTH=16 HAVE_CONFIG_H
# pthread specific
win32 : DEFINES += PCRE2_STATIC PTW32_BUILD
#disables rand.c
unix : DEFINES += HAVE_DRAND48
win32 : DEFINES += _USE_MATH_DEFINES "__func__=__FUNCTION__" "R_OK=4" "atoll=_atoi64" "alloca=_alloca"

LIBS += -L../../$$out_dir()
LIBS += $$add_z_lib()
DESTDIR = ../../$$out_dir()
TARGET = samtools$$D
QMAKE_PROJECT_NAME = samtools

macx {
    DEFINES+="_CURSES_LIB=1"
    LIBS += -lcurses
}

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


win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
}

win32-msvc2013 {
    DEFINES += NOMINMAX _XKEYCHECK_H
}

win32 {
    !win32-msvc2015 {
        DEFINES += "inline=__inline"
    }
}

unix {
    macx {
        QMAKE_RPATHDIR += @executable_path/
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
}
