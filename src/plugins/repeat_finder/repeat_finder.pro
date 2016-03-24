include (repeat_finder.pri)

# Input
HEADERS += src/FindRepeatsDialog.h \
           src/FindRepeatsTask.h \
           src/FindTandemsDialog.h \
           src/RepeatFinderPlugin.h \
           src/RepeatFinderTests.h \
           src/RepeatWorker.h \
           src/RF_BitMask.h \
           src/RF_SArray_TandemFinder.h \
           src/RF_SuffixArray.h \
           src/RFBase.h \
           src/RFConstants.h \
           src/RFDiagonal.h \
           src/RFSArray.h \
           src/RFSArrayWK.h \
           src/RFTaskFactory.h \
           src/RepeatQuery.h \
           src/RepeatTest.h \
           src/TandemQuery.h
FORMS += src/FindRepeatsDialog.ui src/FindTandemsDialog.ui
SOURCES += src/FindRepeatsDialog.cpp \
           src/FindRepeatsTask.cpp \
           src/FindTandemsDialog.cpp \
           src/RepeatFinderPlugin.cpp \
           src/RepeatFinderTests.cpp \
           src/RepeatWorker.cpp \
           src/RF_SArray_TandemFinder.cpp \
           src/RF_SuffixArray.cpp \
           src/RFBase.cpp \
           src/RFDiagonal.cpp \
           src/RFSArray.cpp \
           src/RFSArrayWK.cpp \
           src/RFTaskFactory.cpp \
           src/RepeatQuery.cpp \
           src/RepeatTest.cpp \
           src/TandemQuery.cpp
RESOURCES += repeat_finder.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
