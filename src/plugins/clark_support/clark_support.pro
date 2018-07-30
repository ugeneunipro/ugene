include (clark_support.pri)

# Input
HEADERS += src/ClarkBuildWorker.h \
           src/ClarkClassifyWorker.h \
           src/ClarkSupport.h \
           src/ClarkSupportPlugin.h

SOURCES += src/ClarkBuildWorker.cpp \
           src/ClarkClassifyWorker.cpp \
           src/ClarkSupport.cpp \
           src/ClarkSupportPlugin.cpp

TRANSLATIONS += transl/russian.ts
