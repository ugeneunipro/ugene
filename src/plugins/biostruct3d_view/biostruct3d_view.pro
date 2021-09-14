include (biostruct3d_view.pri)

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

FORMS += src/BioStruct3DSubsetEditor.ui \
         src/SelectModelsDialog.ui \
         src/SettingsDialog.ui \
         src/StructuralAlignmentDialog.ui

RESOURCES += biostruct3d_view.qrc

TRANSLATIONS += transl/russian.ts
