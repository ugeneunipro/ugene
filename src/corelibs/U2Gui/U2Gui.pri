MODULE_ID=U2Gui
include( ../../ugene_lib_common.pri )

QT += network xml svg widgets printsupport
DEFINES+= QT_FATAL_ASSERT BUILDING_U2GUI_DLL
INCLUDEPATH += ../U2Private/src

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Formats$$D -lU2Private$$D

DESTDIR = ../../$$out_dir()

