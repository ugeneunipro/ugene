include (kalign.pri)

# Input
HEADERS += src/KalignAdapter.h \
           src/KalignDialogController.h \
           src/KalignPlugin.h \
           src/KalignConstants.h \
           src/KalignException.h \
           src/KalignTask.h \
           src/KalignUtils.h \
           src/KalignWorker.h \
           src/TaskLocalStorage.h \
           src/kalign_tests/KalignTests.h \
           src/kalign2/kalign2_context.h \
           src/kalign2/kalign2.h \
           src/kalign2/kalign2_advanced_gaps.h \
           src/kalign2/kalign2_feature.h \
           src/kalign2/kalign2_hirschberg.h \
           src/kalign2/kalign2_hirschberg_dna.h \
           src/kalign2/kalign2_input.h \
           src/kalign2/kalign2_output.h \
           src/kalign2/kalign2_profile_alignment.h \ 
    src/PairwiseAlignmentHirschbergTaskFactory.h \
    src/PairwiseAlignmentHirschbergGUIExtensionFactory.h \
    src/PairwiseAlignmentHirschbergTask.h

FORMS += src/KalignDialog.ui \
    src/PairwiseAlignmentHirschbergOptionsPanelMainWidget.ui

SOURCES += src/KalignAdapter.cpp \
           src/KalignDialogController.cpp \
           src/KalignException.cpp \
           src/KalignPlugin.cpp \
           src/KalignTask.cpp \
           src/KalignUtils.cpp \
           src/KalignWorker.cpp \
           src/TaskLocalStorage.cpp \
           src/kalign_tests/KalignTests.cpp \
           src/kalign2/kalign2_context.c \
           src/kalign2/kalign2_distance_calculation.c \
           src/kalign2/kalign2_dp.c \
           src/kalign2/kalign2_input.c \
           src/kalign2/kalign2_main.c \
           src/kalign2/kalign2_mem.c \
           src/kalign2/kalign2_inferface.c \
           src/kalign2/kalign2_misc.c \
           src/kalign2/kalign2_tree.c \
           src/kalign2/kalign2_profile.c \
           src/kalign2/kalign2_alignment_types.c \
           src/kalign2/kalign2_feature.c \
           src/kalign2/kalign2_hirschberg.c \
           src/kalign2/kalign2_advanced_gaps.c \
           src/kalign2/kalign2_hirschberg_dna.c \
           src/kalign2/kalign2_output.c \
           src/kalign2/kalign2_string_matching.c \
           src/kalign2/kalign2_profile_alignment.c \
    src/PairwiseAlignmentHirschbergTaskFactory.cpp \
    src/PairwiseAlignmentHirschbergGUIExtensionFactory.cpp \
    src/PairwiseAlignmentHirschbergTask.cpp

RESOURCES += kalign.qrc
TRANSLATIONS += transl/russian.ts
