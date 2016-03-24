include (weight_matrix.pri)

# Input
HEADERS += src/WeightMatrixPlugin.h \
           src/PWMBuildDialogController.h \
           src/PWMJASPARDialogController.h \
           src/PWMSearchDialogController.h \
           src/ViewMatrixDialogController.h \
           src/SetParametersDialogController.h \
		   src/PMatrixFormat.h \
           src/WeightMatrixAlgorithm.h \
           src/WeightMatrixSearchTask.h \
           src/WeightMatrixIO.h \
           src/WeightMatrixIOWorkers.h \
           src/WeightMatrixWorkers.h \
           src/WMQuery.h

FORMS += src/PWMBuildDialog.ui \
         src/PWMSearchDialog.ui \
         src/SearchJASPARDatabase.ui \
         src/ViewMatrixDialog.ui \
		 src/MatrixAndLogoWidget.ui \
         src/SetDefaultParametersDialog.ui

SOURCES += src/WeightMatrixPlugin.cpp \
           src/PWMBuildDialogController.cpp \
           src/PWMJASPARDialogController.cpp \
           src/PWMSearchDialogController.cpp \
           src/ViewMatrixDialogController.cpp \
           src/SetParametersDialogController.cpp \
		   src/PMatrixFormat.cpp \
           src/WeightMatrixAlgorithm.cpp \
           src/WeightMatrixSearchTask.cpp \
           src/WeightMatrixIO.cpp \
           src/WeightMatrixIOWorkers.cpp \
           src/WeightMatrixBuildWorker.cpp \
           src/WeightMatrixSearchWorker.cpp \
           src/WMQuery.cpp

RESOURCES += weight_matrix.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
