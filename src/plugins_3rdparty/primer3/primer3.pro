include(primer3.pri)

# Input
HEADERS += src/Primer3Dialog.h \
           src/Primer3Plugin.h \
           src/Primer3TaskSettings.h \
           src/Primer3Task.h \
           src/Primer3Tests.h \
           src/Primer3Query.h \
           src/primer3_core/boulder_input.h \
           src/primer3_core/dpal.h \
           src/primer3_core/oligotm.h \
           src/primer3_core/primer3.h \
           src/primer3_core/primer3_main.h \
    src/FindExonRegionsTask.h
FORMS += src/Primer3Dialog.ui
SOURCES += src/Primer3Dialog.cpp \
           src/Primer3Plugin.cpp \
           src/Primer3TaskSettings.cpp \
           src/Primer3Task.cpp \
           src/Primer3Tests.cpp \
           src/Primer3Query.cpp \
           src/primer3_core/boulder_input.c \
           src/primer3_core/dpal.c \
           src/primer3_core/oligotm.c \
           src/primer3_core/primer3.c \
           src/primer3_core/primer3_main.c \
    src/FindExonRegionsTask.cpp
RESOURCES += primer3.qrc
TRANSLATIONS += transl/russian.ts