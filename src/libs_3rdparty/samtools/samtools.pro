include(samtools.pri)

# Input
HEADERS += src/SamtoolsAdapter.h \
           src/samtools/bam.h \
           src/samtools/bam2bcf.h \
           src/samtools/bam_endian.h \
           src/samtools/bgzf.h \
           src/samtools/errmod.h \
           src/samtools/faidx.h \
           src/samtools/kaln.h \
           src/samtools/khash.h \
           src/samtools/kseq.h \
           src/samtools/ksort.h \
           src/samtools/kstring.h \
           src/samtools/razf.h \
           src/samtools/sam.h \
           src/samtools/sam_header.h \
           src/samtools/bcftools/bcf.h

win32 : HEADERS += src/samtools/win32/stdint.h \
                   src/samtools/win32/unistd.h \
                   src/samtools/win32/msvc_compat.h

SOURCES += src/SamtoolsAdapter.cpp \
           src/samtools/bam.c \
           src/samtools/bam2bcf.c \
           src/samtools/bam_aux.c \
           src/samtools/bam_import.c \
           src/samtools/bam_index.c \
           src/samtools/bam_lpileup.c \
           src/samtools/bam_pileup.c \
           src/samtools/bam_sort.c \
           src/samtools/bgzf.c \
           src/samtools/errmod.c \
           src/samtools/faidx.c \
           src/samtools/kaln.c \
           src/samtools/kstring.c \
           src/samtools/razf.c \
           src/samtools/sam.c \
           src/samtools/sam_header.c
