include (pcr_primer_design_for_dna_assembly.pri)

HEADERS += src/options_panel/GeneratedSequencesTable.h \
           src/options_panel/ResultTable.h \
           src/options_panel/UserPimerLineEdit.h \
           src/tasks/ExtractPrimerTask.h \
           src/tasks/FindPresenceOfUnwantedParametersTask.h \
           src/tasks/FindUnwantedIslandsTask.h \
           src/tasks/PCRPrimerDesignForDNAAssemblyTask.h \
           src/tasks/PCRPrimerDesignForDNAAssemblyTaskTest.h \
           src/tasks/UnwantedStructuresInBackboneDialog.h \
           src/utils/PCRPrimerDesignTaskReportUtils.h \
           src/utils/UnwantedConnectionsUtils.h \
           src/PCRPrimerDesignForDNAAssemblyOPSavableTab.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.h \
           src/PCRPrimerDesignForDNAAssemblyPlugin.h \
           src/PCRPrimerDesignForDNAAssemblyTaskSettings.h \
           src/UnwantedStructuresInBackboneDialog.h \

SOURCES += src/options_panel/GeneratedSequencesTable.cpp \
           src/options_panel/ResultTable.cpp \
           src/options_panel/UserPimerLineEdit.cpp \
           src/tasks/ExtractPrimerTask.cpp \
           src/tasks/FindPresenceOfUnwantedParametersTask.cpp \
           src/tasks/FindUnwantedIslandsTask.cpp \
           src/tasks/PCRPrimerDesignForDNAAssemblyTask.cpp \
           src/tasks/PCRPrimerDesignForDNAAssemblyTaskTest.cpp \
           src/utils/PCRPrimerDesignTaskReportUtils.cpp \
           src/utils/UnwantedConnectionsUtils.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPSavableTab.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.cpp \
           src/PCRPrimerDesignForDNAAssemblyPlugin.cpp \
           src/UnwantedStructuresInBackboneDialog.cpp \

FORMS +=   src/PCRPrimerDesignForDNAAssemblyOPWidget.ui \
           src/UnwantedStructuresInBackboneDialog.ui \


TRANSLATIONS += transl/russian.ts

RESOURCES += pcr_primer_design_for_dna_assembly.qrc
