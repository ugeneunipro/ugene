include( src/ugene_globals.pri )

TEMPLATE = subdirs

CONFIG += ordered debug_and_release

message("Qt version is $${QT_VERSION}")
!versionAtLeast(QT_VERSION, 5.15.0):error("UGENE requires Qt version between 5.15.0 and 5.15.x")
!versionAtMost(QT_VERSION, 5.15.99):error("UGENE requires Qt version between 5.15.0 and 5.15.x")

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
    # Rules how to copy files on make install.
    ugene_build_dir.files = src/_release/*
    ugene_build_dir.path = $${PREFIX}

    data.files += data/*
    data.path = $${PREFIX}/data

    resources.files += LICENSE.3rd_party.txt
    resources.files += LICENSE.txt
    resources.files += etc/shared/application-x-ugene-ext.png
    resources.files += etc/shared/application-x-ugene.xml
    resources.files += etc/shared/ugene.1.gz
    resources.files += etc/shared/ugene.desktop
    resources.files += etc/shared/ugene.png etc/shared/ugene.xpm
    resources.path = $${PREFIX}

    INSTALLS += ugene_build_dir data resources
}

