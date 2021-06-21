include (pcr_primer_design_for_dna_assembly.pri)

HEADERS += src/PCRPrimerDesignForDNAAssemblyOPSavableTab.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.h \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.h \
           src/PCRPrimerDesignForDNAAssemblyPlugin.h \

SOURCES += src/PCRPrimerDesignForDNAAssemblyOPSavableTab.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidget.cpp \
           src/PCRPrimerDesignForDNAAssemblyOPWidgetFactory.cpp \
           src/PCRPrimerDesignForDNAAssemblyPlugin.cpp \

FORMS +=   src/PCRPrimerDesignForDNAAssemblyOPWidget.ui \

TRANSLATIONS += transl/russian.ts

#RESOURCES += pcr_primer_design_for_dna_assembly.qrc
