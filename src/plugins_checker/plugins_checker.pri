# include (ugenecl.pri)

include( ../ugene_globals.pri )

use_opencl(){
    DEFINES += OPENCL_SUPPORT
}

UGENE_RELATIVE_DESTDIR = ''

QT += xml network script webkit widgets
TEMPLATE = app
CONFIG +=qt dll thread debug_and_release console
CONFIG -= app_bundle
DEFINES+= QT_DLL QT_FATAL_ASSERT
INCLUDEPATH += src _tmp ../include ../corelibs/U2Private/src

LIBS += -L../_release -lU2Core -lU2Algorithm -lU2Formats -lU2Gui -lU2Private -lugenedb -lbreakpad

!debug_and_release|build_pass {

    CONFIG(debug, debug|release) {
        TARGET = plugins_checkerd
        DEFINES+=_DEBUG
        DESTDIR=../_debug
        MOC_DIR=_tmp/moc/debug
        OBJECTS_DIR=_tmp/obj/debug
        LIBS -= -L../_release -lU2Core -lU2Algorithm -lU2Formats -lU2Gui -lU2Private -lugenedb -lbreakpad
        LIBS += -L../_debug -lU2Cored -lU2Algorithmd -lU2Formatsd -lU2Guid -lU2Privated -lugenedbd -lbreakpadd
    }

    CONFIG(release, debug|release) {
        TARGET = plugins_checker
        DESTDIR=../_release
        DEFINES+=NDEBUG
        MOC_DIR=_tmp/moc/release
        OBJECTS_DIR=_tmp/obj/release
    }
}

UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    LIBS += -luser32     # to import CharToOemA with nmake build

    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
    RC_FILE = plugins_checker.rc
}

macx {
    RC_FILE = images/plugins_checker_mac.icns
    QMAKE_RPATHDIR += @executable_path
}

unix {
    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
    INSTALLS += target
}
