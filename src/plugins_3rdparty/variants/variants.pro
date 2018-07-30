include(variants.pri)

# Input
HEADERS += src/SamtoolMpileupWorker.h \
		   src/AssemblySamtoolsMpileup.h \
		   src/SamtoolsPlugin.h 
           
SOURCES += src/SamtoolMpileupWorker.cpp \
		   src/AssemblySamtoolsMpileup.cpp \
		   src/SamtoolsPlugin.cpp 

TRANSLATIONS += transl/russian.ts
  