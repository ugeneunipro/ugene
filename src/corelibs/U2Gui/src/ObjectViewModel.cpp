/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ObjectViewModel.h"

#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/OptionsPanel.h>
#include <U2Gui/OptionsPanelWidget.h>

// BUG:535 refactor closing interface.
// Idea: make it QObject and call 'deleteLater' on it

namespace U2 {

const QString GObjectViewState::APP_CLOSING_STATE_NAME("Auto saved");
const GObjectViewFactoryId GObjectViewFactory::SIMPLE_TEXT_FACTORY("SimpleTextView");
const QString GObjectViewMenuType::CONTEXT("gobject-view-menu-type-context");
const QString GObjectViewMenuType::STATIC("object-view-menu-type-static");

void GObjectViewState::setViewName(const QString& newName) {
    // this method is not a real state modification: state caches view name as a reference, but not its internal data
    // it is used only on view renaming
    viewName = newName;
}

void GObjectViewState::setStateName(const QString& newName) {
    if (newName == stateName) {
        return;
    }
    stateName = newName;
    emit si_stateModified(this);
}

void GObjectViewState::setStateData(const QVariantMap& data) {
    stateData = data;
    emit si_stateModified(this);
}

void GObjectViewFactoryRegistry::registerGObjectViewFactory(GObjectViewFactory* f) {
    mapping[f->getId()] = f;
}

void GObjectViewFactoryRegistry::unregisterGObjectViewFactory(GObjectViewFactory* f) {
    assert(GObjectViewUtils::findViewsByFactoryId(f->getId()).isEmpty());
    mapping.remove(f->getId());
}

GObjectViewFactory* GObjectViewFactoryRegistry::getFactoryById(const GObjectViewFactoryId& id) const {
    return mapping.value(id, nullptr);
}

//////////////////////////////////////////////////////////////////////////
/// GObjectViewFactory

GObjectViewFactory::GObjectViewFactory(const GObjectViewFactoryId& _id, const QString& name, QObject* parent)
    : QObject(parent), id(_id), name(name) {
}

const GObjectViewFactoryId& GObjectViewFactory::getId() const {
    return id;
}

const QString& GObjectViewFactory::getName() const {
    return name;
}

bool GObjectViewFactory::supportsSavedStates() const {
    return false;
}

bool GObjectViewFactory::isStateInSelection(const MultiGSelection&, const QVariantMap&) {
    return false;
}

Task* GObjectViewFactory::createViewTask(const QString&, const QVariantMap&) {
    SAFE_POINT(false, "createViewTask from state is not supported by the view", nullptr);
}

//////////////////////////////////////////////////////////////////////////
/// GObjectView
GObjectViewController::GObjectViewController(const GObjectViewFactoryId& _factoryId, const QString& _viewName, QObject* parent)
    : QObject(parent) {
    factoryId = _factoryId;
    viewName = _viewName;
    viewWidget = nullptr;
    closeInterface = nullptr;
    closing = false;

    Project* project = AppContext::getProject();
    SAFE_POINT(project != nullptr, "project is null in GObjectView()", );

    connect(project, SIGNAL(si_documentAdded(Document*)), SLOT(sl_onDocumentAdded(Document*)));
    connect(project, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_onDocumentRemoved(Document*)));
    for (Document* document : qAsConst(project->getDocuments())) {
        sl_onDocumentAdded(document);
    }
}

const GObjectViewFactoryId& GObjectViewController::getFactoryId() const {
    return factoryId;
}

const QString& GObjectViewController::getName() const {
    return viewName;
}

const QList<GObject*>& GObjectViewController::getObjects() const {
    return objects;
}

QVariantMap GObjectViewController::saveState() {
    return QVariantMap();
}

void GObjectViewController::saveWidgetState() {
}

void GObjectViewController::addObjectHandler(GObjectViewObjectHandler* oh) {
    objectHandlers.append(oh);
}

void GObjectViewController::removeObjectHandler(GObjectViewObjectHandler* oh) {
    objectHandlers.removeOne(oh);
}

bool GObjectViewController::onCloseEvent() {
    return true;
}

bool GObjectViewController::canAddObject(GObject* obj) {
    if (objects.contains(obj)) {
        // the 'obj' is already in the view.
        return false;
    }
    for (GObjectViewObjectHandler* objectHandler : qAsConst(objectHandlers)) {
        if (objectHandler->canHandle(this, obj)) {
            return true;
        }
    }
    return false;
}

QString GObjectViewController::addObject(GObject* o) {
    if (closing) {
        return tr("Can't add object: %1 to the closing view").arg(o->getGObjectName());
    }
    if (objects.contains(o)) {
        return tr("Object is already added to view %1!").arg(o->getGObjectName());
    }

    bool canBeAdded = canAddObject(o);
    if (!canBeAdded) {
        for (GObjectViewObjectHandler* objectHandler : qAsConst(objectHandlers)) {
            canBeAdded = objectHandler->canHandle(this, o);
            if (canBeAdded) {
                break;
            }
        }
    }

    if (!canBeAdded) {
        return tr("Can't add object: %1").arg(o->getGObjectName());
    }

    objects << o;
    onObjectAdded(o);
    emit si_objectAdded(this, o);
    return QString();
}

void GObjectViewController::_removeObject(GObject* o) {
    o->disconnect(this);
    int i = objects.removeAll(o);
    assert(i == 1);
    Q_UNUSED(i);
    closing = onObjectRemoved(o) || closing;
    emit si_objectRemoved(this, o);

    if (requiredObjects.contains(o)) {
        closing = true;
    }
}

void GObjectViewController::removeObject(GObject* o) {
    assert(!closing);
    _removeObject(o);
    if (closing) {
        SAFE_POINT(closeInterface != nullptr, "No close interface", );
        closeInterface->closeView();
    }
}

void GObjectViewController::sl_onObjectRemovedFromDocument(GObject* o) {
    if (objects.contains(o)) {
        _removeObject(o);
        if (closing) {
            SAFE_POINT(closeInterface != nullptr, "No close interface", );
            closeInterface->closeView();
        }
    }
}

bool GObjectViewController::onObjectRemoved(GObject* obj) {
    for (GObjectViewObjectHandler* objectHandler : qAsConst(objectHandlers)) {
        objectHandler->onObjectRemoved(this, obj);
    }
    return false;
}

void GObjectViewController::onObjectAdded(GObject* obj) {
    connect(obj, SIGNAL(si_nameChanged(const QString&)), SLOT(sl_onObjectNameChanged(const QString&)));
    for (GObjectViewObjectHandler* objectHandler : qAsConst(objectHandlers)) {
        objectHandler->onObjectAdded(this, obj);
    }
}

void GObjectViewController::onObjectRenamed(GObject*, const QString&) {
    // Do nothing by default.
}

void GObjectViewController::sl_onDocumentAdded(Document* d) {
    connect(d, SIGNAL(si_objectRemoved(GObject*)), SLOT(sl_onObjectRemovedFromDocument(GObject*)));
    connect(d, SIGNAL(si_loadedStateChanged()), SLOT(sl_onDocumentLoadedStateChanged()));
}

void GObjectViewController::sl_onDocumentRemoved(Document* d) {
    if (closing) {
        return;
    }
    d->disconnect(this);
    for (GObject* object : qAsConst(d->getObjects())) {
        if (objects.contains(object)) {
            _removeObject(object);
        }
        if (closing) {
            SAFE_POINT(closeInterface != nullptr, "No close interface", );
            closeInterface->closeView();
            break;
        }
    }
}

void GObjectViewController::sl_onDocumentLoadedStateChanged() {
}

void GObjectViewController::sl_onObjectNameChanged(const QString& oldName) {
    CHECK(AppContext::getProject() != nullptr, );
    auto object = qobject_cast<GObject*>(sender());
    SAFE_POINT(object != nullptr, "Can't locate renamed object!", );
    onObjectRenamed(object, oldName);
}

QWidget* GObjectViewController::getWidget() const {
    SAFE_POINT(viewWidget != nullptr, "getWidget is called before createWidget", nullptr);
    return viewWidget;
}

QWidget* GObjectViewController::createWidget(QWidget* parent) {
    SAFE_POINT(viewWidget == nullptr, "createWidget is called when widget is already created", nullptr);
    viewWidget = createViewWidget(parent);
    return viewWidget;
}

void GObjectViewController::setClosingInterface(GObjectViewCloseInterface* i) {
    closeInterface = i;
}

void GObjectViewController::buildStaticToolbar(QToolBar* tb) {
    emit si_buildStaticToolbar(this, tb);
}

void GObjectViewController::buildMenu(QMenu* m, const QString& type) {
    emit si_buildMenu(this, m, type);
}

// Returns true if view  contains this object
bool GObjectViewController::containsObject(GObject* obj) const {
    return objects.contains(obj);
}

// Returns true if view  contains any objects from the document
bool GObjectViewController::containsDocumentObjects(Document* doc) const {
    const QList<GObject*>& docObjects = doc->getObjects();
    return std::any_of(docObjects.begin(), docObjects.end(), [this](auto o) { return containsObject(o); });
}

void GObjectViewController::onAfterViewWindowInit() {
}

void GObjectViewController::setName(const QString& newName) {
    QString oldName = viewName;
    if (oldName == newName) {
        return;
    }
    viewName = newName;
    emit si_nameChanged(oldName);
}

/** Registers a new actions provider to the view. */
void GObjectViewController::registerActionProvider(GObjectViewActionsProvider* actionsProvider) {
    SAFE_POINT(actionsProvider != nullptr, "GObjectViewActionsProvider is null!", );
    SAFE_POINT(!actionsProviders.contains(actionsProvider), "GObjectViewActionsProvider is already registered!", );
    actionsProviders << actionsProvider;
}

/** Unregisters an actions provider from the view. */
void GObjectViewController::unregisterActionProvider(GObjectViewActionsProvider* actionsProvider) {
    bool isFound = actionsProviders.removeOne(actionsProvider);
    SAFE_POINT(isFound, "unregisterActionProvider can't find a registered provider", );
}

void GObjectViewController::buildActionMenu(QMenu* menu, const QString& menuType) {
    buildActionMenu(menu, QStringList(menuType));
}

void GObjectViewController::buildActionMenu(QMenu* menu, const QList<QString>& menuTypes) {
    QVector<QList<GObjectViewAction*>> actionsByType;
    actionsByType.resize(menuTypes.size());

    for (const GObjectViewActionsProvider* provider : qAsConst(actionsProviders)) {
        QList<GObjectViewAction*> actions = provider->getViewActions(this);
        for (GObjectViewAction* action : qAsConst(actions)) {
            for (int menuTypeIndex = 0; menuTypeIndex < menuTypes.size(); menuTypeIndex++) {
                if (action->isInMenu(menuTypes[menuTypeIndex])) {
                    actionsByType[menuTypeIndex].append(action);
                    break;
                }
            }
        }
    }

    auto actionOrderComparator = [](const GObjectViewAction* a1, const GObjectViewAction* a2) { return a1->getActionOrder() < a2->getActionOrder(); };
    for (const QList<GObjectViewAction*>& actions : qAsConst(actionsByType)) {
        CHECK_CONTINUE(!actions.isEmpty());
        QList<GObjectViewAction*> sortedActions = actions;
        std::sort(sortedActions.begin(), sortedActions.end(), actionOrderComparator);
        if (!menu->isEmpty()) {
            menu->addSeparator();
        }
        for (auto action : qAsConst(sortedActions)) {
            menu->addAction(action);
        }
    }
}

OptionsPanelController* GObjectViewController::getOptionsPanelController() const {
    return optionsPanelController;
}

//////////////////////////////////////////////////////////////////////////
/// GObjectViewWindow

GObjectViewWindow::GObjectViewWindow(GObjectViewController* _viewController, const QString& _viewName, bool _persistent)
    : MWMDIWindow(_viewName), viewController(_viewController), persistent(_persistent) {
    QWidget::setVisible(false);  // Must be made visible after is added to the MDI context.
    viewController->setParent(this);
    viewController->setClosingInterface(this);

    // Root scroll area.
    auto rootLayout = new QHBoxLayout();
    rootLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(rootLayout);

    auto rootScrollArea = new QScrollArea(this);
    rootScrollArea->setFrameStyle(QFrame::NoFrame);
    rootScrollArea->setWidgetResizable(true);
    rootLayout->addWidget(rootScrollArea);

    // Scrollable content widget.
    auto contentWidgetLayout = new QHBoxLayout(rootScrollArea);
    contentWidgetLayout->setContentsMargins(0, 0, 0, 0);
    contentWidgetLayout->setSpacing(0);

    auto contentWidget = new QWidget();
    contentWidget->setObjectName("object_view_window_content_widget");
    contentWidget->setLayout(contentWidgetLayout);
    rootScrollArea->setWidget(contentWidget);

    QWidget* viewWidget;
    if (viewController->optionsPanelController == nullptr) {
        // Set the layout of the whole window
        viewWidget = viewController->createWidget(contentWidget);
        SAFE_POINT(viewWidget != nullptr, "Internal error: Object View widget is not initialized", );
        contentWidgetLayout->addWidget(viewWidget);
    } else {
        auto splitter = new QSplitter(contentWidget);
        splitter->setObjectName("OPTIONS_PANEL_SPLITTER");
        splitter->setOrientation(Qt::Horizontal);
        splitter->setChildrenCollapsible(false);

        viewWidget = viewController->createWidget(splitter);
        SAFE_POINT(viewWidget != nullptr, "Internal error: Object View widget is not initialized", );
        splitter->addWidget(viewWidget);

        OptionsPanelWidget* optionsPanelWidget = viewController->optionsPanelController->createWidget(splitter);
        splitter->addWidget(optionsPanelWidget->getOptionsWidget());
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 0);

        contentWidgetLayout->addWidget(splitter);
        contentWidgetLayout->addWidget(optionsPanelWidget);
    }

    // Set the icon
    setWindowIcon(viewWidget->windowIcon());

    // Notify widget after it was registered as a window.
    QTimer::singleShot(0, viewController, [this] { viewController->onAfterViewWindowInit(); });
}

void GObjectViewWindow::setPersistent(bool v) {
    if (persistent == v) {
        return;
    }
    persistent = v;
    emit si_persistentStateChanged(this);
}

void GObjectViewWindow::closeView() {
    AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(this);
    emit si_windowClosed(this);
}

bool GObjectViewWindow::onCloseEvent() {
    viewController->saveWidgetState();
    return viewController->onCloseEvent();
}

GObjectViewFactory* GObjectViewWindow::getViewFactory() const {
    GObjectViewFactory* viewFactory = AppContext::getObjectViewFactoryRegistry()->getFactoryById(viewController->getFactoryId());
    SAFE_POINT(viewFactory != nullptr, "viewFactory is null!", nullptr)
    return viewFactory;
}

void GObjectViewWindow::setupMDIToolbar(QToolBar* tb) {
    viewController->buildStaticToolbar(tb);
}

void GObjectViewWindow::setupViewMenu(QMenu* m) {
    viewController->buildMenu(m, GObjectViewMenuType::STATIC);
}

//////////////////////////////////////////////////////////////////////////
/// Utils

GObjectViewWindow* GObjectViewUtils::findViewByName(const QString& name) {
    QList<MWMDIWindow*> mdiWindows = AppContext::getMainWindow()->getMDIManager()->getWindows();
    for (MWMDIWindow* mdiWindow : qAsConst(mdiWindows)) {
        if (mdiWindow->windowTitle() == name) {
            auto objectViewWindow = qobject_cast<GObjectViewWindow*>(mdiWindow);
            if (objectViewWindow != nullptr) {
                return objectViewWindow;
            }
        }
    }
    return nullptr;
}

QString GObjectViewUtils::genUniqueViewName(const QString& name) {
    SAFE_POINT(!name.isEmpty(), "genUniqueViewName got empty name!", "");

    QSet<QString> usedNames;  // set of names is derived from active views & saved states
    QList<MWMDIWindow*> windows = AppContext::getMainWindow()->getMDIManager()->getWindows();
    for (const MWMDIWindow* w : qAsConst(windows)) {
        usedNames.insert(w->windowTitle());
    }
    Project* project = AppContext::getProject();
    if (project != nullptr) {
        for (const GObjectViewState* state : qAsConst(project->getGObjectViewStates())) {
            usedNames.insert(state->getViewName());
        }
    }
    return TextUtils::variate(name, " ", usedNames, false, 2);
}

QString GObjectViewUtils::genUniqueStateName(const QString& stateName) {
    SAFE_POINT(!stateName.isEmpty(), "genUniqueStateName got empty state name!", "");

    QSet<QString> usedNames;
    const QList<GObjectViewState*> states = AppContext::getProject()->getGObjectViewStates();
    for (const GObjectViewState* state : qAsConst(states)) {
        usedNames.insert(state->getStateName());
    }
    return TextUtils::variate(stateName, " ", usedNames, false, 2);
}

QString GObjectViewUtils::genUniqueViewName(const Document* doc, const GObject* obj) {
    QString fileName = doc->getURL().fileName();
    QString viewName = obj->getGObjectName() + (fileName.isEmpty() ? "" : " [" + fileName + "]");
    return genUniqueViewName(viewName);
}

QList<GObjectViewState*> GObjectViewUtils::findStatesByViewName(const QString& viewName) {
    QList<GObjectViewState*> result;
    Project* project = AppContext::getProject();
    if (project != nullptr) {
        for (GObjectViewState* state : qAsConst(project->getGObjectViewStates())) {
            if (state->getViewName() == viewName) {
                result << state;
            }
        }
    }
    return result;
}

GObjectViewState* GObjectViewUtils::findStateByName(const QString& viewName, const QString& stateName) {
    Project* project = AppContext::getProject();
    SAFE_POINT(project != nullptr, "project is null!", nullptr);
    const QList<GObjectViewState*>& allStates = project->getGObjectViewStates();
    return findStateInList(viewName, stateName, allStates);
}

GObjectViewState* GObjectViewUtils::findStateInList(const QString& viewName, const QString& stateName, const QList<GObjectViewState*>& states) {
    for (GObjectViewState* state : qAsConst(states)) {
        if (state->getViewName() == viewName && state->getStateName() == stateName) {
            return state;
        }
    }
    return nullptr;
}

QList<GObjectViewWindow*> GObjectViewUtils::getAllActiveViews() {
    QList<MWMDIWindow*> mdiWindows = AppContext::getMainWindow()->getMDIManager()->getWindows();
    QList<GObjectViewWindow*> objectViewWindows;
    for (MWMDIWindow* mdiWindow : qAsConst(mdiWindows)) {
        auto objectViewWindow = qobject_cast<GObjectViewWindow*>(mdiWindow);
        if (objectViewWindow != nullptr) {
            objectViewWindows << objectViewWindow;
        }
    }
    return objectViewWindows;
}

QList<GObjectViewWindow*> GObjectViewUtils::findViewsByFactoryId(const GObjectViewFactoryId& id) {
    QList<GObjectViewWindow*> resultWindowList;
    MainWindow* mainWindow = AppContext::getMainWindow();
    if (mainWindow == nullptr || mainWindow->getMDIManager() == nullptr) {
        return resultWindowList;  // Main window is closed.
    }
    QList<MWMDIWindow*> mdiWindows = mainWindow->getMDIManager()->getWindows();
    for (MWMDIWindow* mdiWindow : qAsConst(mdiWindows)) {
        auto objectViewWindow = qobject_cast<GObjectViewWindow*>(mdiWindow);
        if (objectViewWindow != nullptr && objectViewWindow->getViewFactoryId() == id) {
            resultWindowList << objectViewWindow;
        }
    }
    return resultWindowList;
}

QList<GObjectViewState*> GObjectViewUtils::selectStates(const MultiGSelection& ms, const QList<GObjectViewState*>& states) {
    QList<GObjectViewFactory*> objectViewFactoryList = AppContext::getObjectViewFactoryRegistry()->getAllFactories();

    QList<GObjectViewState*> resultStateList;
    for (GObjectViewFactory* objectViewFactory : qAsConst(objectViewFactoryList)) {
        QList<GObjectViewState*> stateList = selectStates(objectViewFactory, ms, states);
        resultStateList += stateList;
    }
    return resultStateList;
}

QList<GObjectViewState*> GObjectViewUtils::selectStates(GObjectViewFactory* f, const MultiGSelection& ms, const QList<GObjectViewState*>& states) {
    QList<GObjectViewState*> resultStateList;
    for (GObjectViewState* state : qAsConst(states)) {
        if (state->getViewFactoryId() == f->getId()) {
            if (f->isStateInSelection(ms, state->getStateData())) {
                resultStateList << state;
            }
        }
    }
    return resultStateList;
}

QList<GObjectViewWindow*> GObjectViewUtils::findViewsWithObject(GObject* obj) {
    QList<GObjectViewWindow*> resultWindowList;
    QList<GObjectViewWindow*> activeViewWindowList = getAllActiveViews();
    for (GObjectViewWindow* activeViewWindow : qAsConst(activeViewWindowList)) {
        if (activeViewWindow->getObjects().contains(obj)) {
            resultWindowList << activeViewWindow;
        }
    }
    return resultWindowList;
}

QList<GObjectViewWindow*> GObjectViewUtils::findViewsWithAnyOfObjects(const QList<GObject*>& objs) {
    QList<GObjectViewWindow*> resultViewWindowList;
    for (GObject* object : qAsConst(objs)) {
        QList<GObjectViewWindow*> viewWindowWithObjectList = findViewsWithObject(object);
        for (GObjectViewWindow* viewWindow : qAsConst(viewWindowWithObjectList)) {
            if (!resultViewWindowList.contains(viewWindow)) {
                resultViewWindowList += viewWindowWithObjectList;
            }
        }
    }
    return resultViewWindowList;
}

GObjectViewWindow* GObjectViewUtils::getActiveObjectViewWindow() {
    MWMDIWindow* activeWindow = AppContext::getMainWindow()->getMDIManager()->getActiveWindow();
    return qobject_cast<GObjectViewWindow*>(activeWindow);
}

//////////////////////////////////////////////////////////////////////////
// GObjectViewWindowContext

GObjectViewWindowContext::GObjectViewWindowContext(QObject* p, const GObjectViewFactoryId& _id)
    : QObject(p), id(_id) {
}

void GObjectViewWindowContext::init() {
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    connect(mdiManager, SIGNAL(si_windowAdded(MWMDIWindow*)), SLOT(sl_windowAdded(MWMDIWindow*)));
    connect(mdiManager, SIGNAL(si_windowClosing(MWMDIWindow*)), SLOT(sl_windowClosing(MWMDIWindow*)));
    const QList<MWMDIWindow*> windowList = mdiManager->getWindows();
    for (MWMDIWindow* mdiWindow : qAsConst(windowList)) {
        sl_windowAdded(mdiWindow);
    }
}

GObjectViewWindowContext::~GObjectViewWindowContext() {
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    if (mdiManager == nullptr) {  // TODO: disconnect context on view removal and assert (mdi!=NULL) here.
        return;
    }
    const QList<MWMDIWindow*> windowList = mdiManager->getWindows();
    for (MWMDIWindow* window : qAsConst(windowList)) {
        auto objectViewWindow = qobject_cast<GObjectViewWindow*>(window);
        if (objectViewWindow == nullptr || (!id.isEmpty() && objectViewWindow->getViewFactoryId() != id)) {
            continue;
        }
        GObjectViewController* objectView = objectViewWindow->getObjectView();
        disconnectView(objectView);
    }
}

void GObjectViewWindowContext::sl_windowAdded(MWMDIWindow* w) {
    auto objectViewWindow = qobject_cast<GObjectViewWindow*>(w);
    if (objectViewWindow == nullptr || (!id.isEmpty() && objectViewWindow->getViewFactoryId() != id)) {
        return;
    }
    GObjectViewController* objectView = objectViewWindow->getObjectView();
    assert(!viewResources.contains(objectView));

    objectView->addObjectHandler(this);

    initViewContext(objectView);

    connect(objectView, SIGNAL(si_buildMenu(GObjectViewController*, QMenu*, const QString&)), SLOT(sl_buildMenu(GObjectViewController*, QMenu*, const QString&)));
}

void GObjectViewWindowContext::sl_windowClosing(MWMDIWindow* w) {
    auto objectViewWindow = qobject_cast<GObjectViewWindow*>(w);
    if (objectViewWindow == nullptr || (!id.isEmpty() && objectViewWindow->getViewFactoryId() != id)) {
        return;
    }
    GObjectViewController* objectView = objectViewWindow->getObjectView();
    disconnectView(objectView);
}

void GObjectViewWindowContext::sl_buildMenu(GObjectViewController* v, QMenu* m, const QString& type) {
    if (type == GObjectViewMenuType::STATIC) {
        buildStaticMenu(v, m);
    } else if (type == GObjectViewMenuType::CONTEXT) {
        buildContextMenu(v, m);
    } else {
        buildActionMenu(v, m, type);
    }
}

void GObjectViewWindowContext::buildStaticMenu(GObjectViewController* view, QMenu* menu) {
    buildStaticOrContextMenu(view, menu);
}

void GObjectViewWindowContext::buildContextMenu(GObjectViewController* view, QMenu* menu) {
    buildStaticOrContextMenu(view, menu);
}

void GObjectViewWindowContext::buildStaticOrContextMenu(GObjectViewController*, QMenu*) {
    // No extra static/context menu items by default.
}

void GObjectViewWindowContext::buildActionMenu(GObjectViewController* view, QMenu* menu, const QString& menuType) {
    QList<GObjectViewAction*> viewActions = getViewActions(view);
    for (GObjectViewAction* action : viewActions) {
        if (action->isInMenu(menuType)) {
            action->addToMenuWithOrder(menu);
        }
    }
}

void GObjectViewWindowContext::disconnectView(GObjectViewController* v) {
    QList<QObject*> resourceObjectList = viewResources[v];
    for (QObject* resourceObject : qAsConst(resourceObjectList)) {
        resourceObject->deleteLater();  // deliver close signals, save view states first
    }
    viewResources.remove(v);
    v->removeObjectHandler(this);
}

void GObjectViewWindowContext::addViewResource(GObjectViewController* v, QObject* r) {
    assert(v != nullptr && (!id.isEmpty() || v->getFactoryId() == id));

    QList<QObject*> resources = viewResources[v];
    assert(!resources.contains(r));
    resources.append(r);
    viewResources[v] = resources;
}

void GObjectViewWindowContext::addViewAction(GObjectViewAction* a) {
    addViewResource(a->getObjectView(), a);
}

GObjectViewAction* GObjectViewWindowContext::findViewAction(GObjectViewController* v, const QString& actionName) const {
    const QList<GObjectViewAction*> viewActionList = getViewActions(v);
    for (GObjectViewAction* viewAction : qAsConst(viewActionList)) {
        if (viewAction->objectName() == actionName) {
            return viewAction;
        }
    }
    return nullptr;
}

QList<GObjectViewAction*> GObjectViewWindowContext::getViewActions(GObjectViewController* v) const {
    QList<GObjectViewAction*> actions;
    QList<QObject*> resourceObjectList = viewResources[v];
    for (QObject* resourceObject : qAsConst(resourceObjectList)) {
        auto viewAction = qobject_cast<GObjectViewAction*>(resourceObject);
        if (viewAction != nullptr) {
            actions << viewAction;
        }
    }
    return actions;
}

void GObjectViewWindowContext::onObjectRemoved(GObjectViewController* v, GObject* obj) {
    GObjectViewObjectHandler::onObjectRemoved(v, obj);
    const QList<GObjectViewAction*> viewActionList = getViewActions(v);
    for (GObjectViewAction* action : qAsConst(viewActionList)) {
        obj->disconnect(action);
    }
}

//////////////////////////////////////////////////////////////////////////
// GObjectViewAction

GObjectViewAction::GObjectViewAction(QObject* p, GObjectViewController* v, const QString& text, int order)
    : QAction(text, p), viewController(v), actionOrder(order) {
}

GObjectViewController* GObjectViewAction::getObjectView() const {
    return viewController;
}

int GObjectViewAction::getActionOrder() const {
    return actionOrder;
}

bool GObjectViewAction::isInMenu(const QString& menuType) const {
    return menuTypes.contains(menuType);
}

void GObjectViewAction::setMenuTypes(const QList<QString>& newMenuTypes) {
    menuTypes = newMenuTypes;
}

void GObjectViewAction::addToMenuWithOrder(QMenu* menu) {
    QList<QAction*> actionList = menu->actions();
    CHECK(!actionList.contains(this), )
    for (QAction* action : actionList) {
        auto viewAction = qobject_cast<GObjectViewAction*>(action);
        if (viewAction != nullptr && viewAction->getActionOrder() > actionOrder) {
            menu->insertAction(action, this);
            return;
        }
    }
    menu->addAction(this);
}

bool GObjectViewObjectHandler::canHandle(GObjectViewController*, GObject*) {
    return false;
}

void GObjectViewObjectHandler::onObjectAdded(GObjectViewController*, GObject*) {
}

void GObjectViewObjectHandler::onObjectRemoved(GObjectViewController*, GObject*) {
}

}  // namespace U2
