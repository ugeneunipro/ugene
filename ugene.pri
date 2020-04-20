include( src/ugene_globals.pri )

# Check the Qt version. If QT_VERSION is not set, it is probably Qt 3.
isEmpty(QT_VERSION) {
    error("QT_VERSION not defined. Unipro UGENE does not work with Qt 3.")
}

!minQtVersion(5, 3, 2) {
    message("Cannot build Unipro UGENE with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.3.2.")
}


TEMPLATE = subdirs

CONFIG += ordered debug_and_release

use_opencl() {
    DEFINES += OPENCL_SUPPORT
}

message("Qt version is $${QT_VERSION}")
if (useWebKit()) {
    message("WebKit is used as web engine")
} else {
    message("Qt WebEngine is used as web engine")
}

GUI_TESTING_ENABLED = 0
if (exists(./src/libs_3rdparty/QSpec/QSpec.pro): !exclude_list_enabled()) {
    if (!useWebKit()) {
        message ("QT WebEngine is used, GUI testing is disabled")
    } else {
        message( "GUI testing is enabled" )
        GUI_TESTING_ENABLED = 1
    }
}

!equals(GUI_TESTING_ENABLED, 1) {
    DEFINES += HI_EXCLUDED
}

# create target build & plugin folders (to copy licenses/descriptors to)
mkpath($$OUT_PWD/src/_debug/plugins)
mkpath($$OUT_PWD/src/_release/plugins)

!win32 {
    system( cp ./installer/_common_data/ugene $$OUT_PWD/src/_release/ugene )
    system( cp ./installer/_common_data/ugened $$OUT_PWD/src/_debug/ugened )
}


#prepare translations
UGENE_TRANSL_IDX   = 0          1
UGENE_TRANSL_FILES = russian.ts english.ts
UGENE_TRANSL_TAG   = ru         en

UGENE_TRANSL_DIR   = transl
UGENE_TRANSL_QM_TARGET_DIR = $$OUT_PWD/src/_debug $$OUT_PWD/src/_release

#detecting lrelease binary
win32 : UGENE_DEV_NULL = nul
unix : UGENE_DEV_NULL = /dev/null

UGENE_LRELEASE =
UGENE_LUPDATE =
message(Using QT from $$[QT_INSTALL_BINS])
system($$[QT_INSTALL_BINS]/lrelease-qt5 -version > $$UGENE_DEV_NULL 2> $$UGENE_DEV_NULL) {
    UGENE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease-qt5
    UGENE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate-qt5
} else : system($$[QT_INSTALL_BINS]/lrelease -version > $$UGENE_DEV_NULL 2> $$UGENE_DEV_NULL) {
    UGENE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    UGENE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

unix {
    system( chmod a+x ./src/gen_bin_script.cmd && ./src/gen_bin_script.cmd $$UGENE_INSTALL_DIR ugene > ugene; chmod a+x ugene )
    binscript.files += ugene
    binscript.path = $$UGENE_INSTALL_BINDIR

# to copy ugene executable to /usr/lib/ugene folder
    ugene_starter.files = ./src/_release/ugene
    ugene_starter.path = $$UGENE_INSTALL_DIR

    transl.files = ./src/_release/transl_en.qm
    transl.files += ./src/_release/transl_ru.qm
    transl.path = $$UGENE_INSTALL_DIR

    plugins.files = ./src/_release/plugins/*
    plugins.path = $$UGENE_INSTALL_DIR/plugins

    scripts.files += scripts/*
    scripts.path = $$UGENE_INSTALL_DIR/scripts

    data.files += data/*
    data.path = $$UGENE_INSTALL_DATA

    desktop.files += installer/_common_data/ugene.desktop
    desktop.path = $$UGENE_INSTALL_DESKTOP

    pixmaps.files += installer/_common_data/ugene.png installer/_common_data/ugene.xpm
    pixmaps.path = $$UGENE_INSTALL_PIXMAPS

    manual.files += installer/_common_data/ugene.1.gz
    manual.path = $$UGENE_INSTALL_MAN

    mime.files += installer/_common_data/application-x-ugene.xml
    mime.path = $$UGENE_INSTALL_MIME

    icons.files += installer/_common_data/application-x-ugene-ext.png
    icons.path = $$UGENE_INSTALL_ICONS/hicolor/32x32/mimetypes/


    INSTALLS += binscript ugene_starter transl plugins scripts data desktop pixmaps mime icons manual
}

