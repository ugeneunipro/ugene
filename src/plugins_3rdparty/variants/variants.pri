PLUGIN_ID=variants
PLUGIN_NAME=CallVariants
PLUGIN_VENDOR=Unipro
CONFIG += warn_off
include( ../../ugene_plugin_common.pri )

DEFINES+="_FILE_OFFSET_BITS=64" _LARGEFILE64_SOURCE
win32 : DEFINES += _USE_MATH_DEFINES "__func__=__FUNCTION__" "R_OK=4" "atoll=_atoi64" "alloca=_alloca"

win32 {
    # not visual studio 2015
    !win32-msvc2015 {
        DEFINES += "inline=__inline"
    }
}

INCLUDEPATH += src ../../include
INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools
INCLUDEPATH += ../../libs_3rdparty/samtools/src
INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools/bcftools
win32 : INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools/win32

LIBS += $$add_z_lib()
LIBS += -lsamtools$$D

win32-msvc2013 {
    DEFINES += _XKEYCHECK_H
}
