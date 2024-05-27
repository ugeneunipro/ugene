MODULE_ID=U2Algorithm
include( ../../ugene_lib_common.pri )

QT += widgets

DEFINES+= QT_FATAL_ASSERT BUILDING_U2ALGORITHM_DLL

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lsamtools$$D
LIBS += $$add_z_lib()

DESTDIR = ../../$$out_dir()

# Force re-linking when lib changes
unix:POST_TARGETDEPS += ../../$$out_dir()/libsamtools$${D}.a
# Same options which samtools is built with
DEFINES += "_FILE_OFFSET_BITS=64" PCRE2_CODE_UNIT_WIDTH=16 HAVE_CONFIG_H
win32 : DEFINES += PCRE2_STATIC PTW32_BUILD
INCLUDEPATH += ../../libs_3rdparty/samtools/src ../../libs_3rdparty/samtools/src/samtools_core/htslib
win32:INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools/win32

win32-msvc2013 {
    DEFINES += NOMINMAX
}

