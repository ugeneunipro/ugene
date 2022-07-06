include (zlib.pri)

# Input
HEADERS += src/crc32.h \
           src/deflate.h \
           src/gzguts.h \
           src/inffast.h \
           src/inffast.h \
           src/inffixed.h \
           src/inflate.h \
           src/inftrees.h \
           src/trees.h \
           src/zconf.h \
           src/zlib.h \
           src/zutil.h
SOURCES += src/adler32.c \
           src/compress.c \
           src/crc32.c \
           src/deflate.c \
           src/gzclose.c \
           src/gzlib.c \
           src/gzread.c \
           src/gzwrite.c \
           src/infback.c \
           src/inffast.c \
           src/inflate.c \
           src/inftrees.c \
           src/trees.c \
           src/uncompr.c \
           src/zutil.c
