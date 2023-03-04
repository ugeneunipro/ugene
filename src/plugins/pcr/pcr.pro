include (pcr.pri)

HEADERS += src/EditPrimerDialog.h \
           src/ExtractProductTask.h \
           src/FindPrimerPairsWorker.h \
           src/InSilicoPcrOPWidgetFactory.h \
           src/InSilicoPcrOptionPanelWidget.h \
           src/InSilicoPcrProductsTable.h \
           src/InSilicoPcrTask.h \
           src/InSilicoPcrWorker.h \
           src/InSilicoPcrWorkflowTask.h \
           src/PcrOptionsPanelSavableTab.h \
           src/PcrPlugin.h \
           src/PrimerGroupBox.h \
           src/PrimerLibrary.h \
           src/PrimerLibraryMdiWindow.h \
           src/PrimerLibrarySelector.h \
           src/PrimerLibraryTable.h \
           src/PrimerLibraryTableController.h \
           src/PrimerLibraryWidget.h \
           src/PrimersDetailsDialog.h \
           src/PrimersGrouperWorker.h \
           src/TmCalculatorDelegate.h \
           src/TmCalculatorPropertyWidget.h \
           src/export/ExportPrimersDialog.h \
           src/export/ExportPrimersToDatabaseTask.h \
           src/export/ExportPrimersToLocalFileTask.h \
           src/import/ImportPrimerFromObjectTask.h \
           src/import/ImportPrimersDialog.h \
           src/import/ImportPrimersFromFileTask.h \
           src/import/ImportPrimersFromFolderTask.h \
           src/import/ImportPrimersMultiTask.h \

FORMS += src/EditPrimerDialog.ui \
         src/export/ExportPrimersDialog.ui \
         src/import/ImportPrimersDialog.ui \
         src/InSilicoPcrOptionPanelWidget.ui \
         src/PrimerGroupBox.ui \
         src/PrimerLibrarySelector.ui \
         src/PrimerLibraryWidget.ui \
         src/PrimersDetailsDialog.ui

SOURCES += src/EditPrimerDialog.cpp \
           src/ExtractProductTask.cpp \
           src/FindPrimerPairsWorker.cpp \
           src/InSilicoPcrOPWidgetFactory.cpp \
           src/InSilicoPcrOptionPanelWidget.cpp \
           src/InSilicoPcrProductsTable.cpp \
           src/InSilicoPcrTask.cpp \
           src/InSilicoPcrWorker.cpp \
           src/InSilicoPcrWorkflowTask.cpp \
           src/PcrOptionsPanelSavableTab.cpp \
           src/PcrPlugin.cpp \
           src/PrimerGroupBox.cpp \
           src/PrimerLibrary.cpp \
           src/PrimerLibraryMdiWindow.cpp \
           src/PrimerLibrarySelector.cpp \
           src/PrimerLibraryTable.cpp \
           src/PrimerLibraryTableController.cpp \
           src/PrimerLibraryWidget.cpp \
           src/PrimersDetailsDialog.cpp \
           src/PrimersGrouperWorker.cpp \
           src/TmCalculatorDelegate.cpp \
           src/TmCalculatorPropertyWidget.cpp \
           src/export/ExportPrimersDialog.cpp \
           src/export/ExportPrimersToDatabaseTask.cpp \
           src/export/ExportPrimersToLocalFileTask.cpp \
           src/import/ImportPrimerFromObjectTask.cpp \
           src/import/ImportPrimersDialog.cpp \
           src/import/ImportPrimersFromFileTask.cpp \
           src/import/ImportPrimersFromFolderTask.cpp \
           src/import/ImportPrimersMultiTask.cpp \

TRANSLATIONS += transl/russian.ts

RESOURCES += pcr.qrc
