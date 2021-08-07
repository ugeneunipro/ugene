MODULE_ID=U2Core
include( ../../ugene_lib_common.pri )

QT += network xml widgets

DEFINES+=UGENE_MIN_VERSION_SQLITE=$${UGENE_MIN_VERSION_SQLITE}
DEFINES+=UGENE_MIN_VERSION_MYSQL=$${UGENE_MIN_VERSION_MYSQL}
DEFINES+=QT_FATAL_ASSERT BUILDING_U2CORE_DLL

LIBS += $$add_z_lib()
LIBS += $$add_sqlite_lib()
LIBS += -L../../$$out_dir()

DESTDIR = ../../$$out_dir()

# Special compiler flags for windows configuration
win32 {
    LIBS += User32.lib
}

unix {
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
}
