MODULE_ID=U2Designer
include( ../../ugene_lib_common.pri )

QT += svg xml

DEFINES+= QT_FATAL_ASSERT BUILDING_U2DESIGNER_DLL

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Lang$$D -lU2Gui$$D

DESTDIR = ../../$$out_dir()

