include( ../../ugene_globals.pri )
UGENE_RELATIVE_DESTDIR = ''

TEMPLATE = lib
CONFIG +=thread debug_and_release staticlib
DEFINES+= _CRT_SECURE_NO_WARNINGS
INCLUDEPATH += src

TARGET = zlib
DESTDIR=../../_release

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        TARGET = zlibd
        DEFINES+=_DEBUG
        CONFIG +=console
        DESTDIR=../../_debug
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

macx {
    QMAKE_RPATHDIR += @executable_path
    QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
}

#unix {
#    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
#    INSTALLS += target
#}
