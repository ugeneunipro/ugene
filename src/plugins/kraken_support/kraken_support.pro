include (kraken_support.pri)

# Input
HEADERS += src/DatabaseSizeRelation.h \
           src/KrakenBuildPrompter.h \
           src/KrakenBuildTask.h \
           src/KrakenBuildValidator.h \
           src/KrakenBuildWorker.h \
           src/KrakenBuildWorkerFactory.h \
           src/KrakenClassifyLogParser.h \
           src/KrakenClassifyPrompter.h \
           src/KrakenClassifyTask.h \
           src/KrakenClassifyValidator.h \
           src/KrakenClassifyWorker.h \
           src/KrakenClassifyWorkerFactory.h \
           src/KrakenSupport.h \
           src/KrakenSupportPlugin.h \
           src/KrakenTranslateLogParser.h

SOURCES += src/DatabaseSizeRelation.cpp \
           src/KrakenBuildPrompter.cpp \
           src/KrakenBuildTask.cpp \
           src/KrakenBuildValidator.cpp \
           src/KrakenBuildWorker.cpp \
           src/KrakenBuildWorkerFactory.cpp \
           src/KrakenClassifyLogParser.cpp \
           src/KrakenClassifyPrompter.cpp \
           src/KrakenClassifyTask.cpp \
           src/KrakenClassifyValidator.cpp \
           src/KrakenClassifyWorker.cpp \
           src/KrakenClassifyWorkerFactory.cpp \
           src/KrakenSupport.cpp \
           src/KrakenSupportPlugin.cpp \
           src/KrakenTranslateLogParser.cpp

TRANSLATIONS += transl/russian.ts

