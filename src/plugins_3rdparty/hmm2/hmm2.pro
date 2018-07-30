include (hmm2.pri)

# Input
HEADERS += src/HMMIO.h \
           src/HMMIOWorker.h \
           src/TaskLocalStorage.h \
           src/uHMMPlugin.h \
           src/hmmer2/config.h \
           src/hmmer2/funcs.h \
           src/hmmer2/structs.h \
           src/u_build/HMMBuildDialogController.h \
           src/u_build/HMMBuildWorker.h \
           src/u_build/uhmmbuild.h \
           src/u_calibrate/HMMCalibrateDialogController.h \
           src/u_calibrate/HMMCalibrateTask.h \
           src/u_calibrate/uhmmcalibrate.h \
           src/u_search/hmmer_ppu.h \
           src/u_search/HMMSearchDialogController.h \
           src/u_search/HMMSearchTask.h \
           src/u_search/HMMSearchWorker.h \
           src/u_search/HMMSearchQDActor.h \
           src/u_search/uhmmsearch.h \
           src/u_search/uhmmsearch_cell.h \
           src/u_search/uhmmsearch_opt.h \
           src/u_search/uhmmsearch_sse.h \
           src/u_spu/hmmer_spu.h \
           src/u_tests/uhmmerTests.h
FORMS += src/u_build/HMMBuildDialog.ui \
         src/u_calibrate/HMMCalibrateDialog.ui \
         src/u_search/HMMSearchDialog.ui
SOURCES += src/HMMIO.cpp \
           src/HMMIOWorker.cpp \
           src/TaskLocalStorage.cpp \
           src/uHMMPlugin.cpp \
           src/hmmer2/aligneval.cpp \
           src/hmmer2/alphabet.cpp \
           src/hmmer2/altivec_algorithms.cpp \
           src/hmmer2/checksum.cpp \
           src/hmmer2/cluster.cpp \
           src/hmmer2/core_algorithms.cpp \
           src/hmmer2/fast_algorithms.cpp \
           src/hmmer2/histogram.cpp \
           src/hmmer2/iupac.cpp \
           src/hmmer2/masks.cpp \
           src/hmmer2/mathsupport.cpp \
           src/hmmer2/modelmakers.cpp \
           src/hmmer2/msa.cpp \
           src/hmmer2/plan7.cpp \
           src/hmmer2/prior.cpp \
           src/hmmer2/sqerror.cpp \
           src/hmmer2/sre_math.cpp \
           src/hmmer2/sre_random.cpp \
           src/hmmer2/sre_string.cpp \
           src/hmmer2/tophits.cpp \
           src/hmmer2/trace.cpp \
           src/hmmer2/vectorops.cpp \
           src/hmmer2/weight.cpp \
           src/u_build/HMMBuildDialogController.cpp \
           src/u_build/HMMBuildWorker.cpp \
           src/u_build/uhmmbuild.cpp \
           src/u_calibrate/HMMCalibrateDialogController.cpp \
           src/u_calibrate/HMMCalibrateTask.cpp \
           src/u_calibrate/uhmmcalibrate.cpp \
           src/u_search/hmmer_ppu.cpp \
           src/u_search/HMMSearchDialogController.cpp \
           src/u_search/HMMSearchTask.cpp \
           src/u_search/HMMSearchWorker.cpp \
           src/u_search/HMMSearchQDActor.cpp \
           src/u_search/uhmmsearch.cpp \
           src/u_search/uhmmsearch_cell.cpp \
           src/u_search/uhmmsearch_opt.cpp \
           src/u_search/uhmmsearch_sse.cpp \
           src/u_spu/hmmer_spu.c \
           src/u_spu/hmmercell_spu.c \
           src/u_tests/uhmmerTests.cpp
RESOURCES += hmm2.qrc
TRANSLATIONS += transl/russian.ts
