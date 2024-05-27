PLUGIN_ID=dbi_bam
PLUGIN_NAME=BAM file based DBI implementation
PLUGIN_VENDOR=Unipro

include( ../../ugene_plugin_common.pri )

LIBS += -lsamtools$$D
LIBS += $$add_z_lib()
LIBS += $$add_sqlite_lib()

win32-msvc2013|win32-msvc2015|greaterThan(QMAKE_MSC_VER, 1909) {
    DEFINES += NOMINMAX _XKEYCHECK_H
}

# Force re-linking when lib changes
unix:POST_TARGETDEPS += ../../$$out_dir()/libsamtools$${D}.a
# Same options which samtools is built with
DEFINES += "_FILE_OFFSET_BITS=64" PCRE2_CODE_UNIT_WIDTH=16 HAVE_CONFIG_H
win32 : DEFINES += PCRE2_STATIC PTW32_BUILD
INCLUDEPATH += ../../libs_3rdparty/samtools/src ../../libs_3rdparty/samtools/src/samtools_core/htslib
win32:INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools/win32
win32:DEFINES += _USE_MATH_DEFINES "__func__=__FUNCTION__" "R_OK=4" "atoll=_atoi64" "alloca=_alloca"

win32 {
    !win32-msvc2015 {
        DEFINES += "inline=__inline"
    }
}

INCLUDEPATH += ../../libs_3rdparty/sqlite3/src
INCLUDEPATH += ../../libs_3rdparty/samtools/src ../../libs_3rdparty/samtools/src/samtools
