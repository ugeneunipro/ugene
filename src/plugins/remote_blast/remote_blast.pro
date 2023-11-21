include (remote_blast.pri)

# Input
HEADERS += src/RemoteBLASTPlugin.h \
			src/RemoteBLASTPluginTests.h \
			src/RemoteBLASTPrimerPairsToAnnotationsTask.h \
			src/RemoteBLASTPrimerPairToAnnotationsTask.h \
			src/RemoteBLASTTask.h \
			src/HttpRequest.h \
			src/SendSelectionDialog.h \
			src/RemoteBLASTConsts.h \
			src/RemoteBLASTWorker.h \
			src/DBRequestFactory.h \
			src/BlastQuery.h

FORMS += src/RemoteBLASTDialog.ui

SOURCES += src/RemoteBLASTPlugin.cpp \
			src/RemoteBLASTPluginTests.cpp \
			src/RemoteBLASTPrimerPairsToAnnotationsTask.cpp \
			src/RemoteBLASTPrimerPairToAnnotationsTask.cpp \
			src/RemoteBLASTTask.cpp \
			src/HttpRequestBLAST.cpp \
			src/SendSelectionDialog.cpp \
			src/RemoteBLASTConsts.cpp \
			src/RemoteBLASTWorker.cpp \
			src/BlastQuery.cpp

RESOURCES += remote_blast.qrc
TRANSLATIONS += transl/russian.ts
