MODULE_ID=$${PLUGIN_ID}

include (ugene_lib_common.pri)

# This file is common for all UGENE plugins

QT += network xml svg

useWebKit() {
    QT += webkit
} else {
    QT += webengine
}

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Algorithm$$D -lU2Formats$$D -lU2Gui$$D -lU2View$$D -lU2Test$$D -lU2Lang$$D -lU2Designer$$D

DESTDIR=../../$$out_dir()/plugins
PLUGIN_ID=$$join(PLUGIN_ID, "", "", $$D)

!debug_and_release|build_pass {
    # Plugin output dir must exist before *.plugin/*.license files generation
    mkpath($$OUT_PWD)
    include (./ugene_plugin_descriptor.pri)
}

DEFINES += PLUGIN_ID=\\\"$${PLUGIN_ID}\\\"

win32 {
    QMAKE_MSVC_PROJECT_NAME=plugin_$${PLUGIN_ID}
    LIBS += psapi.lib
}

unix {
    target.path = $$UGENE_INSTALL_DIR/plugins
    INSTALLS += target
}

unix: {
    macx: {
        QMAKE_RPATHDIR += @executable_path/plugins/
    } else {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/plugins\'"
    }
}
