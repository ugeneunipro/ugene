include (genome_aligner.pri)

# Input
HEADERS += src/BuildSArraySettingsWidget.h \
           src/GenomeAlignerCMDLineTask.h \
           src/GenomeAlignerFindTask.h \
           src/GenomeAlignerIndex.h \
           src/GenomeAlignerIndexPart.h \
           src/GenomeAlignerIndexTask.h \
           src/GenomeAlignerIO.h \
           src/GenomeAlignerPlugin.h \
           src/GenomeAlignerSearchQuery.h \
           src/GenomeAlignerSettingsController.h \
           src/GenomeAlignerSettingsWidget.h \
           src/GenomeAlignerTask.h \
           src/GenomeAlignerWorker.h \
           src/GenomeAlignerIndexWorker.h \
           src/GenomeAlignerWriteTask.h \
           src/ReadShortReadsSubTask.h \
           src/WriteAlignedReadsSubTask.h \
           src/DataBunch.h

FORMS += src/BuildSuffixArraySettings.ui \
         src/GenomeAlignerSettings.ui \
         src/GenomeAlignerSettingsWidget.ui
SOURCES += src/BuildSArraySettingsWidget.cpp \
           src/GenomeAlignerCMDLineTask.cpp \
           src/GenomeAlignerFindTask.cpp \
           src/GenomeAlignerIndex.cpp \
           src/GenomeAlignerIndexPart.cpp \
           src/GenomeAlignerIndexTask.cpp \
           src/GenomeAlignerIO.cpp \
           src/GenomeAlignerPlugin.cpp \
           src/GenomeAlignerSearchQuery.cpp \
           src/GenomeAlignerSettingsController.cpp \
           src/GenomeAlignerSettingsWidget.cpp \
           src/GenomeAlignerTask.cpp \
           src/GenomeAlignerWorker.cpp \
           src/GenomeAlignerIndexWorker.cpp \
           src/GenomeAlignerWriteTask.cpp \
           src/ReadShortReadsSubTask.cpp \
           src/WriteAlignedReadsSubTask.cpp \
           src/DataBunch.cpp

TRANSLATIONS += transl/english.ts transl/russian.ts
