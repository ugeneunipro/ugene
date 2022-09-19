include (smith_waterman.pri)

# Input
HEADERS += src/PairAlignSequences.h \
           src/SmithWatermanAlgorithm.h \
           src/SmithWatermanAlgorithmSSE2.h \
           src/SWAlgorithmPlugin.h \
           src/SWAlgorithmTask.h \
           src/SWTaskFactory.h \
           src/SmithWatermanTests.h \
           src/SWWorker.h \
           src/SWQuery.h \
    src/PairwiseAlignmentSmithWatermanGUIExtension.h

SOURCES += src/PairAlignSequences.cpp \
           src/SmithWatermanAlgorithm.cpp \
           src/SmithWatermanAlgorithmSSE2.cpp \
           src/SWAlgorithmPlugin.cpp \
           src/SWAlgorithmTask.cpp \
           src/SWTaskFactory.cpp \
           src/SmithWatermanTests.cpp \
           src/SWWorker.cpp \
           src/SWQuery.cpp \
    src/PairwiseAlignmentSmithWatermanGUIExtension.cpp

TRANSLATIONS += transl/russian.ts

FORMS += \
    src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui











