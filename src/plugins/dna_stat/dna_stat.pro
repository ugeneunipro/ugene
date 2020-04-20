include (dna_stat.pri)

# Input
HEADERS += src/DistanceMatrixMSAProfileDialog.h \
           src/DNAStatMSAProfileDialog.h \
           src/DNAStatPlugin.h
FORMS += src/DistanceMatrixMSAProfileDialog.ui \
         src/DNAStatMSAProfileDialog.ui
SOURCES += src/DistanceMatrixMSAProfileDialog.cpp \
           src/DNAStatMSAProfileDialog.cpp \
           src/DNAStatPlugin.cpp
TRANSLATIONS += transl/russian.ts
