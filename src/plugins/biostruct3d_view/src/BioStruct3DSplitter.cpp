/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPixmap>

#include <QFile>
#include <QTextStream>
#include <QUrl>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/LoadDocumentTask.h>

#include <U2Gui/OrderedToolbar.h>
#include <U2Gui/ProjectTreeItemSelectorDialog.h>
#include <U2Gui/ProjectTreeController.h>
#include <U2Gui/WidgetWithLocalToolbar.h>

#include <U2View/AnnotatedDNAView.h>

#include "BioStruct3DSplitter.h"
#include "BioStruct3DGLWidget.h"
#include "GLFrameManager.h"


namespace U2 {

#define HEADER_HEIGHT 24
#define MAX_SPLITTER_HEIGHT 1000

BioStruct3DSplitter::BioStruct3DSplitter(QAction* _closeAction, AnnotatedDNAView* view)
    : ADVSplitWidget(view),
      glFrameManager(new GLFrameManager),
      parentSplitter(NULL)
{
    closeAction = _closeAction;

    layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,3,0);
    splitter = new QSplitter(Qt::Horizontal);
    splitterHeight = 400;
    setMinimumHeight(HEADER_HEIGHT);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setAcceptDrops(true);

    widgetWithToolbar = new WidgetWithLocalToolbar(this);
    QLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(splitter);
    widgetWithToolbar->setContentLayout(l);

    header = new SplitterHeaderWidget(this);

    layout->addWidget(header);
    layout->addWidget(widgetWithToolbar);

    isViewCollapsed = false;

    setLayout(layout);
}

bool BioStruct3DSplitter::acceptsGObject( GObject* obj )
{
    if (obj->getGObjectType() == GObjectTypes::BIOSTRUCTURE_3D) {
        return true;
    } else {
        return false;
    }

}

BioStruct3DGLWidget* BioStruct3DSplitter::addBioStruct3DGLWidget( BioStruct3DObject* bioStructObj )
{
    if (isViewCollapsed) {
        adaptSize(1);
    }

    BioStruct3DGLWidget* glWidget = new BioStruct3DGLWidget(bioStructObj, dnaView, getGLFrameManager(), this);
    glWidget->installEventFilter(this);
    // Add widget to splitter
    biostrucViewMap.insert(bioStructObj, glWidget);
    splitter->addWidget(glWidget);
    emit si_bioStruct3DGLWidgetAdded(glWidget);

    return glWidget;
}


void BioStruct3DSplitter::removeBioStruct3DGLWidget( BioStruct3DGLWidget* glw )
{
    // safe to remove widgets from map - it will be deleted later when
    // splitter is deleted
    QMutableMapIterator<BioStruct3DObject*, BioStruct3DGLWidget*> i(biostrucViewMap);
    while (i.hasNext()) {
        if (i.next().value() == glw) {
            i.remove();
            break;
        }

    }

    glFrameManager->removeGLWidgetFrame(glw);
    emit si_bioStruct3DGLWidgetRemoved(glw);

    glw->deleteLater();
}



bool BioStruct3DSplitter::eventFilter(QObject* o, QEvent* e) {
    Q_UNUSED(o); Q_UNUSED(e);
    BioStruct3DGLWidget* glw = qobject_cast<BioStruct3DGLWidget*>(o);
#ifdef Q_WS_X11
    //first variant of fix of QT4 bug: GL widget is frozen after minimize/maximize
    if (e->type() == QEvent::Hide) {
        glw->makeCurrent(); //these 2 lines can be removed
        glFinish();
        return true;
    }
    //second variant of fix of QT4 bug: GL widget is frozen after minimize/maximize
    if (e->type() == QEvent::WindowActivate) {
    //  ((QGLContext*)glw->context())->create();
    }
#endif
    if (e->type() == QEvent::Close && glw != NULL) {
        removeBioStruct3DGLWidget(glw);
        if (biostrucViewMap.isEmpty()) {
            closeAction->trigger();
        }
    }
    return false;
}

void BioStruct3DSplitter::addObject(BioStruct3DObject* bioStructObj) {
    if (!biostrucViewMap.contains(bioStructObj)) {
        setVisible(true);
        addBioStruct3DGLWidget(bioStructObj);
    }
}

bool BioStruct3DSplitter::removeObject(BioStruct3DObject* obj) {
    QList<BioStruct3DGLWidget*> toDelete = biostrucViewMap.values(obj);
    foreach(BioStruct3DGLWidget* glw, toDelete) {
        removeBioStruct3DGLWidget(glw);
    }
    biostrucViewMap.remove(obj);
    bool isEmpty = biostrucViewMap.isEmpty();
    return isEmpty;
}


void BioStruct3DSplitter::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* md = event->mimeData();
    const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
    if (gomd != NULL) {
        GObject* obj = gomd->objPtr.data();
        if (obj->getGObjectType() == GObjectTypes::BIOSTRUCTURE_3D) {
            BioStruct3DObject* bioStrucObj = qobject_cast<BioStruct3DObject*>(gomd->objPtr.data());
            if (biostrucViewMap.contains(bioStrucObj))
                event->acceptProposedAction();
        }
    }
}

void BioStruct3DSplitter::dropEvent(QDropEvent *event)
{
    const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(event->mimeData());
    BioStruct3DObject* bioStrucObj = qobject_cast<BioStruct3DObject*>(gomd->objPtr.data());
    Q_ASSERT(bioStrucObj != NULL);
    addBioStruct3DGLWidget(bioStrucObj);

}

void BioStruct3DSplitter::addActionToLocalToolBar(QAction *action) {
    widgetWithToolbar->addActionToLocalToolbar(action);
}

#define SPLITTER_STATE_MAP_NAME     "BIOSTRUCT3DSPLITTER_MAP"
#define WIDGET_STATE_LIST_NAME      "WIDGET_STATE_MAP"
#define OBJECT_ID_NAME              "OBJECT_ID"

BioStruct3DSplitter::~BioStruct3DSplitter() {
    uiLog.trace("BioStruct3DSplitter deleted");
}

void BioStruct3DSplitter::saveState( QVariantMap& m )
{
    QVariantMap splitterStateMap = m.value(SPLITTER_STATE_MAP_NAME).toMap();
    QVariantList glWidgetStateList = m.value(WIDGET_STATE_LIST_NAME).toList();

    foreach (BioStruct3DGLWidget* glWidget, biostrucViewMap) {
        QVariantMap widgetState = glWidget->getState();
        glWidgetStateList.append(widgetState);
    }

    splitterStateMap[WIDGET_STATE_LIST_NAME] = glWidgetStateList;
    m[SPLITTER_STATE_MAP_NAME] = splitterStateMap;

}


void BioStruct3DSplitter::updateState( const QVariantMap& m )
{
    const QVariantMap splitterStateMap = m.value(SPLITTER_STATE_MAP_NAME).toMap();
    const QVariantList glWidgetStateList = splitterStateMap.value(WIDGET_STATE_LIST_NAME).toList();

    // do nothing if the widget was closed
    if (glWidgetStateList.isEmpty()) {
        return;
    }

    foreach (BioStruct3DGLWidget* widget, biostrucViewMap.values()) {
        widget->hide();
        removeBioStruct3DGLWidget(widget);
    }

    assert(biostrucViewMap.isEmpty());

    setVisible(true);
    QListIterator<QVariant> iter(glWidgetStateList);
    iter.toBack();
    while (iter.hasPrevious()) {
        QVariantMap state = iter.previous().toMap();
        QString objName = state.value(OBJECT_ID_NAME).value<QString>();
        BioStruct3DObject* obj = findBioStruct3DObjByName(objName);
        if (obj == NULL)
            continue;
        BioStruct3DGLWidget* glWidget = addBioStruct3DGLWidget(obj);
        if (!dnaView->getObjects().contains(obj)) {
            dnaView->addObject(obj);
        }
        glWidget->setState(state);
    }

    splitter->update();

}


BioStruct3DObject* BioStruct3DSplitter::findBioStruct3DObjByName( const QString& objName )
{
    // TODO: optimize this by caching objects
    QList<Document*> docs = AppContext::getProject()->getDocuments();
    foreach (Document* doc, docs) {
        QList<GObject*> biostructObjs = doc->findGObjectByType(GObjectTypes::BIOSTRUCTURE_3D);
        if (!biostructObjs.empty()) {
            Q_ASSERT(biostructObjs.size() == 1);
            BioStruct3DObject* obj = qobject_cast<BioStruct3DObject*>(biostructObjs.first());
            Q_ASSERT(obj != NULL);
            if (obj->getGObjectName() == objName) {
                return obj;
            }
        }
    }

    return NULL;
}

QSplitter* BioStruct3DSplitter::getParentSplitter()
{
    if (parentSplitter == NULL) {
        QWidget* widget = parentWidget();
        Q_ASSERT(widget != NULL);
        parentSplitter = qobject_cast<QSplitter* > (widget);
    }

    return parentSplitter;
}

void BioStruct3DSplitter::adaptSize( int numVisibleWidgets )
{
    if (numVisibleWidgets > 0) {
        isViewCollapsed = false;
        setMaximumHeight(1000);
        getParentSplitter();
        // TODO: Simplify resizing
        int index = parentSplitter->indexOf(this);
        QList<int> sizes = parentSplitter->sizes();
        for(int i = 0; i < sizes.count(); ++i) {
            if (sizes.at(i) >= splitterHeight) {
                sizes[i] = sizes.at(i) - splitterHeight;
                break;
            }
        }
        sizes[index] = splitterHeight;
        parentSplitter->setSizes(sizes);
    } else {
        splitterHeight = splitter->height();
        setFixedHeight(HEADER_HEIGHT);
        isViewCollapsed = true;
    }
}

const QList<QAction* > BioStruct3DSplitter::getSettingsMenuActions() const
{
    QList<QAction*> actions;

    return actions;
}

QList<BioStruct3DGLWidget*> BioStruct3DSplitter::getChildWidgets() const
{
    return biostrucViewMap.values();
}

int BioStruct3DSplitter::getNumVisibleWidgets()
{
    int numVisibleWidgets = 0;
    foreach (BioStruct3DGLWidget* widget, biostrucViewMap) {
        if (widget->isVisible()) {
            ++numVisibleWidgets;
        }
    }
    return numVisibleWidgets;
}

void BioStruct3DSplitter::addModelFromObject( BioStruct3DObject* obj )
{
    QList<GObject*> objects = dnaView->getObjects();
    if (objects.contains(obj)) {
        addBioStruct3DGLWidget(obj);
    } else {
      dnaView->addObject(obj);
    }
}

GLFrameManager* BioStruct3DSplitter::getGLFrameManager()
{
    return glFrameManager.data();
}

///////////////////////////////////////////////////////////////////////////////////////////
// Header widget

SplitterHeaderWidget::SplitterHeaderWidget(BioStruct3DSplitter* sp) : splitter(sp)
{
    setFixedHeight(HEADER_HEIGHT);
    setMinimumHeight(HEADER_HEIGHT);
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
    registerWebUrls();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10,0,0,0);
    layout->setSpacing(10);

    connect(splitter, SIGNAL(si_bioStruct3DGLWidgetAdded(BioStruct3DGLWidget* )),
        SLOT(sl_bioStruct3DGLWidgetAdded(BioStruct3DGLWidget*)));

    connect(splitter, SIGNAL(si_bioStruct3DGLWidgetRemoved(BioStruct3DGLWidget* )),
        SLOT(sl_bioStruct3DGLWidgetRemoved(BioStruct3DGLWidget*)));


    // Menu toolbar
    toolbar = new OrderedToolbar(this);
    toolbar->layout()->setSpacing(10);
    toolbar->layout()->setMargin(0);

    QLabel* pixLabel= new QLabel(this);
    QFont f = pixLabel->font();
    if (f.pixelSize() > HEADER_HEIGHT) {
        f.setPixelSize(HEADER_HEIGHT-8);
    }
    QIcon objIcon(":biostruct3d_view/images/logo.png");
    Q_ASSERT(!objIcon.isNull());
    QPixmap pix = objIcon.pixmap(QSize(32, 32), QIcon::Active);
    pixLabel->setPixmap(pix);
    pixLabel->setFont(f);


    layout->addWidget(pixLabel);
    layout->addWidget(new QLabel(tr("3D Structure Viewer")));
    layout->addStretch();
    layout->addWidget(toolbar);
    setLayout(layout);

    // Build header menu

    QLabel* activeWidgetLabel = new QLabel(this);
    activeWidgetLabel->setText(tr("Active view:"));
    toolbar->addWidget(activeWidgetLabel);

    activeWidgetBox = new QComboBox(this);
    toolbar->addWidget(activeWidgetBox);

    restoreDefaultsAction = new QAction(this);
    restoreDefaultsAction->setText(tr("Restore Default View"));
    restoreDefaultsAction->setIcon(QIcon(":biostruct3d_view/images/restore.png"));
    connect(restoreDefaultsAction, SIGNAL(triggered()), SLOT(sl_restoreDefaults()));

    zoomInAction = new QAction(this);
    zoomInAction->setText(tr("Zoom In"));
    zoomInAction->setIcon(QIcon(":core/images/zoom_in.png"));
    connect(zoomInAction, SIGNAL(triggered()), SLOT(sl_zoomIn()));

    zoomOutAction = new QAction(this);
    zoomOutAction->setIcon(QIcon(":core/images/zoom_out.png"));
    zoomOutAction->setText(tr("Zoom Out"));
    connect(zoomOutAction, SIGNAL(triggered()), SLOT(sl_zoomOut()));

    syncLockAction = new QAction(this);
    syncLockAction->setIcon(QIcon(":biostruct3d_view/images/lock.png"));
    syncLockAction->setText(tr("Synchronize 3D Structure Views"));
    syncLockAction->setCheckable(true);
    connect(syncLockAction, SIGNAL(triggered(bool)), SLOT(sl_toggleSyncLock(bool)));

    displayMenuAction = new QAction(this);
    displayMenuAction->setText(tr("Display"));
    connect(displayMenuAction, SIGNAL(triggered()), SLOT(sl_showDisplayMenu()));
    addToolbarAction(displayMenuAction);

    if (webActionMap.count() != 0) {
        webMenuAction = new QAction(this);
        webMenuAction->setText(tr("Links"));
        connect(webMenuAction, SIGNAL(triggered()), SLOT(sl_showWebMenu()));
        addToolbarAction(webMenuAction);
    }

    addModelAction = new QAction(this);
    addModelAction->setIcon(QIcon(":core/images/add_gobject.png"));
    addModelAction->setText(tr("Add"));
    connect(addModelAction, SIGNAL(triggered()), SLOT(sl_addModel()));

    // TODO: add settings functionality
    // Note: do not use widgets on the toolbar, use actions instead
    //settingsMenuButton = new QToolButton(this);
    //settingsMenuButton->setText(tr("settings"));
    // toolbar->addWidget(settingsMenuButton);

    widgetStateMenuAction = new QAction(this);
    widgetStateMenuAction->setIcon(QIcon(":core/images/adv_widget_menu.png"));
    widgetStateMenuAction->setText(tr("Toggle view"));
    widgetStateMenuAction->setToolTip(tr("Toggle view"));
    connect(widgetStateMenuAction, SIGNAL(triggered()), this, SLOT(sl_showStateMenu()));
    addToolbarAction(widgetStateMenuAction);

    splitter->addActionToLocalToolBar(zoomInAction);
    splitter->addActionToLocalToolBar(zoomOutAction);
    splitter->addActionToLocalToolBar(restoreDefaultsAction);
    splitter->addActionToLocalToolBar(addModelAction);
    splitter->addActionToLocalToolBar(syncLockAction);
}

void SplitterHeaderWidget::sl_showStateMenu()
{
    QPointer<QAbstractButton> widgetStateMenuButtonPtr(qobject_cast<QAbstractButton*>(toolbar->widgetForAction(widgetStateMenuAction)));

    QMenu m;


    foreach(QAction* action, toggleActions) {
        m.addAction(action);
    }

    m.addAction(splitter->getCloseSplitterAction());
    m.exec(QCursor::pos());

    if (!widgetStateMenuButtonPtr.isNull()) { //if not self closed
        widgetStateMenuButtonPtr->setDown(false);
    }
}

void SplitterHeaderWidget::sl_showDisplayMenu()
{
    QPointer<QAbstractButton> displayMenuButtonPtr(qobject_cast<QAbstractButton*>(toolbar->widgetForAction(displayMenuAction)));

    BioStruct3DGLWidget* activeWidget = getActiveWidget();

    QMenu* displayMenu = activeWidget->getDisplayMenu();
    displayMenu->exec(QCursor::pos());

    if (!displayMenuButtonPtr.isNull()) { //if not self closed
        displayMenuButtonPtr->setDown(false);
    }
}

#define ZOOM_DELTA 2.0f

void SplitterHeaderWidget::sl_zoomIn()
{
    BioStruct3DGLWidget* w = getActiveWidget();
    w->zoom(-ZOOM_DELTA);
}

void SplitterHeaderWidget::sl_zoomOut()
{
    BioStruct3DGLWidget* w = getActiveWidget();
    w->zoom(ZOOM_DELTA);
}

// TODO: add settings functionality
//void SplitterHeaderWidget::sl_showSettingsMenu()
//{
//    QPointer<QToolButton> settingsMenuButtonPtr(settingsMenuButton);

//    QMenu m;

//    const QList<QAction* > settingsMenuActions = splitter->getSettingsMenuActions();
//    foreach (QAction* action, settingsMenuActions) {
//        m.addAction(action);
//    }
//    m.exec(QCursor::pos());

//    if (!settingsMenuButtonPtr.isNull()) { //if not self closed
//        settingsMenuButtonPtr->setDown(false);
//    }


//}

void SplitterHeaderWidget::sl_restoreDefaults()
{
    BioStruct3DGLWidget* w = getActiveWidget();
    w->restoreDefaultSettigns();
}

void SplitterHeaderWidget::sl_bioStruct3DGLWidgetAdded( BioStruct3DGLWidget* glWidget )
{
    int index = activeWidgetBox->count() + 1;
    const char* pdbId = glWidget->getBioStruct3D().pdbId.constData();

    QString name = QString("%1: %2").arg(index).arg(pdbId) ;
    activeWidgetBox->addItem(name);
    activeWidgetBox->setCurrentIndex(index - 1);
    glWidget->installEventFilter(this);

    // Add show widget action
    QString actionName( tr("Show %1").arg(pdbId) );
    QAction* action = new QAction(actionName, glWidget);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(triggered(bool)), SLOT(sl_toggleBioStruct3DWidget(bool)));
    toggleActions.append(action);

    enableToolbar();
}

void SplitterHeaderWidget::sl_bioStruct3DGLWidgetRemoved( BioStruct3DGLWidget* glWidget )
{
    glWidget->disconnect(this);
    updateActiveWidgetBox();
    updateToolbar();
    foreach (QAction* action, toggleActions) {
        if (action->parent() == glWidget) {
            toggleActions.removeOne(action);
        }
    }
}

bool SplitterHeaderWidget::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::Wheel) {
        BioStruct3DGLWidget* glWidget = qobject_cast<BioStruct3DGLWidget*>(obj);
        if (glWidget) {
            setActiveView(glWidget);
        }
    }

    return QObject::eventFilter(obj, event);
}



BioStruct3DGLWidget* SplitterHeaderWidget::getActiveWidget()
{
    int idx = activeWidgetBox->currentIndex();
    GLFrameManager* manager = splitter->getGLFrameManager();
    GLFrame* frame = manager->getGLFrames().at(idx);
    frame->makeCurrent();

    return qobject_cast<BioStruct3DGLWidget*>(frame->getGLWidget());
}

void SplitterHeaderWidget::addToolbarAction(QAction* action) {
    if (!toolbar || !action) {
        return;
    }

    toolbar->addAction(action);
    QPointer<QAbstractButton> buttonPtr(qobject_cast<QAbstractButton*>(toolbar->widgetForAction(action)));
    if (!buttonPtr.isNull()) {
        buttonPtr->setObjectName(action->objectName());
        if (!action->icon().isNull()) {
            buttonPtr->setFixedWidth(20);
        }
    }
}


void SplitterHeaderWidget::enableToolbar() {
    activeWidgetBox->setEnabled(true);
    restoreDefaultsAction->setEnabled(true);
    zoomInAction->setEnabled(true);
    zoomOutAction->setEnabled(true);
    displayMenuAction->setEnabled(true);
    widgetStateMenuAction->setEnabled(true);
    syncLockAction->setEnabled(splitter->getGLFrameManager()->getGLFrames().count() > 1);
}


void SplitterHeaderWidget::updateActiveWidgetBox() {
    activeWidgetBox->clear();
    int index = 0;
    QList<BioStruct3DGLWidget*> widgets = splitter->getChildWidgets();
    foreach(BioStruct3DGLWidget* widget, widgets) {
        ++index;
        const char* pdbId = widget->getBioStruct3D().pdbId.constData();
        QString name = QString("%1: %2").arg(index).arg(pdbId);
        activeWidgetBox->addItem(name);
    }


}

void SplitterHeaderWidget::updateToolbar() {
    int numVisibleWidgets = splitter->getNumVisibleWidgets();

    activeWidgetBox->setEnabled(true);
    restoreDefaultsAction->setEnabled(true);
    zoomInAction->setEnabled(true);
    zoomOutAction->setEnabled(true);
    syncLockAction->setEnabled(numVisibleWidgets > 1);
    displayMenuAction->setEnabled(true);
    widgetStateMenuAction->setEnabled(!splitter->getChildWidgets().isEmpty());
}

void SplitterHeaderWidget::sl_toggleBioStruct3DWidget( bool show )
{
    BioStruct3DGLWidget* glWidget = qobject_cast<BioStruct3DGLWidget*> (sender()->parent());
    Q_ASSERT(glWidget != NULL);

    glWidget->setVisible(show);

    int numVisibleWidgets = splitter->getNumVisibleWidgets();

    if ( (show == true && numVisibleWidgets == 1) ||
        (show == false && numVisibleWidgets == 0) ) {
            splitter->adaptSize(numVisibleWidgets);
    }

    updateToolbar();

}

void SplitterHeaderWidget::sl_addModel()
{
    QPointer<QAbstractButton> addModelButtonPtr(qobject_cast<QAbstractButton*>(toolbar->widgetForAction(addModelAction)));

    if (!addModelButtonPtr.isNull()) { //if not self closed
        addModelButtonPtr->setDown(false);
    }

    ProjectTreeControllerModeSettings settings;
    settings.objectTypesToShow.insert(GObjectTypes::BIOSTRUCTURE_3D);
    settings.allowSelectUnloaded = true;
    QList<GObject*> objects = ProjectTreeItemSelectorDialog::selectObjects(settings, this);

    if (!objects.isEmpty()) {
        foreach(GObject* obj, objects) {
            Task* t = new AddModelToSplitterTask(obj, splitter);
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    }

}

void SplitterHeaderWidget::sl_showWebMenu()
{
    QPointer<QAbstractButton> webMenuButtonPtr(qobject_cast<QAbstractButton*>(toolbar->widgetForAction(webMenuAction)));

    QMenu m;
    QString pdbId(getActiveWidget()->getBioStruct3D().pdbId);

    const QList<QAction* >& webMenuActions = webActionMap.keys();
    foreach (QAction* action, webMenuActions) {
        QString wikiName = action->text().split(": ", QString::SkipEmptyParts).last();
        action->setText(pdbId+": "+wikiName);
        m.addAction(action);
    }
    m.exec(QCursor::pos());

    if (!webMenuButtonPtr.isNull()) { //if not self closed
        webMenuButtonPtr->setDown(false);
    }


}

void SplitterHeaderWidget::registerWebUrls()
{

    DBLinksFile linksFile;
    if (!linksFile.load()) {
        return;
    }

    QList<DBLink> links = linksFile.getLinks();
    foreach (const DBLink& link, links) {
        QAction* webAction = new QAction(link.name,this);
        webActionMap.insert(webAction, link.url);
        connect(webAction, SIGNAL(triggered(bool)), this, SLOT(sl_openBioStructUrl()));
    }

}

void SplitterHeaderWidget::sl_openBioStructUrl()
{
    QAction* webAction = qobject_cast<QAction*>(QObject::sender());
    if (webAction == NULL)
        return;
    const QString& urlHeader = webActionMap.value(webAction);
    QString pdbId(getActiveWidget()->getBioStruct3D().pdbId.toLower());
    QString urlName = urlHeader.arg(pdbId);
    QUrl url(urlName);
    QDesktopServices::openUrl(urlName);

}

void SplitterHeaderWidget::setActiveView( BioStruct3DGLWidget* glWidget )
{
    int index = 0;
    QList<GLFrame*> frames = splitter->getGLFrameManager()->getGLFrames();
    foreach (GLFrame* frame, frames) {
        if ( frame->getGLWidget() == glWidget ) {
            frame->makeCurrent();
            activeWidgetBox->setCurrentIndex(index);
            break;
        }
        ++index;
    }

}

void SplitterHeaderWidget::sl_toggleSyncLock( bool toggle )
{
    QList<BioStruct3DGLWidget*> widgets = splitter->getChildWidgets();
    splitter->getGLFrameManager()->setSyncLock(toggle, widgets[0]);
}



///////////////////////////////////////////////////////////////////////////////////////////
// DBLinksFile

#define DB_LINKS_FILE_NAME "biostruct3d_plugin/BioStruct3DLinks.txt"

bool DBLinksFile::load()
{
    QFile file( QString(PATH_PREFIX_DATA)+ ":" + DB_LINKS_FILE_NAME );
    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        ioLog.error(SplitterHeaderWidget::tr("File not found: %1").arg(DB_LINKS_FILE_NAME));
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(line.length()==0 || line.startsWith("#")) {
            continue;
        }
        QStringList fields = line.split("|");
        if (fields.size() != 2) {
            ioLog.error(SplitterHeaderWidget::tr("Illegal entry: %1").arg(line));
            continue;
        }
        DBLink linkInfo(fields[0], fields[1].trimmed());
        links.append(linkInfo);

    }
    file.close();

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
// AddModelToSplitterTask

AddModelToSplitterTask::AddModelToSplitterTask( GObject* o, BioStruct3DSplitter* s ) :
    Task("", TaskFlags_FOSCOE), doc(NULL), obj(o), bObj(NULL), splitter(s)
{
    setTaskName(tr("Add 3d model '%1' to BioStruct3DSplitter").arg(o->getGObjectName()));
}

void AddModelToSplitterTask::prepare()
{
    if (obj->isUnloaded()) {
        doc = obj->getDocument();
        addSubTask(new LoadUnloadedDocumentTask(doc));
    }
}

void AddModelToSplitterTask::run()
{
    if (doc == NULL) {
        bObj = qobject_cast<BioStruct3DObject*> (obj);
    } else {
        Q_ASSERT(doc->isLoaded());
        QList<GObject*> lst = doc->findGObjectByType(GObjectTypes::BIOSTRUCTURE_3D);
        Q_ASSERT(!lst.isEmpty());
        bObj = qobject_cast<BioStruct3DObject*> (lst.first());
    }


}

Task::ReportResult AddModelToSplitterTask::report()
{
    if ( (bObj == NULL) || ( propagateSubtaskError() == true)  ) {
        stateInfo.setError(tr("Model %1 wasn't added").arg(obj->getGObjectName()));
    } else {
        splitter->addModelFromObject(bObj);
    }

    return ReportResult_Finished;
}

} //namespace
