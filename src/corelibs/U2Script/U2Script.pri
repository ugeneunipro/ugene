MODULE_ID=U2Script

include( ../../ugene_lib_common.pri )

DEFINES += QT_FATAL_ASSERT BUILDING_U2SCRIPT_DLL

QT += network xml widgets

INCLUDEPATH +=      ../../include \
                    ../U2Private/src

#count( UGENE_NODE_DIR, 1 ) {
#    QMAKE_EXTENSION_SHLIB = node
#
#    INCLUDEPATH +=  $${UGENE_NODE_DIR}/src \
#                    $${UGENE_NODE_DIR}/deps/v8/include \
#                    $${UGENE_NODE_DIR}/deps/uv/include
#}

LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Algorithm$$D -lU2Formats$$D -lU2Lang$$D -lU2Private$$D -lU2Gui$$D -lU2Test$$D
LIBS += $$add_sqlite_lib()

DESTDIR = ../../$$out_dir()
