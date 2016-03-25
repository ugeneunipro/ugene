include (biostruct3d_view.pri)

use_deprecated_view() {

HEADERS += src/deprecated/AnaglyphRenderer.h \
           src/deprecated/BallAndStickGLRenderer.h \
           src/deprecated/BioStruct3DColorScheme.h \
           src/deprecated/BioStruct3DGLImageExportTask.h \
           src/deprecated/BioStruct3DGLRender.h \
           src/deprecated/BioStruct3DGLWidget.h \
           src/deprecated/BioStruct3DSplitter.h \
           src/deprecated/BioStruct3DSubsetEditor.h \
           src/deprecated/BioStruct3DViewPlugin.h \
           src/deprecated/GLFrameManager.h \
           src/deprecated/GraphicUtils.h \
           src/deprecated/MolecularSurfaceRenderer.h \
           src/deprecated/SelectModelsDialog.h \
           src/deprecated/SettingsDialog.h \
           src/deprecated/StructuralAlignmentDialog.h \
           src/deprecated/TubeGLRenderer.h \
           src/deprecated/VanDerWaalsGLRenderer.h \
           src/deprecated/WormsGLRenderer.h \
           src/gl2ps/gl2ps.h

SOURCES += src/deprecated/BioStruct3DColorScheme.cpp \
           src/deprecated/BioStruct3DGLRender.cpp \
           src/deprecated/BioStruct3DSplitter.cpp \
           src/deprecated/BioStruct3DSubsetEditor.cpp \
           src/deprecated/BioStruct3DViewPlugin.cpp \
           src/deprecated/SelectModelsDialog.cpp \
           src/deprecated/SettingsDialog.cpp \
           src/deprecated/StructuralAlignmentDialog.cpp \
           src/deprecated/AnaglyphRenderer.cpp \
           src/deprecated/BallAndStickGLRenderer.cpp \
           src/deprecated/BioStruct3DGLImageExportTask.cpp \
           src/deprecated/BioStruct3DGLWidget.cpp \
           src/deprecated/GLFrameManager.cpp \
           src/deprecated/GraphicUtils.cpp \
           src/deprecated/MolecularSurfaceRenderer.cpp \
           src/deprecated/TubeGLRenderer.cpp \
           src/deprecated/VanDerWaalsGLRenderer.cpp \
           src/deprecated/WormsGLRenderer.cpp \
           src/gl2ps/gl2ps.cpp

} else {

HEADERS += src/AnaglyphRenderer.h \
           src/BallAndStickGLRenderer.h \
           src/BioStruct3DColorScheme.h \
           src/BioStruct3DGLImageExportTask.h \
           src/BioStruct3DGLRender.h \
           src/BioStruct3DGLWidget.h \
           src/BioStruct3DSplitter.h \
           src/BioStruct3DSubsetEditor.h \
           src/BioStruct3DViewPlugin.h \
           src/GLFrameManager.h \
           src/GraphicUtils.h \
           src/MolecularSurfaceRenderer.h \
           src/SelectModelsDialog.h \
           src/SettingsDialog.h \
           src/StructuralAlignmentDialog.h \
           src/TubeGLRenderer.h \
           src/VanDerWaalsGLRenderer.h \
           src/WormsGLRenderer.h \
           src/gl2ps/gl2ps.h

SOURCES += src/AnaglyphRenderer.cpp \
           src/BallAndStickGLRenderer.cpp \
           src/BioStruct3DColorScheme.cpp \
           src/BioStruct3DGLImageExportTask.cpp \
           src/BioStruct3DGLRender.cpp \
           src/BioStruct3DGLWidget.cpp \
           src/BioStruct3DSplitter.cpp \
           src/BioStruct3DSubsetEditor.cpp \
           src/BioStruct3DViewPlugin.cpp \
           src/GLFrameManager.cpp \
           src/GraphicUtils.cpp \
           src/MolecularSurfaceRenderer.cpp \
           src/SelectModelsDialog.cpp \
           src/SettingsDialog.cpp \
           src/StructuralAlignmentDialog.cpp \
           src/TubeGLRenderer.cpp \
           src/VanDerWaalsGLRenderer.cpp \
           src/WormsGLRenderer.cpp \
           src/gl2ps/gl2ps.cpp
}

FORMS += src/ui/BioStruct3DSubsetEditor.ui \
         src/ui/SelectModelsDialog.ui \
         src/ui/SettingsDialog.ui \
         src/ui/StructuralAlignmentDialog.ui

RESOURCES += biostruct3d_view.qrc

TRANSLATIONS += transl/english.ts \
                transl/russian.ts
