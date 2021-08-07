include (dna_export.pri)

# Input
HEADERS += src/CSVColumnConfiguration.h \
           src/CSVColumnConfigurationDialog.h \
           src/DNAExportPlugin.h \
           src/DNAExportPluginTests.h \
           src/DNASequenceGenerator.h \
           src/DNASequenceGeneratorDialog.h \
           src/ExportAlignmentViewItems.h \
           src/ExportBlastResultDialog.h \
           src/ExportChromatogramDialog.h \
           src/ExportMSA2MSADialog.h \
           src/ExportMSA2SequencesDialog.h \
           src/ExportProjectViewItems.h \
           src/ExportQualityScoresTask.h \
           src/ExportQualityScoresWorker.h \
           src/ExportSelectedSeqRegionsTask.h \
           src/ExportSequences2MSADialog.h \
           src/ExportSequencesDialog.h \
           src/ExportSequenceTask.h \
           src/ExportSequenceViewItems.h \
           src/ExportUtils.h \
           src/GenerateDNAWorker.h \
           src/GetSequenceByIdDialog.h \
           src/ImportAnnotationsFromCSVDialog.h \
           src/ImportAnnotationsFromCSVTask.h \
           src/ImportQualityScoresTask.h \
           src/ImportQualityScoresWorker.h \
           src/McaEditorContext.h \
           src/dialogs/ExportMca2MsaDialog.h \
           src/tasks/ConvertMca2MsaTask.h \
           src/tasks/ExportMca2MsaTask.h

FORMS += src/CSVColumnConfigurationDialog.ui \
         src/DNASequenceGeneratorDialog.ui \
         src/ExportBlastResultDialog.ui \
         src/ExportChromatogramDialog.ui \
         src/ExportMSA2MSADialog.ui \
         src/ExportMSA2SequencesDialog.ui \
         src/ExportSequences2MSADialog.ui \
         src/ExportSequencesDialog.ui \
         src/GetSequenceByIdDialog.ui \
         src/ImportAnnotationsFromCSVDialog.ui \
         src/dialogs/ExportMca2MsaDialog.ui

SOURCES += src/CSVColumnConfigurationDialog.cpp \
           src/DNAExportPlugin.cpp \
           src/DNAExportPluginTests.cpp \
           src/DNASequenceGenerator.cpp \
           src/DNASequenceGeneratorDialog.cpp \
           src/ExportAlignmentViewItems.cpp \
           src/ExportBlastResultDialog.cpp \
           src/ExportChromatogramDialog.cpp \
           src/ExportMSA2MSADialog.cpp \
           src/ExportMSA2SequencesDialog.cpp \
           src/ExportProjectViewItems.cpp \
           src/ExportQualityScoresTask.cpp \
           src/ExportQualityScoresWorker.cpp \
           src/ExportSelectedSeqRegionsTask.cpp \
           src/ExportSequences2MSADialog.cpp \
           src/ExportSequencesDialog.cpp \
           src/ExportSequenceTask.cpp \
           src/ExportSequenceViewItems.cpp \
           src/ExportUtils.cpp \
           src/GenerateDNAWorker.cpp \
           src/GetSequenceByIdDialog.cpp \
           src/ImportAnnotationsFromCSVDialog.cpp \
           src/ImportAnnotationsFromCSVTask.cpp \
           src/ImportQualityScoresTask.cpp \
           src/ImportQualityScoresWorker.cpp \
           src/McaEditorContext.cpp \
           src/dialogs/ExportMca2MsaDialog.cpp \
           src/tasks/ConvertMca2MsaTask.cpp \
           src/tasks/ExportMca2MsaTask.cpp

TRANSLATIONS += transl/russian.ts
