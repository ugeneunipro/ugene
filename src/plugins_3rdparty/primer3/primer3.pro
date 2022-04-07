include(primer3.pri)

# Input
HEADERS += src/Primer3Dialog.h \
           src/Primer3Plugin.h \
           src/Primer3TaskSettings.h \
           src/Primer3Task.h \
           src/Primer3Tests.h \
           src/Primer3Query.h \
		   src/primer3_core_new/dpal.h \
		   src/primer3_core_new/format_output.h \
		   src/primer3_core_new/libprimer3.h \
		   src/primer3_core_new/masker.h \
		   src/primer3_core_new/oligotm.h \
		   src/primer3_core_new/p3_seq_lib.h \
		   src/primer3_core_new/print_boulder.h \
		   src/primer3_core_new/read_boulder.h \
		   src/primer3_core_new/thal.h \
		   src/primer3_core_new/thal_parameters.h \
		   src/primer3_core_new/win/getopt.h \
		   src/primer3_core_new/win/mman.h \
		   src/primer3_core_new/win/unistd.h \
		   src/FindExonRegionsTask.h
	
FORMS += src/Primer3Dialog.ui
SOURCES += src/Primer3Dialog.cpp \
           src/Primer3Plugin.cpp \
           src/Primer3TaskSettings.cpp \
           src/Primer3Task.cpp \
           src/Primer3Tests.cpp \
           src/Primer3Query.cpp \
		   src/primer3_core_new/dpal.c \
		   src/primer3_core_new/format_output.c \
		   src/primer3_core_new/masker.c \
		   src/primer3_core_new/oligotm.c \
		   src/primer3_core_new/p3_seq_lib.c \
		   src/primer3_core_new/primer3_boulder_main.c \
		   src/primer3_core_new/print_boulder.c \
		   src/primer3_core_new/read_boulder.c \
		   src/primer3_core_new/thal.c \
		   src/primer3_core_new/thal_parameters.c \
		   src/primer3_core_new/libprimer3.cc \
		   src/FindExonRegionsTask.cpp
RESOURCES += primer3.qrc
TRANSLATIONS += transl/russian.ts