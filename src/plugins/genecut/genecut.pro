include (genecut.pri)

# Input
HEADERS += src/io/GenecutHttpFileAdapter.h \
           src/io/GenecutHttpFileAdapterFactory.h \
           src/GenecutOPWidget.h \
           src/GenecutOPWidgetFactory.h \
           src/GenecutPlugin.h

SOURCES += src/io/GenecutHttpFileAdapter.cpp \
           src/io/GenecutHttpFileAdapterFactory.cpp \
           src/GenecutOPWidget.cpp \
           src/GenecutOPWidgetFactory.cpp \
           src/GenecutPlugin.cpp

FORMS   += src/GenecutOPWidget.ui

RESOURCES += genecut.qrc

TRANSLATIONS += transl/russian.ts
