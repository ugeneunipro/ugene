include(ugene.pri)

use_bundled_zlib() {
    SUBDIRS += src/libs_3rdparty/zlib
}

use_bundled_sqlite() {
    SUBDIRS += src/libs_3rdparty/sqlite3
}

SUBDIRS += \
          src/libs_3rdparty/breakpad \
          src/libs_3rdparty/qscore \
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
          src/plugins/metaphlan2_support \
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

!equals(GUI_TESTING_ENABLED, 1) {
    SUBDIRS -= src/plugins/GUITestBase
    SUBDIRS -= src/libs_3rdparty/QSpec
}

without_non_free() {
    SUBDIRS -= src/plugins_3rdparty/psipred
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
