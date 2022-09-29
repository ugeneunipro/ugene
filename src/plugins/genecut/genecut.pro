include (genecut.pri)

# Input
HEADERS += src/GenecutOPWidget.h \
           src/GenecutOPWidgetFactory.h \
           src/GenecutPlugin.h

SOURCES += src/GenecutOPWidget.cpp \
           src/GenecutOPWidgetFactory.cpp \
           src/GenecutPlugin.cpp

FORMS   += src/GenecutOPWidget.ui

RESOURCES += genecut.qrc

TRANSLATIONS += transl/russian.ts
