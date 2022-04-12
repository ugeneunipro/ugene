/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _U2_OPENGL_WIDGET_H_
#define _U2_OPENGL_WIDGET_H_

#include <QAction>
#include <QActionGroup>
#include <QColor>
#include <QMenu>
#include <QOpenGLWidget>
#include <QSharedPointer>
#include <QTimer>

#include <U2Algorithm/MolecularSurface.h>

#include <U2Core/BioStruct3DObject.h>
#include <U2Core/Vector3D.h>

#include "AnaglyphRenderer.h"
#include "BioStruct3DColorScheme.h"
#include "BioStruct3DGLRender.h"
#include "MolecularSurfaceRenderer.h"

class QLabel;

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class Annotation;
class AnnotationSelection;
class AnnotationTableObject;
class BioStruct3D;
class BioStruct3DColorSchemeFactory;
class BioStruct3DGLRendererFactory;
class Document;
class GLFrame;
class GLFrameManager;
class LRegionsSelection;
class MolecularSurfaceCalcTask;
class MolecularSurfaceRendererFactory;
class Task;
class U2Region;
class U2SequenceObject;

/** Each biostruct added to scene should be represented by context */
class BioStruct3DRendererContext {
public:
    BioStruct3DRendererContext(const BioStruct3DObject* obj)
        : obj(obj), biostruct(&obj->getBioStruct3D()) {
    }

    const BioStruct3DObject* obj;
    const BioStruct3D* biostruct;

    QSharedPointer<BioStruct3DGLRenderer> renderer;
    QSharedPointer<BioStruct3DColorScheme> colorScheme;
};

/*!
 * @class BioStruct3DGLWidget BioStruct3DGLWidget.h
 * @brief Class for BioStruct3DObject visualization.
 *
 * This widget provides a 3D graphical view of a macromolecular structure.   In terms
 * of the Model-View architecture we consider
 * the BioStruct3D the model and GLWidget a view of this model.
 *
 * The widget relies on various GLRenderer and ColorScheme subclasses to handle the
 * rendering of the 3d objects.
 *
 * Also it includes actions for visualization control.
 */
class BioStruct3DGLWidget : public QOpenGLWidget {
    Q_OBJECT
    static int widgetCount;

public:
    // Used in PluginChecker to detect whether the GL is available
    static void tryGL();

    /*!
     * Constructor.
     * @param bsObj BioStruct3DObject contains 3d model for visualization
     * @param view DnaView context for connecting structure 3D representation and sequence view
     * @param manager GlFrameManager is required for OpenGL frame manipulation
     * @param parent Parent widget
     */
    BioStruct3DGLWidget(BioStruct3DObject* bsObj, const AnnotatedDNAView* view, GLFrameManager* manager, QWidget* parent);

    //! Destructor.
    ~BioStruct3DGLWidget();

    //! @return BioStruct3D const reference.
    const BioStruct3D& getBioStruct3D() const;

    //! @return BioStruct3D Protein Data Bank id.
    const QString getPDBId() const;

    //! @return BioStruct3DObject name.
    const QString getBioStruct3DObjectName() const;

    /*!
     * @return Menu containing display actions: renderers, color schemes etc.
     */
    QMenu* getDisplayMenu();
    /*!
     * Records widget active settings (part of UGENE architecture).
     * @return Current widgets settings.
     */
    QVariantMap getState();
    /*!
     * Restores widget settings with given (part of UGENE architecture).
     * @param state Widgets settings.
     */
    void setState(const QVariantMap& state);
    /*!
     * Restores default widget settings (viewMatrix, glRenderer, color scheme)
     */
    void restoreDefaultSettings();
    /*!
     * @param delta Positive value: zoom in, negative value: zoom out.
     */
    void zoom(float delta);
    void shift(float deltaX, float deltaY);
    /*!
     * Creates 2d vector image of BioStruct3DGLWidget contents using gl2ps.
     */
    void writeImage2DToFile(int format, int options, int nbcol, const char* fileName);

    void setBackgroundColor(QColor backgroundColor);

    /** @returns This widget GLFrame */
    GLFrame* getGLFrame();

    /** Draws scene without setting camera */
    void draw();

    /** @returns scene rotation center */
    Vector3D getSceneCenter() const;

    /** @returns scene bounding sphere radius */
    float getSceneRadius() const;

    void setImageRenderingMode(bool status);

protected:
    /*!
     * QGlWidget virtual function, initializes OpenGL params. See, Qt docs "QGLWidget" for details.
     */
    void initializeGL();
    /*!
     * QGlWidget virtual function, sets viewport
     * @param width GLWidget width
     * @param height GLWidget height
     */
    void resizeGL(int width, int height);
    /*!
     * QGlWidget virtual function, draw GL scene.
     */
    void paintGL();
    /*!
     * QWidget virtual function, mouse button down event handler.
     */
    void mousePressEvent(QMouseEvent* event);
    /*!
     * QWidget virtual function, mouse move event handler.
     */
    void mouseMoveEvent(QMouseEvent* event);
    /*!
     * QWidget virtual function, mouse wheel event handler.
     */
    void wheelEvent(QWheelEvent* event);
    /*!
     * QWidget virtual function, executes context menu.
     */
    void contextMenuEvent(QContextMenuEvent* _event);

private:
    //! Sets unselected regions shading level
    void setUnselectedShadingLevel(int shading);

    /*!
     * Sets light position.
     * @param pos New light source position. Directional light is being used.
     */
    void setLightPosition(const Vector3D& pos);

    // controller logic
    //! Creates actions for existing GLRenderers, loads default renderer.
    void loadGLRenderers(const QList<QString>& availableRenderers);
    //! Creates actions for existing ColorSchemes, loads default color scheme.
    void loadColorSchemes();

    //! Creates actions for structural alignment
    void createStructuralAlignmentActions();

    //! Creates menu for structural alignment
    QMenu* createStructuralAlignmentMenu();

    void createActions();
    void createMenus();

    // view logic
    /** Adds biostruct object to scene.
     * @param shownModels is a modelId list (same as in PDB)
     */
    void addBiostruct(const BioStruct3DObject* biostruct, const QList<int>& shownModels = QList<int>());

    //! Creates renderers for all biostructs.
    void setupRenderer(const QString& name);

    //! Creates color scheme for all biostructs.
    void setupColorScheme(const QString& name);

    //! Updates color scheme for all renderers.
    void updateAllColorSchemes();

    /** Updates all renderers.
     * Should be called from initializeGL for display list recreation
     */
    void updateAllRenderers();

    /** Setup frame settings: camera clip, camera position */
    void setupFrame();

    void saveDefaultSettings();

    void connectExternalSignals();

    Vector3D getTrackballMapping(int x, int y);
    bool isSyncModeOn();

    // Checks if this widget can render and creates a label with a text error if it cannot
    void checkRenderingAndCreateLblError();

private:
    // related sequences view
    const AnnotatedDNAView* dnaView;

    QList<BioStruct3DRendererContext> contexts;

    // Settings common for all renderers, such as detail level
    BioStruct3DRendererSettings rendererSettings;

    GLFrameManager* frameManager;
    QScopedPointer<GLFrame> glFrame;

    QScopedPointer<MolecularSurface> molSurface;
    QScopedPointer<MolecularSurfaceRenderer> surfaceRenderer;

    AnaglyphStatus anaglyphStatus;
    QScopedPointer<AnaglyphRenderer> anaglyph;

    QVariantMap defaultsSettings;

    // controller logic
    QString currentColorSchemeName;
    QString currentGLRendererName;

    // camera
    GLfloat rotAngle, spinAngle;
    Vector3D rotAxis, lastPos;

    // light
    GLfloat lightPosition[4];

    QColor backgroundColor;
    QColor selectionColor;
    QTimer* animationTimer;

    int unselectedShadingLevel;
    // Should be true when painting an image
    bool imageRenderingMode;

    // controller logic
    QAction* spinAction;
    QAction* settingsAction;
    QAction* closeAction;
    QAction* exportImageAction;

    // actions for selecting/deselecting shown models
    QAction* selectModelsAction;

    // structural alignment related actions
    QAction* alignWithAction;
    QAction* resetAlignmentAction;

    QActionGroup* colorSchemeActions;
    QActionGroup* rendererActions;
    QActionGroup* molSurfaceRenderActions;
    QActionGroup* molSurfaceTypeActions;

    QMenu* selectColorSchemeMenu;
    QMenu* selectRendererMenu;
    QMenu* displayMenu;

    // if OpenGL has error, label is not null and overlaps GlWidget with text "Failed to initialize OpenGL",
    // otherwise label is null
    QLabel* lblGlError;

private slots:
    void sl_selectColorScheme(QAction* action);
    void sl_selectGLRenderer(QAction* action);
    void sl_updateRenderSettings(const QStringList& list);
    void sl_activateSpin();
    void sl_updateAnimation();
    void sl_settings();
    void sl_exportImage();

    // structural alignment related slots
    void sl_alignWith();
    void sl_resetAlignment();
    void sl_onAlignmentDone(Task*);

    // slots for handling sequence selection
    void sl_onSequenceAddedToADV(ADVSequenceObjectContext* ctx);
    void sl_onSequenceRemovedFromADV(ADVSequenceObjectContext* ctx);
    void sl_onSequenceSelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed);

    // slots for select/deselect shown models actions
    // they affect only first biostruct
    void sl_selectModels();

    // slots for surface renderers
    void sl_showSurface();
    void sl_hideSurface();
    void sl_selectSurfaceRenderer(QAction* surfaceRenderer);

public:
    static const QString BACKGROUND_COLOR_NAME;

    static const QString PLUGIN_NAME;
    static const QString COLOR_SCHEME_NAME;
    static const QString RENDERER_NAME;
    static const QString OBJECT_ID_NAME;

    static const QString SELECTION_COLOR_NAME;
    static const QString RENDER_DETAIL_LEVEL_NAME;
    static const QString SHADING_LEVEL_NAME;
    static const QString ANAGLYPH_STATUS_NAME;
};

}  // namespace U2

#endif  // _U2_OPENGL_WIDGET_H_
