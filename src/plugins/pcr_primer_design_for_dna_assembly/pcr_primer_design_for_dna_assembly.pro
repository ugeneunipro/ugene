include (pcr_primer_design_for_dna_assembly.pri)

HEADERS += src/options_panel/GeneratedSequencesTable.h \
           src/options_panel/ResultTable.h \
           src/options_panel/UserPimerLineEdit.h \
           src/tasks/ExtractPrimerTask.h \
           src/tasks/FindPresenceOfUnwantedParametersTask.h \
           src/tasks/FindUnwantedIslandsTask.h \
           src/tasks/PCRPrimerDesignForDNAAssemblyTask.h \
           src/tasks/UnwantedStructuresInBackboneDialog.h \
           src/utils/UnwantedConnectionsUtils.h \
           src/PCRPrimerDesignForDNAAssemblyOPSavableTab.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.h \
           src/PCRPrimerDesignForDNAAssemblyPlugin.h \
           src/PCRPrimerDesignForDNAAssemblyTaskSettings.h \

SOURCES += src/options_panel/GeneratedSequencesTable.cpp \
           src/options_panel/ResultTable.cpp \
           src/options_panel/UserPimerLineEdit.cpp \
           src/tasks/ExtractPrimerTask.cpp \
           src/tasks/FindPresenceOfUnwantedParametersTask.cpp \
           src/tasks/FindUnwantedIslandsTask.cpp \
           src/tasks/PCRPrimerDesignForDNAAssemblyTask.cpp \
           src/tasks/UnwantedStructuresInBackboneDialog.cpp \
           src/utils/UnwantedConnectionsUtils.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPSavableTab.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.cpp \
           src/PCRPrimerDesignForDNAAssemblyPlugin.cpp \

FORMS +=   src/tasks/UnwantedStructuresInBackboneDialog.ui \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.ui \

TRANSLATIONS += transl/russian.ts

RESOURCES += pcr_primer_design_for_dna_assembly.qrc
