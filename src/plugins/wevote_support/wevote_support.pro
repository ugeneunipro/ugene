include (wevote_support.pri)

# Input
HEADERS += src/PrepareWevoteTaxonomyDataTask.h \
           src/WevotePrompter.h \
           src/WevoteSupport.h \
           src/WevoteSupportPlugin.h \
           src/WevoteTask.h \
           src/WevoteValidator.h \
           src/WevoteWorker.h \
           src/WevoteWorkerFactory.h

SOURCES += src/PrepareWevoteTaxonomyDataTask.cpp \
           src/WevotePrompter.cpp \
           src/WevoteSupport.cpp \
           src/WevoteSupportPlugin.cpp \
           src/WevoteTask.cpp \
           src/WevoteValidator.cpp \
           src/WevoteWorker.cpp \
           src/WevoteWorkerFactory.cpp

TRANSLATIONS += transl/russian.ts
