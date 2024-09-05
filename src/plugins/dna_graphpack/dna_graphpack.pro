include (dna_graphpack.pri)

# Input
HEADERS += src/BaseContentGraph.h \
           src/CumulativeSkew.h \
           src/DeviationGraph.h \
           src/DNAGraphPackPlugin.h \
           src/EntropyAlgorithm.h \
           src/GCFramePlot.h \
           src/KarlinSignatureDifferenceGraph.h
SOURCES += src/BaseContentGraph.cpp \
           src/CumulativeSkew.cpp \
           src/DeviationGraph.cpp \
           src/DNAGraphPackPlugin.cpp \
           src/EntropyAlgorithm.cpp \
           src/GCFramePlot.cpp \
           src/KarlinSignatureDifferenceGraph.cpp
TRANSLATIONS += transl/russian.ts
