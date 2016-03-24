include (dotplot.pri)

# Input
HEADERS += src/DotPlotClasses.h \
           src/DotPlotDialog.h \
           src/DotPlotFilesDialog.h \
           src/DotPlotFilterDialog.h \
           src/DotPlotImageExportTask.h \
           src/DotPlotPlugin.h \
           src/DotPlotSplitter.h \
           src/DotPlotTasks.h \
           src/DotPlotWidget.h

FORMS +=   src/DotPlotDialog.ui \
           src/DotPlotFilterDialog.ui \
           src/DotPlotFilesDialog.ui

SOURCES += src/DotPlotClasses.cpp \
           src/DotPlotDialog.cpp \
           src/DotPlotFilesDialog.cpp \
           src/DotPlotFilterDialog.cpp \
           src/DotPlotImageExportTask.cpp \
           src/DotPlotPlugin.cpp \
           src/DotPlotSplitter.cpp \
           src/DotPlotTasks.cpp \
           src/DotPlotWidget.cpp

RESOURCES += dotplot.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
