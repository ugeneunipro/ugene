include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG +=thread debug_and_release staticlib warn_off
INCLUDEPATH += src src/samtools ../../include
win32 : INCLUDEPATH += src/samtools/win32
DEFINES+="_FILE_OFFSET_BITS=64" _LARGEFILE64_SOURCE _USE_KNETFILE
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

unix: {
    macx: {
        QMAKE_RPATHDIR += @executable_path/
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
}

linux-g++ {
    # Original samtools package has multiple warnings like this.
    QMAKE_CXXFLAGS += -Wno-sign-compare
}

#unix {
#    target.path = $$UGENE_INSTALL_DIR/
#    INSTALLS += target
#}
