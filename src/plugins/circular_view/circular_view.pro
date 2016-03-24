include (circular_view.pri)

# Input
HEADERS += src/CircularItems.h \
           src/CircularView.h \
           src/CircularViewImageExportTask.h \
           src/CircularViewPlugin.h \
           src/CircularViewSettingsWidget.h \
           src/CircularViewSettingsWidgetFactory.h \
           src/CircularViewSplitter.h \
           src/RestrictionMapWidget.h \
           src/SetSequenceOriginDialog.h \
           src/ShiftSequenceStartTask.h
FORMS +=   src/CircularViewSettingsWidget.ui \
           src/SetSequenceOriginDialog.ui
SOURCES += src/CircularItems.cpp \
           src/CircularView.cpp \
           src/CircularViewImageExportTask.cpp \
           src/CircularViewPlugin.cpp \
           src/CircularViewSettingsWidget.cpp \
           src/CircularViewSettingsWidgetFactory.cpp \
           src/CircularViewSplitter.cpp \
           src/RestrictionMapWidget.cpp \
           src/SetSequenceOriginDialog.cpp \
           src/ShiftSequenceStartTask.cpp
RESOURCES += circular_view.qrc
TRANSLATIONS += transl/english.ts transl/russian.ts
