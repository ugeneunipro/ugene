include( src/ugene_globals.pri )

isEmpty(QT_VERSION) {
    error("QT_VERSION is not defined.")
}

!minQtVersion(5, 12, 0) {
    message("Cannot build Unipro UGENE with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.12.0.")
}

TEMPLATE = subdirs

CONFIG += ordered debug_and_release

use_opencl() {
    DEFINES += OPENCL_SUPPORT
}

message("Qt version is $${QT_VERSION}")

# create target build & plugin folders (to copy licenses/descriptors to)
mkpath($$OUT_PWD/src/_debug/plugins)
mkpath($$OUT_PWD/src/_release/plugins)

!win32 {
    system( cp ./etc/shared/ugene $$OUT_PWD/src/_release/ugene )
    system( cp ./etc/shared/ugened $$OUT_PWD/src/_debug/ugened )
}


#prepare translations
UGENE_TRANSL_IDX   = 0  1
UGENE_TRANSL_FILES = russian.ts turkish.ts
UGENE_TRANSL_TAG   = ru tr

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
    transl.files += ./src/_release/transl_tr.qm
    transl.path = $$UGENE_INSTALL_DIR

    plugins.files = ./src/_release/plugins/*
    plugins.path = $$UGENE_INSTALL_DIR/plugins

    scripts.files += scripts/*
    scripts.path = $$UGENE_INSTALL_DIR/scripts

    data.files += data/*
    data.path = $$UGENE_INSTALL_DATA

    desktop.files += etc/share/ugene.desktop
    desktop.path = $$UGENE_INSTALL_DESKTOP

    pixmaps.files += etc/shared/ugene.png etc/shared/ugene.xpm
    pixmaps.path = $$UGENE_INSTALL_PIXMAPS

    manual.files += etc/shared/ugene.1.gz
    manual.path = $$UGENE_INSTALL_MAN

    mime.files += etc/shared/application-x-ugene.xml
    mime.path = $$UGENE_INSTALL_MIME

    icons.files += etc/shared/application-x-ugene-ext.png
    icons.path = $$UGENE_INSTALL_ICONS/hicolor/32x32/mimetypes/


    INSTALLS += binscript ugene_starter transl plugins scripts data desktop pixmaps mime icons manual
}

