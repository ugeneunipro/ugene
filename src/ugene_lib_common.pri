# Include global section if needed
isEmpty(UGENE_GLOBALS_DEFINED) {
    include( ugene_globals.pri )
}

# This file is common for all UGENE modules

TEMPLATE = lib
CONFIG +=qt dll thread debug_and_release
macx : CONFIG -=plugin
DEFINES+= QT_DLL
QT += script
INCLUDEPATH += src _tmp ../../include

# Visual Studio project file name
QMAKE_PROJECT_NAME=$${MODULE_ID}

MODULE_ID=$$join(MODULE_ID, "", "", $$D)
TARGET = $${MODULE_ID}
CONFDIR=$$out_dir()

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

# Special compiler flags for windows configuration
win32 {
    QMAKE_CXXFLAGS_WARN_ON = -W3
    QMAKE_CFLAGS_WARN_ON = -W3
}

unix: {
    macx: {
        QMAKE_RPATHDIR += @executable_path/
        QMAKE_LFLAGS_SONAME = -Wl,-dylib_install_name,@rpath/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    }
}
