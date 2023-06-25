include (kalign.pri)

# Input
HEADERS += src/KalignAdapter.h \
           src/KalignException.h \
           src/KalignPlugin.h \
           src/KalignTask.h \
           src/KalignUtils.h \
           src/PairwiseAlignmentHirschbergGUIExtensionFactory.h \
           src/PairwiseAlignmentHirschbergTask.h \
           src/PairwiseAlignmentHirschbergTaskFactory.h \
           src/TaskLocalStorage.h \
           src/kalign2/kalign2.h \
           src/kalign2/kalign2_advanced_gaps.h \
           src/kalign2/kalign2_context.h \
           src/kalign2/kalign2_feature.h \
           src/kalign2/kalign2_hirschberg.h \
           src/kalign2/kalign2_hirschberg_dna.h \
           src/kalign2/kalign2_input.h \
           src/kalign2/kalign2_output.h \
           src/kalign2/kalign2_profile_alignment.h

SOURCES += src/KalignAdapter.cpp \
           src/KalignException.cpp \
           src/KalignPlugin.cpp \
           src/KalignTask.cpp \
           src/KalignUtils.cpp \
           src/PairwiseAlignmentHirschbergGUIExtensionFactory.cpp \
           src/PairwiseAlignmentHirschbergTask.cpp \
           src/PairwiseAlignmentHirschbergTaskFactory.cpp \
           src/TaskLocalStorage.cpp \
           src/kalign2/kalign2_advanced_gaps.c \
           src/kalign2/kalign2_alignment_types.c \
           src/kalign2/kalign2_context.c \
           src/kalign2/kalign2_distance_calculation.c \
           src/kalign2/kalign2_dp.c \
           src/kalign2/kalign2_feature.c \
           src/kalign2/kalign2_hirschberg.c \
           src/kalign2/kalign2_hirschberg_dna.c \
           src/kalign2/kalign2_inferface.c \
           src/kalign2/kalign2_input.c \
           src/kalign2/kalign2_main.c \
           src/kalign2/kalign2_mem.c \
           src/kalign2/kalign2_misc.c \
           src/kalign2/kalign2_output.c \
           src/kalign2/kalign2_profile.c \
           src/kalign2/kalign2_profile_alignment.c \
           src/kalign2/kalign2_string_matching.c \
           src/kalign2/kalign2_tree.c

FORMS += src/PairwiseAlignmentHirschbergOptionsPanelMainWidget.ui

RESOURCES += kalign.qrc
TRANSLATIONS += transl/russian.ts
