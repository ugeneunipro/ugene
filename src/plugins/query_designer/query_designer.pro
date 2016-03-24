include (query_designer.pri)

# Input
HEADERS += src/QueryDesignerPlugin.h \
           src/QueryViewController.h \
           src/QueryViewItems.h \
           src/QueryPalette.h \
           src/QueryEditor.h \
           src/QueryProcCfgModel.h \
           src/QueryProcCfgDelegate.h \
           src/QDSceneIOTasks.h \
           src/QDDocument.h \
           src/QueryViewAdapter.h \
           src/QDSamples.h \
           src/QDRunDialog.h \
           src/QDDocumentFormat.h \
           src/QDTests.h \
           src/QDGroupsEditor.h \
           src/QDWorker.h \
           src/library/QDFindActor.h \
           src/library/QDFindPolyRegionsActor.h \
           src/library/QDGcContentActor.h

FORMS += src/AddConstraintDialog.ui \
         src/QueryEditorWidget.ui \
         src/RunQueryDialog.ui \
         src/QDDialog.ui

SOURCES += src/QueryDesignerPlugin.cpp \
           src/QueryViewController.cpp \
           src/QueryViewItems.cpp \
           src/QueryPalette.cpp \
           src/QueryEditor.cpp \
           src/QDSceneIOTasks.cpp \
           src/QDDocument.cpp \
           src/QueryViewAdapter.cpp \
           src/QDSamples.cpp \
           src/QDRunDialog.cpp \
           src/QDDocumentFormat.cpp \
           src/QDTests.cpp \
           src/QDGroupsEditor.cpp \
           src/QDWorker.cpp \
           src/library/QDFindActor.cpp \
           src/library/QDFindPolyRegionsActor.cpp \
           src/library/QDGcContentActor.cpp

RESOURCES += query_designer.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts