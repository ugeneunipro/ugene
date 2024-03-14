include (dbi_bam.pri)

# Input
HEADERS += src/Alignment.h \
           src/BaiReader.h \
           src/BaiWriter.h \
           src/BAMDbiPlugin.h \
           src/BAMFormat.h \
           src/BgzfReader.h \
           src/BgzfWriter.h \
           src/CancelledException.h \
           src/CigarValidator.h \
           src/ConvertToSQLiteDialog.h \
           src/ConvertToSQLiteTask.h \
           src/Dbi.h \
           src/Exception.h \
           src/Header.h \
           src/Index.h \
           src/InvalidFormatException.h \
           src/IOException.h \
           src/LoadBamInfoTask.h \
           src/PrepareToImportTask.h \
           src/Reader.h \
           src/SamReader.h \
           src/SamtoolsBasedDbi.h \
           src/VirtualOffset.h
FORMS += src/ConvertToSQLiteDialog.ui
SOURCES += src/Alignment.cpp \
           src/BaiReader.cpp \
           src/BaiWriter.cpp \
           src/BAMDbiPlugin.cpp \
           src/BAMFormat.cpp \
           src/BgzfReader.cpp \
           src/BgzfWriter.cpp \
           src/CancelledException.cpp \
           src/CigarValidator.cpp \
           src/ConvertToSQLiteDialog.cpp \
           src/ConvertToSQLiteTask.cpp \
           src/Dbi.cpp \
           src/Exception.cpp \
           src/Header.cpp \
           src/Index.cpp \
           src/InvalidFormatException.cpp \
           src/IOException.cpp \
           src/LoadBamInfoTask.cpp \
           src/PrepareToImportTask.cpp \
           src/Reader.cpp \
           src/SamReader.cpp \
           src/SamtoolsBasedDbi.cpp \
           src/VirtualOffset.cpp
