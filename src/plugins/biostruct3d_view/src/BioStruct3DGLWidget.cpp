/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <math.h>
#include <time.h>

#include <QImageWriter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QTime>

#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/StructuralAlignmentAlgorithm.h>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ExportImageDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "BioStruct3DColorScheme.h"
#include "BioStruct3DGLImageExportTask.h"
#include "BioStruct3DGLRender.h"
#include "BioStruct3DGLWidget.h"
#include "GLFrameManager.h"
#include "SelectModelsDialog.h"
#include "SettingsDialog.h"
#include "StructuralAlignmentDialog.h"
#include "gl2ps/gl2ps.h"

// disable "unsafe functions" deprecation warnings on MS VS
#ifdef Q_OS_WIN
#    pragma warning(disable : 4996)
#endif

namespace U2 {

int BioStruct3DGLWidget::widgetCount = 0;

void BioStruct3DGLWidget::tryGL() {
    // Basic OpenGL support must be provided for all platforms.
    // If there is no vendor specific OpenGL driver UGENE must fall back to the software impl (see opengl32sw in Qt).
    //    volatile QOpenGLWidget wgt;
    //    Q_UNUSED(wgt);
}

static QColor DEFAULT_BACKGROUND_COLOR = Qt::black;
static QColor DEFAULT_SELECTION_COLOR = Qt::yellow;

static float DEFAULT_RENDER_DETAIL_LEVEL = 1.0;
static int DEFAULT_SHADING_LEVEL = 50;

const QString BioStruct3DGLWidget::BACKGROUND_COLOR_NAME("BackgroundColor");
const QString BioStruct3DGLWidget::PLUGIN_NAME("BioStruct3D Viewer Plugin");
const QString BioStruct3DGLWidget::COLOR_SCHEME_NAME("ColorScheme");
const QString BioStruct3DGLWidget::RENDERER_NAME("GLRenderer");
const QString BioStruct3DGLWidget::OBJECT_ID_NAME("OBJECT_ID");

const QString BioStruct3DGLWidget::SELECTION_COLOR_NAME("SelectionColor");
const QString BioStruct3DGLWidget::SHADING_LEVEL_NAME("Shading Unselected Regions Level");
const QString BioStruct3DGLWidget::RENDER_DETAIL_LEVEL_NAME("RenderDetailLevel");
const QString BioStruct3DGLWidget::ANAGLYPH_STATUS_NAME("AnaglyphStatus");

BioStruct3DGLWidget::BioStruct3DGLWidget(BioStruct3DObject* obj, const AnnotatedDNAView* _dnaView, GLFrameManager* manager, QWidget* parent /* = 0*/)
    : QOpenGLWidget(parent),
      dnaView(_dnaView), contexts(),
      rendererSettings(DEFAULT_RENDER_DETAIL_LEVEL),
      frameManager(manager), glFrame(new GLFrame(this)),
      molSurface(0), surfaceRenderer(),
      anaglyphStatus(DISABLED),
      anaglyph(new AnaglyphRenderer(this, AnaglyphSettings::defaultSettings())),

      defaultsSettings(), currentColorSchemeName(), currentGLRendererName(),
      rotAngle(0), spinAngle(0), rotAxis(), lastPos(),
      backgroundColor(DEFAULT_BACKGROUND_COLOR),
      selectionColor(DEFAULT_SELECTION_COLOR), animationTimer(0),
      unselectedShadingLevel(DEFAULT_SHADING_LEVEL), imageRenderingMode(false),

      spinAction(0), settingsAction(0), closeAction(0), exportImageAction(0), selectModelsAction(0), alignWithAction(0),
      resetAlignmentAction(0), colorSchemeActions(0), rendererActions(0), molSurfaceRenderActions(0),
      molSurfaceTypeActions(0), selectColorSchemeMenu(0), selectRendererMenu(0), displayMenu(0), lblGlError(nullptr) {
    lightPosition[0] = lightPosition[1] = lightPosition[2] = lightPosition[3] = 0;
    GCOUNTER(cvar, "BioStruct3DGLWidget");

    QString currentModelID = obj->getBioStruct3D().pdbId;
    setObjectName(QString("%1-%2").arg(++widgetCount).arg(currentModelID));

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::BIOSTRUCTURE_3D).icon);

    connectExternalSignals();

    currentColorSchemeName = BioStruct3DColorSchemeRegistry::defaultFactoryName();
    currentGLRendererName = BioStruct3DGLRendererRegistry::defaultFactoryName();

    QList<QString> availableRenders = BioStruct3DGLRendererRegistry::getRenderersAvailableFor(obj->getBioStruct3D());
    if (!availableRenders.contains(currentGLRendererName)) {
        currentGLRendererName = availableRenders.first();
    }

    addBiostruct(obj);

    checkRenderingAndCreateLblError();

    createActions();
    createMenus();

    loadColorSchemes();
    loadGLRenderers(availableRenders);

    frameManager->addGLFrame(glFrame.data());
    saveDefaultSettings();
}

BioStruct3DGLWidget::~BioStruct3DGLWidget() {
    uiLog.trace("Biostruct3DGLWdiget " + objectName() + " deleted");
}

void BioStruct3DGLWidget::setupFrame() {
    const float scaleFactor = 2.5;
    float radius = getSceneRadius();
    float camZ = scaleFactor * radius;

    float cameraClipNear = (camZ - radius) * 0.66f;
    float cameraClipFar = (camZ + radius) * 1.2f;

    glFrame->setCameraClip(cameraClipNear, cameraClipFar);

    Vector3D pos = glFrame->getCameraPosition();
    pos.z = camZ;
    glFrame->setCameraPosition(pos);
}

float BioStruct3DGLWidget::getSceneRadius() const {
    // good idea: ask renderer for radius instead of asking biostruct
    float maxRadius = 0;
    const Vector3D sceneCenter = getSceneCenter();

    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        Vector3D center = ctx.biostruct->getCenter();
        float radius = (center - sceneCenter).length() + ctx.biostruct->getRadius();
        if (maxRadius < radius) {
            maxRadius = radius;
        }
    }

    return maxRadius;
}

Vector3D BioStruct3DGLWidget::getSceneCenter() const {
    // good idea: ask renderer for center instead of asking biostruct
    Vector3D c;
    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        // TODO: transform should be applied in BioStruct
        Vector3D tmp = ctx.biostruct->getCenter();
        c += tmp.dot(ctx.biostruct->getTransform());
    }

    return c / float(contexts.length());
}

const BioStruct3D& BioStruct3DGLWidget::getBioStruct3D() const {
    return *(contexts.first().biostruct);
}

const QString BioStruct3DGLWidget::getPDBId() const {
    return contexts.first().biostruct->pdbId;
}

const QString BioStruct3DGLWidget::getBioStruct3DObjectName() const {
    return contexts.first().obj->getGObjectName();
}

void BioStruct3DGLWidget::setImageRenderingMode(bool status) {
    imageRenderingMode = status;
}

GLFrame* BioStruct3DGLWidget::getGLFrame() {
    return glFrame.data();
}

void BioStruct3DGLWidget::initializeGL() {
    setLightPosition(Vector3D(0, 0.0, 1.0));
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0};
    GLfloat light_specular[] = {0.6f, 0.6f, 0.6f, 1.0};
    GLfloat mat_specular[] = {0.6f, 0.6f, 0.6f, 1.0};
    GLfloat mat_shininess[] = {90.0};

    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF());
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_BLEND);  // Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    updateAllRenderers();

    QString videoAdapterString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
    if (videoAdapterString.contains("intel", Qt::CaseInsensitive)) {
        anaglyphStatus = NOT_AVAILABLE;
    } else if (!imageRenderingMode) {
        anaglyph->init();
        if (!anaglyph->isAvailable()) {
            anaglyphStatus = NOT_AVAILABLE;
        }
    }
}

void BioStruct3DGLWidget::resizeGL(int width, int height) {
    glFrame->updateViewPort(width, height);
    if (lblGlError != nullptr) {
        lblGlError->resize(size());
    }
    if (anaglyphStatus == ENABLED) {
        anaglyph->resize(width, height);
    }
}

void BioStruct3DGLWidget::paintGL() {
    if (!isVisible()) {
        return;
    }

    clock_t frameStart = clock();

    // Clear buffers, setup modelview matrix
    // Scene render unable to do this since it used by anaglyph renderer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    if (anaglyphStatus == ENABLED && !imageRenderingMode) {
        // draw using anaglyph renderer
        anaglyph->draw();
    } else {
        // draw using default scene renderer (this)
        draw();
    }

    clock_t frameEnd = clock();
    double frameTime = (frameEnd - frameStart) / (double)CLOCKS_PER_SEC;
    perfLog.trace(QString("BioStruct3DView frame rendering time %1 s").arg(frameTime));
}

void BioStruct3DGLWidget::draw() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Vector3D rotCenter = getSceneCenter();

    glTranslatef(glFrame->getCameraPosition().x, glFrame->getCameraPosition().y, 0);

    glMultMatrixf(glFrame->getRotationMatrix());
    glTranslatef(-rotCenter.x, -rotCenter.y, -rotCenter.z);

    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        glPushMatrix();

        Matrix44 colmt = ctx.biostruct->getTransform();
        colmt.transpose();
        glMultMatrixf(colmt.data());

        ctx.renderer->drawBioStruct3D();
        glPopMatrix();
    }

    if (!molSurface.isNull()) {
        glEnable(GL_CULL_FACE);

        glCullFace(GL_FRONT);
        surfaceRenderer->drawSurface(*molSurface);

        glCullFace(GL_BACK);
        surfaceRenderer->drawSurface(*molSurface);

        glDisable(GL_CULL_FACE);
        CHECK_GL_ERROR;
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_DEPTH_TEST);
}

Vector3D BioStruct3DGLWidget::getTrackballMapping(int x, int y) {
    Vector3D pos;
    /* project x,y onto a hemisphere centered within width, height */
    pos.x = (2.0f * x - width()) / width();
    pos.y = (height() - 2.0f * y) / height();
    pos.z = 0;
    float d = pos.length();
    d = (d < 1.0) ? d : 1.0;
    pos.z = sqrtf(1.0f - d * d);
    pos.normalize();

    return pos;
}

void BioStruct3DGLWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu;
    foreach (QAction* action, getDisplayMenu()->actions()) {
        menu.addAction(action);
    }

    menu.addAction(closeAction);

    menu.exec(event->globalPos());
}

void BioStruct3DGLWidget::setLightPosition(const Vector3D& pos) {
    lightPosition[0] = pos.x;
    lightPosition[1] = pos.y;
    lightPosition[2] = pos.z;
    lightPosition[3] = 1.0;
}

static int getSequenceChainId(const U2SequenceObject* seqObj) {
    bool isFound = false;
    qint64 result = seqObj->getSequenceInfo().value(DNAInfo::CHAIN_ID).toInt(&isFound);
    SAFE_POINT(isFound, "Sequence does not have the CHAIN_ID attribute", -1);
    return (int)result;
}

void BioStruct3DGLWidget::sl_onSequenceSelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed) {
    if (!isVisible())
        return;

    auto selection = qobject_cast<DNASequenceSelection*>(s);
    const U2SequenceObject* seqObj = selection->getSequenceObject();
    assert(seqObj);

    const BioStruct3DRendererContext& ctx = contexts.first();

    // check that biostruct and sequence objects are from the same document
    // appropriate relation check must be here
    if (seqObj->getDocument() == ctx.obj->getDocument()) {
        int chainId = getSequenceChainId(seqObj);
        assert(ctx.biostruct->moleculeMap.contains(chainId));

        ctx.colorScheme->updateSelectionRegion(chainId, added, removed);

        updateAllColorSchemes();
        update();
    }
}

QVariantMap BioStruct3DGLWidget::getState() {
    QVariantMap state;
    glFrame->writeStateToMap(state);
    anaglyph->getSettings().toMap(state);

    state[ANAGLYPH_STATUS_NAME] = QVariant::fromValue((int)anaglyphStatus);

    state[COLOR_SCHEME_NAME] = QVariant::fromValue(currentColorSchemeName);
    state[RENDERER_NAME] = QVariant::fromValue(currentGLRendererName);
    state[OBJECT_ID_NAME] = QVariant::fromValue(getBioStruct3DObjectName());

    state[BACKGROUND_COLOR_NAME] = QVariant::fromValue(backgroundColor);
    state[SELECTION_COLOR_NAME] = QVariant::fromValue(selectionColor);

    state[RENDER_DETAIL_LEVEL_NAME] = QVariant::fromValue(rendererSettings.detailLevel);
    state[SHADING_LEVEL_NAME] = QVariant::fromValue(unselectedShadingLevel);

    return state;
}

void BioStruct3DGLWidget::setState(const QVariantMap& state) {
    // bug-2859: correct save/restore current selection.
    if (state.isEmpty()) {
        return;
    }
    glFrame->makeCurrent();
    glFrame->setState(state);

    anaglyphStatus = (AnaglyphStatus)state.value(ANAGLYPH_STATUS_NAME).value<int>();
    anaglyph->setSettings(AnaglyphSettings::fromMap(state));

    backgroundColor = state.value(BACKGROUND_COLOR_NAME, DEFAULT_BACKGROUND_COLOR).value<QColor>();
    setBackgroundColor(backgroundColor);

    selectionColor = state.value(SELECTION_COLOR_NAME, DEFAULT_SELECTION_COLOR).value<QColor>();

    rendererSettings.detailLevel = state.value(RENDER_DETAIL_LEVEL_NAME, DEFAULT_RENDER_DETAIL_LEVEL).value<float>();

    QString previousColorSchemeName = currentColorSchemeName;
    QString previousGLRendererName = currentGLRendererName;

    currentColorSchemeName = state.value(COLOR_SCHEME_NAME, BioStruct3DColorSchemeRegistry::defaultFactoryName()).value<QString>();
    currentGLRendererName = state.value(RENDERER_NAME, BioStruct3DGLRendererRegistry::defaultFactoryName()).value<QString>();

    if (previousColorSchemeName != currentColorSchemeName) {
        setupColorScheme(currentColorSchemeName);
    }

    unselectedShadingLevel = state.value(SHADING_LEVEL_NAME, DEFAULT_SHADING_LEVEL).value<int>();
    setUnselectedShadingLevel(unselectedShadingLevel);

    if (previousGLRendererName != currentGLRendererName) {
        setupRenderer(currentGLRendererName);
    }

    resizeGL(width(), height());
    update();
}

void BioStruct3DGLWidget::setupColorScheme(const QString& name) {
    QList<BioStruct3DRendererContext>::iterator i = contexts.begin();
    for (; i != contexts.end(); ++i) {
        BioStruct3DRendererContext& ctx = *(i);

        // TODO: this situation may be potentialy dangerous
        // if renderer starts draw right now, maybe SharedPointer will be good solution
        BioStruct3DColorScheme* scheme = BioStruct3DColorSchemeRegistry::createColorScheme(name, ctx.obj);
        assert(scheme);

        scheme->setSelectionColor(selectionColor);
        scheme->setUnselectedShadingLevel((double)unselectedShadingLevel / 100.0);

        ctx.colorScheme = QSharedPointer<BioStruct3DColorScheme>(scheme);
        ctx.renderer->setColorScheme(scheme);
    }
}

void BioStruct3DGLWidget::setupRenderer(const QString& name) {
    QList<BioStruct3DRendererContext>::iterator i = contexts.begin();
    for (; i != contexts.end(); ++i) {
        BioStruct3DRendererContext& ctx = *(i);

        // TODO: this situation may be potentialy dangerous
        // if renderer starts draw right now, maybe SharedPointer will be good solution
        const QList<int>& shownModelsIndexes = ctx.renderer->getShownModelsIds();
        BioStruct3DGLRenderer* rend = BioStruct3DGLRendererRegistry::createRenderer(name, *ctx.biostruct, ctx.colorScheme.data(), shownModelsIndexes, &rendererSettings);
        assert(rend);
        ctx.renderer = QSharedPointer<BioStruct3DGLRenderer>(rend);
    }
}

void BioStruct3DGLWidget::updateAllColorSchemes() {
    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        ctx.renderer->updateColorScheme();
    }
}

void BioStruct3DGLWidget::updateAllRenderers() {
    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        ctx.renderer->update();
    }
}

void BioStruct3DGLWidget::setBackgroundColor(QColor backgroundColor) {
    this->backgroundColor = backgroundColor;
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF());
}

void BioStruct3DGLWidget::zoom(float delta) {
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.data(), syncLock);
    foreach (GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->performZoom(delta);
        frame->updateViewPort();
        frame->updateGL();
    }
}

void BioStruct3DGLWidget::shift(float deltaX, float deltaY) {
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.data(), syncLock);
    foreach (GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->performShift(deltaX, deltaY);
        frame->updateViewPort();
        frame->updateGL();
    }
}

void BioStruct3DGLWidget::saveDefaultSettings() {
    glFrame->writeStateToMap(defaultsSettings);
    defaultsSettings[COLOR_SCHEME_NAME] = QVariant::fromValue(currentColorSchemeName);
    defaultsSettings[RENDERER_NAME] = QVariant::fromValue(currentGLRendererName);
}

void BioStruct3DGLWidget::restoreDefaultSettings() {
    assert(!defaultsSettings.isEmpty());
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.data(), syncLock);
    foreach (GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->setState(defaultsSettings);
        frame->updateViewPort();
        frame->updateGL();
    }
}

void BioStruct3DGLWidget::sl_selectModels() {
    BioStruct3DRendererContext& ctx = contexts.first();
    QObjectScopedPointer<SelectModelsDialog> dlg = new SelectModelsDialog(ctx.biostruct->getModelsIds(), ctx.renderer->getShownModelsIds(), this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        ctx.renderer->setShownModelsIndexes(dlg->getSelectedModelsIds());

        contexts.first().renderer->updateShownModels();
        update();
    }
}

void BioStruct3DGLWidget::writeImage2DToFile(int format, int options, int nbcol, const char* fileName) {
    FILE* fp = nullptr;
    const char* FOPEN_ARGS = "wb";
    const QByteArray title(fileName);
    int state = GL2PS_OVERFLOW, buffsize = 0;
    GLint viewport[4];
    int sort = GL2PS_SIMPLE_SORT;

    fp = fopen(fileName, FOPEN_ARGS);

    if (!fp) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to open file %1 for writing").arg(fileName));
        return;
    }

    if (format == GL2PS_EPS) {
        // hack -> make widget aspect ratio 1:1
        if (width() > height()) {
            int size = height();
            resize(size, size);
        }
    }

    glFrame->updateViewPort();
    glGetIntegerv(GL_VIEWPORT, viewport);

    while (state == GL2PS_OVERFLOW) {
        buffsize += 2048 * 2048;
        gl2psBeginPage(title.constData(),
                       "Unipro UGENE BioStruct3D Viewer plugin",
                       viewport,
                       format,
                       sort,
                       options,
                       GL_RGBA,
                       0,
                       nullptr,
                       nbcol,
                       nbcol,
                       nbcol,
                       buffsize,
                       fp,
                       fileName);
        paintGL();
        state = gl2psEndPage();
    }

    fclose(fp);

    if (format == GL2PS_EPS) {
        // restore sizes
        updateGeometry();
    }
}

void BioStruct3DGLWidget::loadColorSchemes() {
    currentColorSchemeName = BioStruct3DColorSchemeRegistry::defaultFactoryName();

    // highlight default color scheme in menu
    QList<QAction*>::iterator iter;
    QList<QAction*> schemeActions = colorSchemeActions->actions();
    for (iter = schemeActions.begin(); iter != schemeActions.end(); ++iter) {
        if ((*iter)->text() == currentColorSchemeName) {
            (*iter)->setChecked(true);
            break;
        }
    }
    assert(iter != schemeActions.end());
}

void BioStruct3DGLWidget::loadGLRenderers(const QList<QString>& availableRenderers) {
    // highlight current renderer in menu

    foreach (QAction* ac, rendererActions->actions()) {
        // disable all unavailable renderers in menu
        if (!availableRenderers.contains(ac->text())) {
            ac->setDisabled(true);
        }

        if (ac->text() == currentGLRendererName) {
            ac->setChecked(true);
        }
    }

    QString surfaceRendererName = ConvexMapRenderer::ID;
    surfaceRenderer.reset(MolecularSurfaceRendererRegistry::createMSRenderer(surfaceRendererName));
}

bool BioStruct3DGLWidget::isSyncModeOn() {
    Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    bool synchronizationMode = km.testFlag(Qt::ShiftModifier) || frameManager->getSyncLock();
    synchronizationMode &= frameManager->getGLFrames().count() > 1;
    return synchronizationMode;
}

void BioStruct3DGLWidget::checkRenderingAndCreateLblError() {
    QOffscreenSurface surf;
    QOpenGLContext ctx;
    surf.create();
    ctx.create();
    ctx.makeCurrent(&surf);

    GLenum error = glGetError();
    bool canRender = error == GL_NO_ERROR;
    if (!canRender) {
        QString errorDetails = QString("(%1): %2").arg(error).arg(reinterpret_cast<const char*>(gluErrorString(error)));
        coreLog.info(tr("The \"3D Structure Viewer\" was disabled, because OpenGL has error ") + errorDetails);
        lblGlError = new QLabel("Failed to initialize OpenGL: " + errorDetails, this);
        lblGlError->setObjectName("opengl_initialization_error_label");
        lblGlError->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
        lblGlError->setStyleSheet("QLabel { background-color : black; color : white; }");
    }
}

void BioStruct3DGLWidget::setUnselectedShadingLevel(int shading) {
    foreach (const BioStruct3DRendererContext& ctx, contexts) {
        ctx.colorScheme->setUnselectedShadingLevel((double)shading / 100.0);
    }
    updateAllColorSchemes();
}

QMenu* BioStruct3DGLWidget::getDisplayMenu() {
    assert(displayMenu != nullptr);
    return displayMenu;
}

void BioStruct3DGLWidget::mousePressEvent(QMouseEvent* event) {
    lastPos = getTrackballMapping(event->x(), event->y());
}

void BioStruct3DGLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        Vector3D curPos = getTrackballMapping(event->x(), event->y());
        Vector3D delta = curPos - lastPos;

        if (delta.x || delta.y || delta.z) {
            rotAngle = 90.0f * delta.length();
            rotAxis = vector_cross(lastPos, curPos);

            bool syncLock = isSyncModeOn();
            QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.data(), syncLock);
            foreach (GLFrame* frame, frames) {
                frame->makeCurrent();

                if (event->modifiers() & Qt::CTRL)
                    frame->performShift(delta.x, delta.y);
                else
                    frame->rotateCamera(rotAxis, rotAngle);

                frame->updateGL();
            }
        }

        lastPos = curPos;
    }
}

void BioStruct3DGLWidget::wheelEvent(QWheelEvent* event) {
    float numDegrees = event->angleDelta().y() / 8;
    zoom(numDegrees / 10);
}

void BioStruct3DGLWidget::createActions() {
    QAction* action = nullptr;

    animationTimer = new QTimer(this);
    animationTimer->setInterval(20);  // fixed interval
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(sl_updateAnimation()));

    rendererActions = new QActionGroup(this);
    connect(rendererActions, SIGNAL(triggered(QAction*)), this, SLOT(sl_selectGLRenderer(QAction*)));

    foreach (const QString& key, BioStruct3DGLRendererRegistry::factoriesNames()) {
        action = new QAction(key, rendererActions);
        action->setCheckable(true);
        action->setObjectName(action->text());
    }

    colorSchemeActions = new QActionGroup(this);
    connect(colorSchemeActions, SIGNAL(triggered(QAction*)), this, SLOT(sl_selectColorScheme(QAction*)));
    foreach (const QString& key, BioStruct3DColorSchemeRegistry::factoriesNames()) {
        action = new QAction(key, colorSchemeActions);
        action->setCheckable(true);
        action->setObjectName(key);
    }

    molSurfaceRenderActions = new QActionGroup(this);
    connect(molSurfaceRenderActions, SIGNAL(triggered(QAction*)), this, SLOT(sl_selectSurfaceRenderer(QAction*)));
    foreach (const QString& key, MolecularSurfaceRendererRegistry::factoriesNames()) {
        action = new QAction(key, molSurfaceRenderActions);
        action->setCheckable(true);
        if (key == ConvexMapRenderer::ID) {
            action->setChecked(true);
        }
    }

    molSurfaceTypeActions = new QActionGroup(this);
    foreach (QString key, AppContext::getMolecularSurfaceFactoryRegistry()->getSurfNameList()) {
        action = new QAction(key, molSurfaceTypeActions);
        action->setObjectName(key);
        connect(action, SIGNAL(triggered()), this, SLOT(sl_showSurface()));
        action->setCheckable(true);
        bool hasConstraints = AppContext::getMolecularSurfaceFactoryRegistry()->getSurfaceFactory(key)->hasConstraints(*contexts.first().biostruct);
        action->setEnabled(!hasConstraints);
    }
    action = new QAction(tr("Off"), molSurfaceTypeActions);
    connect(action, SIGNAL(triggered()), this, SLOT(sl_hideSurface()));
    action->setCheckable(true);
    action->setChecked(true);

    selectModelsAction = 0;
    if (!contexts.isEmpty() && contexts.first().biostruct->getModelsIds().size() > 1) {
        selectModelsAction = new QAction(tr("Models.."), this);
        connect(selectModelsAction, SIGNAL(triggered()), this, SLOT(sl_selectModels()));
    }

    spinAction = new QAction(tr("Spin"), this);
    spinAction->setCheckable(true);
    connect(spinAction, SIGNAL(triggered()), this, SLOT(sl_activateSpin()));

    settingsAction = new QAction(tr("Settings..."), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(sl_settings()));

    closeAction = new QAction(tr("Close"), this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    exportImageAction = new QAction(tr("Export Image..."), this);
    exportImageAction->setObjectName("bioStruct3DExportImageAction");
    connect(exportImageAction, SIGNAL(triggered()), this, SLOT(sl_exportImage()));

    createStructuralAlignmentActions();
}

void BioStruct3DGLWidget::createStructuralAlignmentActions() {
    alignWithAction = new QAction(tr("Align With..."), this);
    alignWithAction->setObjectName("align_with");
    connect(alignWithAction, SIGNAL(triggered()), this, SLOT(sl_alignWith()));

    resetAlignmentAction = new QAction(tr("Reset"), this);
    connect(resetAlignmentAction, SIGNAL(triggered()), this, SLOT(sl_resetAlignment()));
}

void BioStruct3DGLWidget::createMenus() {
    // Renderer selection
    selectRendererMenu = new QMenu(tr("Render Style"));
    selectRendererMenu->addActions(rendererActions->actions());
    selectRendererMenu->menuAction()->setObjectName("Render Style");

    // Color scheme selection
    selectColorSchemeMenu = new QMenu(tr("Coloring Scheme"));
    selectColorSchemeMenu->addActions(colorSchemeActions->actions());
    selectColorSchemeMenu->menuAction()->setObjectName("Coloring Scheme");

    // Molecular surface
    auto surfaceRenderingStyleMenu = new QMenu(tr("Molecular Surface Render Style"));
    surfaceRenderingStyleMenu->addActions(molSurfaceRenderActions->actions());
    surfaceRenderingStyleMenu->menuAction()->setObjectName("Molecular Surface Render Style");

    auto surfaceTypeMenu = new QMenu(tr("Molecular Surface"));
    surfaceTypeMenu->addActions(molSurfaceTypeActions->actions());
    surfaceTypeMenu->menuAction()->setObjectName("Molecular Surface");

    // Display (context) menu
    displayMenu = new QMenu(this);
    displayMenu->addMenu(selectRendererMenu);
    displayMenu->addMenu(selectColorSchemeMenu);

    displayMenu->addSeparator();
    displayMenu->addMenu(surfaceTypeMenu);
    displayMenu->addMenu(surfaceRenderingStyleMenu);
    displayMenu->addSeparator();

    if (selectModelsAction) {
        displayMenu->addAction(selectModelsAction);
    }

    displayMenu->addAction(spinAction);
    displayMenu->addAction(settingsAction);
    displayMenu->addAction(exportImageAction);

    auto structuralAlignmentMenu = createStructuralAlignmentMenu();
    displayMenu->addMenu(structuralAlignmentMenu);
}

QMenu* BioStruct3DGLWidget::createStructuralAlignmentMenu() {
    auto saMenu = new QMenu(tr("Structural Alignment"));
    saMenu->menuAction()->setObjectName("Structural Alignment");

    saMenu->addAction(alignWithAction);
    saMenu->addAction(resetAlignmentAction);

    return saMenu;
}

void BioStruct3DGLWidget::connectExternalSignals() {
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    connect(asr, SIGNAL(si_annotationSettingsChanged(const QStringList&)), this, SLOT(sl_updateRenderSettings(const QStringList&)));

    const QList<ADVSequenceObjectContext*> seqContexts = dnaView->getSequenceContexts();

    foreach (ADVSequenceObjectContext* ctx, seqContexts) {
        connect(ctx->getSequenceSelection(),
                SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
                SLOT(sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));
    }

    connect(dnaView,
            SIGNAL(si_sequenceAdded(ADVSequenceObjectContext*)),
            SLOT(sl_onSequenceAddedToADV(ADVSequenceObjectContext*)));

    connect(dnaView,
            SIGNAL(si_sequenceRemoved(ADVSequenceObjectContext*)),
            SLOT(sl_onSequenceRemovedFromADV(ADVSequenceObjectContext*)));
}

void BioStruct3DGLWidget::sl_onSequenceAddedToADV(ADVSequenceObjectContext* ctx) {
    connect(ctx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));
}

void BioStruct3DGLWidget::sl_onSequenceRemovedFromADV(ADVSequenceObjectContext* ctx) {
    disconnect(ctx->getSequenceSelection(), SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)), this, SLOT(sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));
}

void BioStruct3DGLWidget::sl_selectColorScheme(QAction* action) {
    QString schemeName = action->text();

    currentColorSchemeName = schemeName;
    setupColorScheme(schemeName);

    GLFrame* frame = frameManager->getGLWidgetFrame(this);
    frame->makeCurrent();
    frame->updateGL();
}

void BioStruct3DGLWidget::sl_updateRenderSettings(const QStringList& list) {
    Q_UNUSED(list);
    sl_selectColorScheme(colorSchemeActions->checkedAction());
}

void BioStruct3DGLWidget::sl_activateSpin() {
    if (spinAction->isChecked()) {
        animationTimer->start();
    } else {
        animationTimer->stop();
    }

    update();
}

void BioStruct3DGLWidget::sl_updateAnimation() {
    static float velocity = 0.05f;
    spinAngle = velocity * animationTimer->interval();
    Vector3D rotAxis(0, 1, 0);
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.data(), syncLock);

    foreach (GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->rotateCamera(rotAxis, spinAngle);
        frame->updateGL();
    }
    update();
}

void BioStruct3DGLWidget::sl_selectGLRenderer(QAction* action) {
    QString rendererName = action->text();
    currentGLRendererName = rendererName;
    setupRenderer(currentGLRendererName);

    GLFrame* frame = frameManager->getGLWidgetFrame(this);
    frame->makeCurrent();
    frame->updateGL();
}

void BioStruct3DGLWidget::sl_settings() {
    QObjectScopedPointer<BioStruct3DSettingsDialog> dialog = new BioStruct3DSettingsDialog();

    dialog->setWidget(this);

    dialog->setBackgroundColor(backgroundColor);
    dialog->setSelectionColor(selectionColor);
    dialog->setRenderDetailLevel(rendererSettings.detailLevel);
    dialog->setShadingLevel(unselectedShadingLevel);

    dialog->setAnaglyphStatus(anaglyphStatus);
    dialog->setAnaglyphSettings(anaglyph->getSettings());

    QVariantMap previousState = getState();

    dialog->exec();
    CHECK(!dialog.isNull(), );

    if (QDialog::Accepted == dialog->result()) {
        backgroundColor = dialog->getBackgroundColor();
        selectionColor = dialog->getSelectionColor();
        unselectedShadingLevel = dialog->getShadingLevel();

        foreach (const BioStruct3DRendererContext& ctx, contexts) {
            ctx.colorScheme->setSelectionColor(selectionColor);
        }
        setUnselectedShadingLevel(unselectedShadingLevel);

        rendererSettings.detailLevel = dialog->getRenderDetailLevel();

        anaglyphStatus = dialog->getAnaglyphStatus();
        anaglyph->setSettings(dialog->getAnaglyphSettings());

        this->makeCurrent();
        setBackgroundColor(backgroundColor);

        update();
    } else {
        setState(previousState);
    }
}

void BioStruct3DGLWidget::sl_exportImage() {
    BioStruct3DImageExportController factory(this);
    QString fileName = GUrlUtils::fixFileName(getBioStruct3DObjectName());
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(&factory, ExportImageDialog::MolView, fileName, ExportImageDialog::SupportScaling, this);
    dialog->exec();
}

void BioStruct3DGLWidget::sl_showSurface() {
    QList<SharedAtom> atoms;
    BioStruct3DRendererContext ctx = contexts.first();
    atoms = ctx.biostruct->getAllAtoms();

    QString surfaceType = qobject_cast<QAction*>(sender())->text();
    auto surfaceCalcTask = new MolecularSurfaceCalcTask(surfaceType, atoms);
    connect(new TaskSignalMapper(surfaceCalcTask), &TaskSignalMapper::si_taskSucceeded, this, [this](Task* task) {
        auto surfaceTask = qobject_cast<MolecularSurfaceCalcTask*>(task);
        SAFE_POINT_NN(surfaceTask, );
        molSurface.reset(surfaceTask->getCalculatedSurface());
        makeCurrent();
        update();
    });
    AppContext::getTaskScheduler()->registerTopLevelTask(surfaceCalcTask);
}

void BioStruct3DGLWidget::sl_hideSurface() {
    molSurface.reset();

    makeCurrent();
    update();
}

void BioStruct3DGLWidget::sl_selectSurfaceRenderer(QAction* action) {
    QString msRendererName = action->text();
    surfaceRenderer.reset(MolecularSurfaceRendererRegistry::createMSRenderer(msRendererName));

    makeCurrent();
    update();
}

void BioStruct3DGLWidget::addBiostruct(const BioStruct3DObject* obj, const QList<int>& shownModels /* = QList<int>()*/) {
    assert(contexts.size() < 2 && "Multiple models in one view is unsupported now");
    BioStruct3DRendererContext ctx(obj);

    QList<int> shownModelsIds = shownModels;
    // Show only first model if model list is empty.
    if (shownModelsIds.isEmpty()) {
        shownModelsIds << obj->getBioStruct3D().modelMap.keys().first();
    }

    BioStruct3DColorScheme* colorScheme = BioStruct3DColorSchemeRegistry::createColorScheme(currentColorSchemeName, ctx.obj);
    assert(colorScheme);
    ctx.colorScheme = QSharedPointer<BioStruct3DColorScheme>(colorScheme);
    ctx.colorScheme->setSelectionColor(selectionColor);
    ctx.colorScheme->setUnselectedShadingLevel((double)unselectedShadingLevel / 100.0);

    BioStruct3DGLRenderer* renderer = BioStruct3DGLRendererRegistry::createRenderer(currentGLRendererName, *ctx.biostruct, ctx.colorScheme.data(), shownModelsIds, &rendererSettings);
    assert(renderer);
    ctx.renderer = QSharedPointer<BioStruct3DGLRenderer>(renderer);

    contexts.append(ctx);
    setupRenderer(currentGLRendererName);

    setupFrame();
}

void BioStruct3DGLWidget::sl_alignWith() {
    const BioStruct3DRendererContext& ctx = contexts.first();
    int currentModelId = ctx.renderer->getShownModelsIds().first();

    QObjectScopedPointer<StructuralAlignmentDialog> dlg = new StructuralAlignmentDialog(contexts.first().obj, currentModelId);
    int dialogResult = dlg->execIfAlgorithmAvailable();
    CHECK(!dlg.isNull(), );

    if (dialogResult == QDialog::Accepted) {
        sl_resetAlignment();

        Task* task = dlg->getTask();
        assert(task && "If dialog accepded it must return valid task");

        auto taskMapper = new TaskSignalMapper(task);
        connect(taskMapper, SIGNAL(si_taskFinished(Task*)), this, SLOT(sl_onAlignmentDone(Task*)));

        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void BioStruct3DGLWidget::sl_resetAlignment() {
    assert(contexts.size() < 3 && "Multiple models in one view is unsupported now");
    if (contexts.size() == 2) {
        contexts.removeLast();
        setupFrame();

        glFrame->makeCurrent();
        update();
    }
}

void BioStruct3DGLWidget::sl_onAlignmentDone(Task* task) {
    if (!task->hasError()) {
        auto saTask = qobject_cast<StructuralAlignmentTask*>(task);
        assert(saTask && "Task should have type StructuralAlignmentTask");

        StructuralAlignment result = saTask->getResult();
        StructuralAlignmentTaskSettings settings = saTask->getSettings();

        const Matrix44& mt = result.transform;
        const_cast<BioStruct3D*>(&settings.alt.obj->getBioStruct3D())->setTransform(mt);

        addBiostruct(settings.alt.obj, QList<int>() << settings.alt.modelId);

        glFrame->makeCurrent();
        update();
    }
}

}  // namespace U2
