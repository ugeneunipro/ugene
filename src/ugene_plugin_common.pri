MODULE_ID=$${PLUGIN_ID}
include (ugene_lib_common.pri)

# This file is common for all UGENE plugins

UGENE_RELATIVE_DESTDIR = 'plugins'
QT += network xml webkit svg
LIBS += -L../../_release -lU2Core -lU2Algorithm -lU2Formats -lU2Gui -lU2View -lU2Test -lU2Lang -lU2Designer

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        PLUGIN_ID=$$join(PLUGIN_ID, "", "", "d")
        DESTDIR=../../_debug/plugins
        LIBS -= -L../../_release -lU2Core -lU2Algorithm -lU2Formats -lU2Gui -lU2View -lU2Test -lU2Lang -lU2Designer
        LIBS += -L../../_debug -lU2Cored -lU2Algorithmd -lU2Formatsd -lU2Guid -lU2Viewd -lU2Testd -lU2Langd -lU2Designerd
    }
    CONFIG(release, debug|release) {
        DESTDIR=../../_release/plugins
    }

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
    target.path = $$UGENE_INSTALL_DIR/$$UGENE_RELATIVE_DESTDIR
    INSTALLS += target
}

macx {
    QMAKE_RPATHDIR += @executable_path/$${UGENE_RELATIVE_DESTDIR}
}
