MODULE_ID=U2Private
include( ../../ugene_lib_common.pri )

QT += xml widgets network
DEFINES += QT_FATAL_ASSERT BUILDING_U2PRIVATE_DLL
LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Formats$$D -lbreakpad$$D
INCLUDEPATH += ../../libs_3rdparty/breakpad/src
DESTDIR = ../../$$out_dir()

freebsd {
    LIBS += -lexecinfo
}

win32 {
    LIBS += Advapi32.lib -lUser32
}

win32-msvc2013 {
    DEFINES += NOMINMAX
}

macx {
    LIBS += -framework Foundation -framework IOKit
}
