MODULE_ID=U2Test
include( ../../ugene_lib_common.pri )

QT += xml gui widgets
DEFINES+= QT_FATAL_ASSERT BUILDING_U2TEST_DLL

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D

DESTDIR = ../../$$out_dir()


