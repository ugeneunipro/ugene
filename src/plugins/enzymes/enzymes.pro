include (enzymes.pri)

# Input
HEADERS += src/enzymes_dialog/EnzymeGroupTreeItem.h \
           src/enzymes_dialog/EnzymesSelectorWidget.h \
           src/enzymes_dialog/EnzymeTreeItem.h \
           src/enzymes_dialog/FindEnzymesDialog.h \
           src/enzymes_dialog/RegionSelectorWithExclude.h \
           src/enzymes_dialog/ResultsCountFilter.h \
           src/insert/InsertEnzymeDialog.h \
           src/insert/InsertEnzymeWidget.h \
           src/CloningUtilTasks.h \
           src/ConstructMoleculeDialog.h \
           src/CreateFragmentDialog.h \
           src/DigestSequenceDialog.h \
           src/DNAFragment.h \
           src/EditFragmentDialog.h \
           src/EnzymesIO.h \
           src/EnzymesPlugin.h \
           src/EnzymesQuery.h \
           src/EnzymesTests.h \
           src/FindEnzymesAlgorithm.h \
           src/FindEnzymesTask.h
FORMS += src/enzymes_dialog/EnzymesSelectorWidget.ui \
         src/enzymes_dialog/ResultsCountFilter.ui \
         src/insert/InsertEnzymeWidget.ui \
         src/ConstructMoleculeDialog.ui \
         src/CreateFragmentDialog.ui \
         src/DigestSequenceDialog.ui \
         src/EditFragmentDialog.ui
SOURCES += src/enzymes_dialog/EnzymeGroupTreeItem.cpp \
           src/enzymes_dialog/EnzymesSelectorWidget.cpp \
           src/enzymes_dialog/EnzymeTreeItem.cpp \
           src/enzymes_dialog/FindEnzymesDialog.cpp \
           src/enzymes_dialog/RegionSelectorWithExclude.cpp \
           src/enzymes_dialog/ResultsCountFilter.cpp \
           src/insert/InsertEnzymeDialog.cpp \
           src/insert/InsertEnzymeWidget.cpp \
           src/CloningUtilTasks.cpp \
           src/ConstructMoleculeDialog.cpp \
           src/CreateFragmentDialog.cpp \
           src/DigestSequenceDialog.cpp \
           src/DNAFragment.cpp \
           src/EditFragmentDialog.cpp \
           src/EnzymesIO.cpp \
           src/EnzymesPlugin.cpp \
           src/EnzymesQuery.cpp \
           src/EnzymesTests.cpp \
           src/FindEnzymesTask.cpp
RESOURCES += enzymes.qrc
TRANSLATIONS += transl/russian.ts
