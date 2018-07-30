include (psipred.pri)

HEADERS += src/PsipredAlgTask.h \
           src/PsipredPlugin.h \
           src/ssdefs.h \
           src/sspred_avpred.h \
           src/sspred_hmulti.h \
           src/sspred_net.h \
           src/sspred_net2.h \
           src/sspred_utils.h
SOURCES += src/PsipredAlgTask.cpp \
           src/PsipredPlugin.cpp \
           src/seq2mtx.cpp \
           src/sspred_avpred.cpp \
           src/sspred_hmulti.cpp \
           src/sspred_utils.cpp
RESOURCES += psipred.qrc
TRANSLATIONS += transl/russian.ts
