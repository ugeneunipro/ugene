include(primer3.pri)

# Input
HEADERS += src/Primer3Dialog.h \
           src/Primer3Plugin.h \
           src/Primer3Query.h \
           src/Primer3Tests.h \
           src/primer3_core/dpal.h \
           src/primer3_core/format_output.h \
           src/primer3_core/libprimer3.h \
           src/primer3_core/masker.h \
           src/primer3_core/oligotm.h \
           src/primer3_core/p3_seq_lib.h \
           src/primer3_core/primer3_boulder_main.h \
           src/primer3_core/print_boulder.h \
           src/primer3_core/read_boulder.h \
           src/primer3_core/thal.h \
           src/primer3_core/thal_parameters.h \
           src/primer3_core/win/getopt.h \
           src/primer3_core/win/mman.h \
           src/primer3_core/win/unistd.h \
           src/task/CheckComplementTask.h \
           src/task/FindExonRegionsTask.h \
           src/task/Primer3Task.h \
           src/task/Primer3TaskSettings.h \
           src/task/Primer3TopLevelTask.h \
           src/task/PrimerPair.h \
           src/task/PrimerSingle.h \
           src/task/ProcessPrimer3ResultsToAnnotationsTask.h \
           src/temperature/Primer3TmCalculator.h \
           src/temperature/Primer3TmCalculatorFactory.h \
           src/temperature/Primer3TmCalculatorSettingsWidget.h
	
FORMS += src/Primer3Dialog.ui \
         src/temperature/Primer3TmCalculatorSettingsWidget.ui
          
SOURCES += src/Primer3Dialog.cpp \
           src/Primer3Plugin.cpp \
           src/Primer3Query.cpp \
           src/Primer3Tests.cpp \
           src/primer3_core/dpal.c \
           src/primer3_core/format_output.c \
           src/primer3_core/masker.c \
           src/primer3_core/oligotm.c \
           src/primer3_core/p3_seq_lib.c \
           src/primer3_core/primer3_boulder_main.c \
           src/primer3_core/print_boulder.c \
           src/primer3_core/read_boulder.c \
           src/primer3_core/thal.c \
           src/primer3_core/thal_parameters.c \
           src/primer3_core/libprimer3.cc \
           src/task/CheckComplementTask.cpp \
           src/task/FindExonRegionsTask.cpp \
           src/task/Primer3Task.cpp \
           src/task/Primer3TaskSettings.cpp \
           src/task/Primer3TopLevelTask.cpp \
           src/task/PrimerPair.cpp \
           src/task/PrimerSingle.cpp \
           src/task/ProcessPrimer3ResultsToAnnotationsTask.cpp \
           src/temperature/Primer3TmCalculator.cpp \
           src/temperature/Primer3TmCalculatorFactory.cpp \
           src/temperature/Primer3TmCalculatorSettingsWidget.cpp
RESOURCES += primer3.qrc
TRANSLATIONS += transl/russian.ts
