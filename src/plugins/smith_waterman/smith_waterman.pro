include (smith_waterman.pri)

# Input
HEADERS += src/PairAlignSequences.h \
           src/SmithWatermanAlgorithm.h \
           src/SWAlgorithmPlugin.h \
           src/SWAlgorithmTask.h \
           src/SWTaskFactory.h \
           src/SmithWatermanTests.h \
           src/SWWorker.h \
           src/SWQuery.h \
    src/PairwiseAlignmentSmithWatermanGUIExtension.h

SOURCES += src/PairAlignSequences.cpp \
           src/SmithWatermanAlgorithm.cpp \
           src/SWAlgorithmPlugin.cpp \
           src/SWAlgorithmTask.cpp \
           src/SWTaskFactory.cpp \
           src/SmithWatermanTests.cpp \
           src/SWWorker.cpp \
           src/SWQuery.cpp \
           src/PairwiseAlignmentSmithWatermanGUIExtension.cpp

target_platform_has_sse() {
    HEADERS +=  src/SmithWatermanAlgorithmSSE2.h
    SOURCES +=  src/SmithWatermanAlgorithmSSE2.cpp
}

TRANSLATIONS += transl/russian.ts

FORMS += \
    src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui
