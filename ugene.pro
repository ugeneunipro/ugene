include( src/ugene_globals.pri )

# Check the Qt version. If QT_VERSION is not set, it is probably Qt 3.
isEmpty(QT_VERSION) {
    error("QT_VERSION not defined. Unipro UGENE does not work with Qt 3.")
}

!minQtVersion(5, 2, 1) {
    message("Cannot build Unipro UGENE with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.2.1.")
}


TEMPLATE = subdirs

CONFIG += ordered debug_and_release

use_bundled_zlib() {
    SUBDIRS += src/libs_3rdparty/zlib
}

SUBDIRS += \
          src/libs_3rdparty/breakpad \
          src/libs_3rdparty/qscore \
          src/libs_3rdparty/sqlite3 \
          src/libs_3rdparty/samtools \
          src/libs_3rdparty/QSpec \
          src/corelibs/U2Core \
          src/corelibs/U2Test \
          src/corelibs/U2Algorithm \
          src/corelibs/U2Formats \
          src/corelibs/U2Lang \
          src/corelibs/U2Private \
          src/corelibs/U2Gui \
          src/corelibs/U2View \
          src/corelibs/U2Designer \
          src/corelibs/U2Script \
          src/ugeneui \
          src/ugenecl \
          src/ugenem \
          src/plugins_checker \
          src/plugins_3rdparty/ball \
          src/plugins_3rdparty/sitecon \
          src/plugins_3rdparty/umuscle \
          src/plugins_3rdparty/hmm2 \
          src/plugins_3rdparty/gor4 \
          src/plugins_3rdparty/psipred \
          src/plugins_3rdparty/primer3 \
          src/plugins_3rdparty/phylip \
          src/plugins_3rdparty/kalign \
          src/plugins_3rdparty/ptools \
          src/plugins_3rdparty/variants \
          src/plugins/ngs_reads_classification \
          src/plugins/CoreTests \
          src/plugins/GUITestBase \
          src/plugins/annotator \
          src/plugins/api_tests \
          src/plugins/biostruct3d_view \
          src/plugins/browser_support \
          src/plugins/chroma_view \
          src/plugins/circular_view \
          src/plugins/clark_support \
          src/plugins/dbi_bam \
          src/plugins/diamond_support \
          src/plugins/dna_export \
          src/plugins/dna_flexibility \
          src/plugins/dna_graphpack \
          src/plugins/dna_stat \
          src/plugins/dotplot \
          src/plugins/enzymes \
          src/plugins/external_tool_support \
          src/plugins/genome_aligner \
          src/plugins/kraken_support \
          src/plugins/linkdata_support \
          src/plugins/orf_marker \
          src/plugins/pcr \
          src/plugins/perf_monitor \
          src/plugins/query_designer \
          src/plugins/remote_blast \
          src/plugins/repeat_finder \
          src/plugins/smith_waterman \
          src/plugins/test_runner \
          src/plugins/weight_matrix \
          src/plugins/wevote_support \
          src/plugins/workflow_designer

use_cuda() {
    SUBDIRS += src/plugins/cuda_support
}

use_opencl() {
    DEFINES += OPENCL_SUPPORT
    SUBDIRS += src/plugins/opencl_support
}

exclude_list_enabled() {
    SUBDIRS -= src/plugins/CoreTests
    SUBDIRS -= src/plugins/test_runner
    SUBDIRS -= src/plugins/perf_monitor
    SUBDIRS -= src/plugins/GUITestBase
    SUBDIRS -= src/plugins/api_tests
    SUBDIRS -= src/libs_3rdparty/QSpec
}

if(exists( ./src/libs_3rdparty/QSpec/QSpec.pro ):!exclude_list_enabled()) {
    message( "QSpec exists, enable GUI testing..." )
    !exists( ./src/libs_3rdparty/QSpec/custom.pri) {
        unix: system( cp ./installer/_common_data/QSpec_custom.pri ./src/libs_3rdparty/QSpec/custom.pri )
        win32: system (copy /B installer\_common_data\QSpec_custom.pri src\libs_3rdparty\QSpec\custom.pri)
    }
}
!exists( ./src/libs_3rdparty/QSpec/QSpec.pro ){
    DEFINES += HI_EXCLUDED
    SUBDIRS -= src/plugins/GUITestBase
    SUBDIRS -= src/libs_3rdparty/QSpec
}

without_non_free() {
    SUBDIRS -= src/plugins_3rdparty/psipred
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

#foreach 'language'
for( i, UGENE_TRANSL_IDX ) {
    UGENE_TRANSLATIONS =

    curTranslFile = $$member( UGENE_TRANSL_FILES, $$i )
    curTranslTag  = $$member( UGENE_TRANSL_TAG, $$i )

    #foreach project folder
    for( prj_dir, SUBDIRS ) {
        #look for file and add it to translation list if it exists
        translFile = $$prj_dir/$$UGENE_TRANSL_DIR/$$curTranslFile   # 'project/transl/english.ts' etc.
        exists( $$translFile ) {
            UGENE_TRANSLATIONS += $$translFile
#            system( $$UGENE_LUPDATE $$translFile ) FIXME
        }
    }
    !isEmpty(UGENE_LRELEASE) {
        for( targetDir, UGENE_TRANSL_QM_TARGET_DIR ) {
            targetQmFile = $$targetDir/transl_$$curTranslTag            # 'transl_en.qm' etc.
            targetQmFile = $$join( targetQmFile, , , .qm )              # special workaround for adding suffix started with '.'
            message( Generating traslations for language: $$curTranslTag )
            system( $$UGENE_LRELEASE $$UGENE_TRANSLATIONS -qm $$targetQmFile > $$UGENE_DEV_NULL )
        }
    } else {
        message( Cannot generate translations: no lrelease binary found )
    }
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

