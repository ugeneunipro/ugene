include (annotator.pri)

# Input
HEADERS += src/AnnotatorPlugin.h \
           src/AnnotatorTests.h \
           src/CollocationsDialogController.h \
           src/CollocationsSearchAlgorithm.h \
           src/CollocationWorker.h \
           src/CustomAutoAnnotationDialog.h \
           src/CustomPatternAnnotationTask.h \
           src/GeneByGeneReportTask.h \
           src/GeneByGeneReportWorker.h
FORMS += src/CustomAutoAnnotationDialog.ui \
         src/FindAnnotationCollocationsDialog.ui
SOURCES += src/AnnotatorPlugin.cpp \
           src/AnnotatorTests.cpp \
           src/CollocationsDialogController.cpp \
           src/CollocationsSearchAlgorithm.cpp \
           src/CollocationWorker.cpp \
           src/CustomAutoAnnotationDialog.cpp \
           src/CustomPatternAnnotationTask.cpp \
           src/GeneByGeneReportTask.cpp \
           src/GeneByGeneReportWorker.cpp
RESOURCES += annotator.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
