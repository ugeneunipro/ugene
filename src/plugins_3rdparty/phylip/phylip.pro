include (phylip.pri)

# Input
HEADERS += src/SeqBootAdapter.h \
           src/cons.h \
           src/seqboot.h \
           src/DistanceMatrix.h \
           src/NeighborJoinAdapter.h \
           src/NeighborJoinWidget.h \
           src/PhylipPlugin.h \
           src/PhylipPluginTests.h \
           src/dist.h \
           src/dnadist.h \
           src/neighbor.h \
           src/phylip.h \
           src/protdist.h \
           src/seq.h
FORMS += src/NeighborJoinWidget.ui
SOURCES += src/SeqBootAdapter.cpp \
           src/cons.cpp \
           src/seqboot.cpp \
           src/DistanceMatrix.cpp \
           src/NeighborJoinAdapter.cpp \
           src/NeighborJoinWidget.cpp \
           src/PhylipPlugin.cpp \
           src/PhylipPluginTests.cpp \
           src/dist.cpp \
           src/dnadist.cpp \
           src/neighbor.cpp \
           src/phylip.cpp \
           src/protdist.cpp \
           src/seq.cpp
TRANSLATIONS += transl/english.ts transl/russian.ts
