# include (ugeneui.pri)

include( ../ugene_globals.pri )

use_opencl(){
    DEFINES += OPENCL_SUPPORT
}

QT += xml network script widgets

useWebKit() {
    QT += webkit webkitwidgets
} else {
    QT += webengine webenginewidgets webchannel websockets
}

TEMPLATE = app
CONFIG +=qt dll thread debug_and_release
macx : CONFIG -= app_bundle
DEFINES+= QT_DLL QT_FATAL_ASSERT
INCLUDEPATH += src _tmp ../include ../corelibs/U2Private/src ../libs_3rdparty/QSpec/src
macx : INCLUDEPATH += /System/Library/Frameworks/Security.framework/Headers

LIBS += -L../$$out_dir()
LIBS += -lU2Core$$D -lU2Designer$$D -lU2Algorithm$$D -lU2Formats$$D -lU2Gui$$D -lU2View$$D -lU2Test$$D -lU2Lang$$D -lU2Private$$D -lbreakpad$$D -lQSpec$$D
LIBS += $$add_sqlite_lib()

macx: LIBS += -framework Foundation /System/Library/Frameworks/Security.framework/Security
if (contains(DEFINES, HI_EXCLUDED)) {
    LIBS -= -lQSpec$$D
}

DESTDIR = ../$$out_dir()
TARGET = ugeneui$$D
QMAKE_PROJECT_NAME = ugeneui

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

        FORCE_CONSOLE = $$(UGENE_BUILD_WITH_CONSOLE)
        !isEmpty( FORCE_CONSOLE ) : CONFIG +=console
    }

}

UI_DIR=_tmp/ui
RCC_DIR=_tmp/rcc

win32 {
    LIBS += -lMinCore -lAdvapi32 -luser32     # to import CharToOemA with nmake build
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
    RC_FILE = ugeneui.rc
}

macx {
    RC_FILE = images/ugeneui.icns
    QMAKE_INFO_PLIST = ../../installer/macosx/Info.plist
    QMAKE_RPATHDIR += @executable_path/
}

unix {
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
    !macx: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

# Prepare version info for NSIS installer
win32 {
    !debug_and_release|build_pass {
        CONFIG(release, debug|release) {
            NSIS_LINE = "!define ProductVersion $${UGENE_VERSION}"
            NSIS_FILE = $${DESTDIR}/version.nsis
                        
            NSIS_LINE = $$replace(NSIS_LINE, "\\.","_")
            NSIS_LINE = $$replace(NSIS_LINE, "\\-","_")
            
            system (echo $${NSIS_LINE} > $${NSIS_FILE})
            
            NSIS_LINE = "!define PrintableVersion $${UGENE_VERSION}"
            system (echo $${NSIS_LINE} >> $${NSIS_FILE})
        }
    }
}
