include (ngs_reads_classification.pri)

# Input
HEADERS += src/ClassificationFilterWorker.h \
           src/ClassificationReportWorker.h \
           src/DatabaseDelegate.h \
           src/EnsembleClassificationWorker.h \
           src/GenomicLibraryDelegate.h \
           src/GenomicLibraryDialog.h \
           src/GenomicLibraryPropertyWidget.h \
           src/NgsReadsClassificationPlugin.h \
           src/NgsReadsClassificationUtils.h \
           src/TaxonomySupport.h

SOURCES += src/ClassificationFilterWorker.cpp \
           src/ClassificationReportWorker.cpp \
           src/DatabaseDelegate.cpp \
           src/EnsembleClassificationWorker.cpp \
           src/GenomicLibraryDelegate.cpp \
           src/GenomicLibraryDialog.cpp \
           src/GenomicLibraryPropertyWidget.cpp \
           src/NgsReadsClassificationPlugin.cpp \
           src/NgsReadsClassificationUtils.cpp \
           src/TaxonomySupport.cpp

FORMS += src/GenomicLibraryDialog.ui

TRANSLATIONS += transl/russian.ts
