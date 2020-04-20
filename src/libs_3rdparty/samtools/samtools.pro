include(samtools.pri)

# Input
HEADERS += src/SamtoolsAdapter.h \
           src/ugene_custom_io.h \
           src/samtools/bam.h \
           src/samtools/bam2bcf.h \
           src/samtools/bam_endian.h \
           src/samtools/bgzf.h \
           src/samtools/errmod.h \
           src/samtools/faidx.h \
           src/samtools/kaln.h \
           src/samtools/khash.h \
           src/samtools/klist.h \
           src/samtools/knetfile.h \
           src/samtools/kprobaln.h \
           src/samtools/kseq.h \
           src/samtools/ksort.h \
           src/samtools/kstring.h \
           src/samtools/razf.h \
           src/samtools/sam.h \
           src/samtools/sam_header.h \
           src/samtools/sample.h \
           src/samtools/bcftools/bcf.h \
           src/samtools/bcftools/kmin.h \
           src/samtools/bcftools/prob1.h
win32 : HEADERS += src/samtools/win32/getopt.h \
                   src/samtools/win32/stdint.h \
                   src/samtools/win32/unistd.h \
                   src/samtools/win32/msvc_compat.h
SOURCES += src/SamtoolsAdapter.cpp \
           src/ugene_custom_io.c \
           src/samtools/bam.c \
           src/samtools/bam2bcf.c \
           src/samtools/bam2bcf_indel.c \
           src/samtools/bam2depth.c \
           src/samtools/bam_aux.c \
           src/samtools/bam_cat.c \
           src/samtools/bam_color.c \
           src/samtools/bam_import.c \
           src/samtools/bam_index.c \
           src/samtools/bam_lpileup.c \
           src/samtools/bam_mate.c \
           src/samtools/bam_md.c \
           src/samtools/bam_pileup.c \
           src/samtools/bam_plcmd.c \
           src/samtools/bam_reheader.c \
           src/samtools/bam_rmdup.c \
           src/samtools/bam_rmdupse.c \
           src/samtools/bam_sort.c \
           src/samtools/bam_stat.c \
           src/samtools/bam_tview.c \
           src/samtools/bedidx.c \
           src/samtools/bgzf.c \
           src/samtools/cut_target.c \
           src/samtools/errmod.c \
           src/samtools/faidx.c \
           src/samtools/kaln.c \
           src/samtools/knetfile.c \
           src/samtools/kprobaln.c \
           src/samtools/kstring.c \
           src/samtools/phase.c \
           src/samtools/razf.c \
           src/samtools/razip.c \
           src/samtools/sam.c \
           src/samtools/sam_header.c \
           src/samtools/sam_view.c \
           src/samtools/sample.c \
           src/samtools/bcftools/bcf.c \
           src/samtools/bcftools/bcf2qcall.c \
           src/samtools/bcftools/bcfutils.c \
		   src/samtools/bcftools/call1.c \
           src/samtools/bcftools/em.c \
           src/samtools/bcftools/fet.c \
           src/samtools/bcftools/index.c \
           src/samtools/bcftools/kfunc.c \
           src/samtools/bcftools/kmin.c \
           src/samtools/bcftools/mut.c \
           src/samtools/bcftools/prob1.c \
           src/samtools/bcftools/vcf.c
win32 : SOURCES += src/samtools/win32/getopt.c