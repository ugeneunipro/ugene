include( ../../ugene_globals.pri )

TEMPLATE = lib
CONFIG +=thread debug_and_release staticlib
DEFINES+= _CRT_SECURE_NO_WARNINGS
INCLUDEPATH += src

TARGET = zlib$$D
DESTDIR = ../../$$out_dir()

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        DEFINES+=_DEBUG
        CONFIG +=console
        OBJECTS_DIR=_tmp/obj/debug
        MOC_DIR=_tmp/moc/debug
    }

    CONFIG(release, debug|release) {
        DEFINES+=NDEBUG
        OBJECTS_DIR=_tmp/obj/release
        MOC_DIR=_tmp/moc/release
    }
}

UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3

    QMAKE_CXXFLAGS+=/wd4996
    QMAKE_MSVC_PROJECT_NAME=lib_3rd_zlib
}
win32-msvc2015 {
	DEFINES += _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
	DEFINES += _XKEYCHECK_H
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
