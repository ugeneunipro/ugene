# include (U2Algorithm.pri)

MODULE_ID=U2Algorithm
include( ../../ugene_lib_common.pri )

QT += widgets

use_opencl(){
    DEFINES += OPENCL_SUPPORT
}

DEFINES+= QT_FATAL_ASSERT BUILDING_U2ALGORITHM_DLL

unix: QMAKE_CXXFLAGS += -Wno-char-subscripts

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lsamtools$$D
LIBS += $$add_z_lib()

DESTDIR = ../../$$out_dir()

# Force re-linking when lib changes
unix:POST_TARGETDEPS += ../../$$out_dir()/libsamtools$${D}.a
# Same options which samtools is built with
DEFINES+="_FILE_OFFSET_BITS=64" _LARGEFILE64_SOURCE _USE_KNETFILE
INCLUDEPATH += ../../libs_3rdparty/samtools/src ../../libs_3rdparty/samtools/src/samtools
win32:INCLUDEPATH += ../../libs_3rdparty/samtools/src/samtools/win32
win32:LIBS+=-lws2_32

win32-msvc2013 {
    DEFINES += NOMINMAX
}

unix {
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
}
