include (orf_marker.pri)

# Input
HEADERS += src/ORFDialog.h \
           src/ORFMarkerPlugin.h \
           src/ORFMarkerTask.h \
           src/ORFMarkerTests.h \
           src/ORFQuery.h \
           src/ORFWorker.h
FORMS += src/ORFDialogUI.ui
SOURCES += src/ORFDialog.cpp \
           src/ORFMarkerPlugin.cpp \
           src/ORFMarkerTask.cpp \
           src/ORFMarkerTests.cpp \
           src/ORFQuery.cpp \
           src/ORFWorker.cpp
RESOURCES += orf_marker.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
