include (sitecon.pri)

# Input
HEADERS += src/DIPropertiesSitecon.h \
           src/DIPropertiesTests.h \
           src/SiteconAlgorithm.h \
           src/SiteconAlgorithmTests.h \
           src/SiteconBuildDialogController.h \
           src/SiteconIO.h \
           src/SiteconIOWorkers.h \
           src/SiteconMath.h \
           src/SiteconPlugin.h \
           src/SiteconSearchDialogController.h \
           src/SiteconSearchTask.h \
           src/SiteconWorkers.h \
           src/SiteconQuery.h
FORMS += src/SiteconBuildDialog.ui src/SiteconSearchDialog.ui
SOURCES += src/DIPropertiesSitecon.cpp \
           src/DIPropertiesTests.cpp \
           src/SiteconAlgorithm.cpp \
           src/SiteconAlgorithmTests.cpp \
           src/SiteconBuildDialogController.cpp \
           src/SiteconBuildWorker.cpp \
           src/SiteconIO.cpp \
           src/SiteconIOWorkers.cpp \
           src/SiteconMath.cpp \
           src/SiteconPlugin.cpp \
           src/SiteconSearchDialogController.cpp \
           src/SiteconSearchTask.cpp \
           src/SiteconSearchWorker.cpp \
           src/SiteconQuery.cpp
RESOURCES += sitecon.qrc
TRANSLATIONS += transl/russian.ts
