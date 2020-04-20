# include (U2Private.pri)

MODULE_ID=U2Private
include( ../../ugene_lib_common.pri )

QT += xml widgets network
DEFINES += QT_FATAL_ASSERT BUILDING_U2PRIVATE_DLL
LIBS += -L../../$$out_dir()
LIBS += -lU2Core$$D -lU2Formats$$D -lbreakpad$$D
INCLUDEPATH += ../../libs_3rdparty/breakpad/src
DESTDIR = ../../$$out_dir()

unix {
    target.path = $$UGENE_INSTALL_DIR/
    INSTALLS += target
}

freebsd {
    LIBS += -lexecinfo
}

win32 {
    LIBS += Advapi32.lib -lUser32

    contains(DEFINES, UGENE_X86_64) {
        ASM += src/crash_handler/StackRollbackX64.asm
        masm.name = MASM compiler
        masm.input = ASM
        masm.output = ${QMAKE_FILE_BASE}.obj
        masm.commands = ml64 /Fo ${QMAKE_FILE_OUT} /c ${QMAKE_FILE_IN}
        QMAKE_EXTRA_COMPILERS += masm
    }
}

win32-msvc2013 {
    DEFINES += NOMINMAX
}

macx {
    LIBS += -framework Foundation -framework IOKit
}
