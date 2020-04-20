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

#include "ObjectViewTreeController.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <QEvent>
#include <QMenu>

//TODO:
// track factory registry and show only the states with factories available
// delete view state if factory refuses create view


namespace U2 {


ObjectViewTreeController::ObjectViewTreeController(QTreeWidget* w) : QObject(w), tree(w) {
    bookmarkStateIcon = QIcon(":core/images/bookmark_item.png");
    bookmarkActiveIcon = QIcon(":core/images/bookmark.png");
    bookmarkInactiveIcon = QIcon(":core/images/bookmark_inactive.png");

    tree->headerItem()->setHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->setObjectName(ACTION_BOOKMARK_TREE_VIEW);

    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem*)), SLOT(sl_onTreeCurrentChanged(QTreeWidgetItem *, QTreeWidgetItem*)));
    connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));
    connect(tree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(sl_onItemActivated(QTreeWidgetItem*, int)));
    connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(sl_onItemChanged(QTreeWidgetItem*, int)));

    activateViewAction = new QAction(tr("Activate view"), this);
    activateViewAction->setObjectName(ACTION_ACTIVATE_VIEW);
    activateViewAction->setShortcut(QKeySequence(Qt::Key_Space));
    activateViewAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(activateViewAction, SIGNAL(triggered()), SLOT(sl_activateView()));

    addStateAction = new QAction(tr("Add bookmark"), this);
    addStateAction->setObjectName(ACTION_ADD_BOOKMARK);
    addStateAction->setIcon(QIcon(":core/images/bookmark_add.png"));
    connect(addStateAction, SIGNAL(triggered()), SLOT(sl_addState()));

    removeStateAction = new QAction(tr("Remove bookmark"), this);
    removeStateAction->setObjectName(ACTION_REMOVE_BOOKMARK);
    removeStateAction->setIcon(QIcon(":core/images/bookmark_remove.png"));
    removeStateAction->setShortcut(QKeySequence(Qt::Key_Delete));
    removeStateAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(removeStateAction, SIGNAL(triggered()), SLOT(sl_removeState()));

    renameStateAction = new QAction(tr("Rename bookmark"), this);
    renameStateAction->setObjectName(ACTION_RENAME_BOOKMARK);
    renameStateAction->setIcon(QIcon(":core/images/bookmark_edit.png"));
    renameStateAction->setShortcut(QKeySequence(Qt::Key_F2));
    renameStateAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(renameStateAction, SIGNAL(triggered()), SLOT(sl_renameState()));

    tree->addAction(activateViewAction);
    tree->addAction(removeStateAction);
    tree->addAction(renameStateAction);

    connectModel();

    buildTree();
    updateActions();
}

void ObjectViewTreeController::connectModel() {
    Project* p = AppContext::getProject();
    connect(p, SIGNAL(si_objectViewStateAdded(GObjectViewState*)), SLOT(sl_onViewStateAdded(GObjectViewState*)));
    connect(p, SIGNAL(si_objectViewStateRemoved(GObjectViewState*)), SLOT(sl_onViewStateRemoved(GObjectViewState*)));

    MWMDIManager* mdi = AppContext::getMainWindow()->getMDIManager();
    connect(mdi, SIGNAL(si_windowAdded(MWMDIWindow*)), SLOT(sl_onMdiWindowAdded(MWMDIWindow*)));
    connect(mdi, SIGNAL(si_windowClosing(MWMDIWindow*)), SLOT(sl_onMdiWindowClosing(MWMDIWindow*)));

    connect(mdi, SIGNAL(si_windowActivated(MWMDIWindow*)), SLOT(sl_onMdiWindowActivated(MWMDIWindow*)));
}


void ObjectViewTreeController::buildTree() {
    tree->clear();

    const QList<GObjectViewState*>& states= AppContext::getProject()->getGObjectViewStates();
    foreach(GObjectViewState* s, states) {
        addState(s);
    }

    QList<GObjectViewWindow*> views = GObjectViewUtils::getAllActiveViews();
    foreach(GObjectViewWindow* v, views) {
        addViewWindow(v);
    }
}

void ObjectViewTreeController::addViewWindow(GObjectViewWindow* viewWindow) {
    viewWindow->installEventFilter(this);
    connect(viewWindow, SIGNAL(si_persistentStateChanged(GObjectViewWindow*)), SLOT(sl_onViewPersistentStateChanged(GObjectViewWindow*)));
    connect(viewWindow->getObjectView(), SIGNAL(si_nameChanged(const QString&)), SLOT(sl_onViewNameChanged(const QString&)));
    OVTViewItem* vi = findViewItem(viewWindow->getViewName());
    if (vi == NULL) {
        vi = new OVTViewItem(viewWindow, this);
        tree->addTopLevelItem(vi);
        if (tree->currentItem() == NULL) {
            tree->setCurrentItem(vi);
            vi->markAsActive(true);
        }
    } else {
        assert(vi->viewWindow == NULL);
        vi->viewWindow = viewWindow;
        vi->updateVisual();
    }
}

OVTStateItem* ObjectViewTreeController::addState(GObjectViewState* s) {
    OVTViewItem* vi = findViewItem(s->getViewName());
    if (vi == NULL) {
        vi = new OVTViewItem(s->getViewName(), this);
        tree->addTopLevelItem(vi);
    }
    OVTStateItem* si = findStateItem(s);
    SAFE_POINT(si == NULL, QString("State item is already exists: %1 -> %2").arg(s->getViewName()).arg(s->getStateName()), si);
    si = new OVTStateItem(s, vi, this);
    si->setIcon(0, bookmarkStateIcon);
    vi->addChild(si);
    return si;
}

void ObjectViewTreeController::removeState(GObjectViewState* s) {
    OVTStateItem* si = findStateItem(s);
    SAFE_POINT(si != NULL, QString("Failed to find state item to remove: %1 -> %2").arg(s->getViewName()).arg(s->getStateName()),);
    OVTViewItem* vi = static_cast<OVTViewItem*>(si->parent());
    delete si;
    if (vi->childCount() == 0) {
        if (vi->viewWindow==NULL) {
            delete vi;
        } else {
            makeViewTransient(vi->viewWindow);
        }
    }
}


OVTItem* ObjectViewTreeController::currentItem() const {
    QTreeWidgetItem* i = tree->currentItem();
    return static_cast<OVTItem*>(i);
}


OVTViewItem* ObjectViewTreeController::currentViewItem(bool deriveFromState) const {
    OVTItem* i = currentItem();
    OVTViewItem* vi = (i != NULL && i->isViewItem()) ? static_cast<OVTViewItem*>(i) : NULL;
    if (vi == NULL && deriveFromState) {
        OVTStateItem* si = currentStateItem();
        if (si!=NULL) {
            vi = static_cast<OVTViewItem*>(si->parent());
        }
    }
    return vi;
}

OVTStateItem* ObjectViewTreeController::currentStateItem() const {
    OVTItem* i = currentItem();
    return (i != NULL && i->isStateItem()) ? static_cast<OVTStateItem*>(i) : NULL;
}
OVTViewItem* ObjectViewTreeController::activeViewItem() const {
    const GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();
    if (w == NULL) {
        return NULL;
    }
    for(int i = 0; i < tree->topLevelItemCount(); i++) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(tree->topLevelItem(i));
        if (vi->viewWindow == w) {
            return vi;
        }
    }
    return NULL;
}


GObjectViewState* ObjectViewTreeController::findStateToOpen() const {
    OVTStateItem* si = currentStateItem();
    GObjectViewState* state = NULL;
    if (si!=NULL) {
        state = si->state;
    } else {
        OVTViewItem* vi = currentViewItem();
        if (vi!=NULL && vi->viewWindow == NULL) {
            const QList<GObjectViewState*>& allStates = AppContext::getProject()->getGObjectViewStates();
            state = GObjectViewUtils::findStateInList(vi->viewName, GObjectViewState::APP_CLOSING_STATE_NAME, allStates);
        }
    }
    return state;
}

void ObjectViewTreeController::updateActions() {
    OVTStateItem* si = currentStateItem();
    OVTViewItem* vi = currentViewItem(true);

    bool hasActiveView = vi!=NULL && vi->viewWindow!=NULL;

    GObjectViewState* stateToOpen = findStateToOpen();

    bool canAddStates = hasActiveView && vi->viewWindow->getViewFactory()->supportsSavedStates() && vi->isActiveItem();

    activateViewAction->setEnabled(hasActiveView || stateToOpen!=NULL);
    addStateAction->setEnabled(canAddStates);
    removeStateAction->setEnabled(si!=NULL || (vi!=NULL && vi->childCount() > 0));
    renameStateAction->setEnabled(si!=NULL);
}

void ObjectViewTreeController::sl_onMdiWindowAdded(MWMDIWindow* w) {
    GObjectViewWindow* vw = qobject_cast<GObjectViewWindow*>(w);
    if (vw == NULL) {
        return;
    }
    addViewWindow(vw);
    updateActions();
}

void ObjectViewTreeController::sl_onMdiWindowClosing(MWMDIWindow* w) {
    GObjectViewWindow* wv = qobject_cast<GObjectViewWindow*>(w);
    if (wv == NULL) {
        return;
    }
    OVTViewItem* vi = findViewItem(wv->getViewName());
    SAFE_POINT(vi != NULL, QString("Can't find view item on window closing! View name: %1").arg(wv->getViewName()),);
    if (wv->isPersistent()) {
        vi->viewWindow = NULL;
        vi->updateVisual();
    } else {
        assert(vi->childCount() == 0);
        delete vi;
    }
    updateActions();
}

void ObjectViewTreeController::sl_onMdiWindowActivated(MWMDIWindow* w) {
    GObjectViewWindow* wv = qobject_cast<GObjectViewWindow*>(w);

    for(int i =0; i< tree->topLevelItemCount(); i++) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(tree->topLevelItem(i));
        bool isActiveItem = (vi->viewWindow == wv && wv != NULL);
        vi->markAsActive(isActiveItem);
    }
    updateActions();
}

void ObjectViewTreeController::sl_onViewStateAdded(GObjectViewState* s) {
    OVTStateItem * si = addState(s);
    updateActions();
    connect(s, SIGNAL(si_stateModified(GObjectViewState*)), SLOT(sl_onStateModified(GObjectViewState*)));

    if(s->getStateName() != GObjectViewState::APP_CLOSING_STATE_NAME) {
        // Start renaming to allow user to enter the name for bookmark
        tree->setCurrentItem(si);
        sl_renameState();
    }
}

void ObjectViewTreeController::sl_onViewStateRemoved(GObjectViewState* s) {
    removeState(s);
    updateActions();
    s->disconnect(this);
}

void ObjectViewTreeController::sl_onViewPersistentStateChanged(GObjectViewWindow* v) {
    OVTViewItem* vi = findViewItem(v->getViewName());
    vi->updateVisual();
    updateActions();
}

OVTViewItem* ObjectViewTreeController::findViewItem(const QString& name) {
    for(int i =0; i< tree->topLevelItemCount(); i++) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(tree->topLevelItem(i));
        if (vi->viewName == name) {
            return vi;
        }
    }
    return NULL;
}

OVTStateItem* ObjectViewTreeController::findStateItem(GObjectViewState* s) {
    OVTViewItem* vi = findViewItem(s->getViewName());
    if (vi == NULL) {
        return NULL;
    }
    for(int i = 0; i< vi->childCount(); i++) {
        OVTStateItem* si = static_cast<OVTStateItem*>(vi->child(i));
        if (si->state == s) {
            return si;
        }
    }
    return NULL;
}

void ObjectViewTreeController::sl_onContextMenuRequested(const QPoint & pos) {
    Q_UNUSED(pos);
    QMenu popup;
    bool hasItemSelected = tree->currentItem()!=NULL;

    if(hasItemSelected) {
        popup.addAction(activateViewAction);
    }

    popup.addAction(addStateAction);
    popup.addAction(renameStateAction);
    popup.addAction(removeStateAction);

//TODO: emit si_onPopupMenuRequested(*popup);
    if (!popup.isEmpty()) {
        popup.exec(QCursor::pos());
    }
}


void ObjectViewTreeController::sl_onTreeCurrentChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);
    updateActions();
}


void ObjectViewTreeController::sl_activateView() {
    GCOUNTER(cvar, tvar, "Bookmarks::Bookmark Activated");
    OVTViewItem* vi = currentViewItem();
    if (vi!=NULL && vi->viewWindow!=NULL) { //raise existing view, no state change
        AppContext::getMainWindow()->getMDIManager()->activateWindow(vi->viewWindow);
        return;
    }
    //open closed view by state or update state of the active view
    GObjectViewState* state = findStateToOpen();
    if (state==NULL) {
        return;
    }
    GObjectViewWindow* view = GObjectViewUtils::findViewByName(state->getViewName());
    if (view!=NULL) {
        assert(view->isPersistent());
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
        if (state!=NULL) { // state was selected -> apply state
            AppContext::getTaskScheduler()->registerTopLevelTask(view->getObjectView()->updateViewTask(state->getStateName(), state->getStateData()));
        }
    } else {
        GObjectViewFactory* f = AppContext::getObjectViewFactoryRegistry()->getFactoryById(state->getViewFactoryId());
        AppContext::getTaskScheduler()->registerTopLevelTask(f->createViewTask(state->getViewName(), state->getStateData()));
    }
}

void ObjectViewTreeController::sl_onItemActivated(QTreeWidgetItem* i, int col) {
    Q_UNUSED(i); Q_UNUSED(col);
    sl_activateView();
}


void ObjectViewTreeController::makeViewPersistent(GObjectViewWindow* w) {
    assert(!w->isPersistent());
    assert(w->getViewFactory()->supportsSavedStates());
    w->setPersistent(true);
}

void ObjectViewTreeController::sl_addState() {
    GCOUNTER(cvar, tvar, "Bookmarks::Add New Bookmark");
    OVTViewItem* vi = activeViewItem();
    SAFE_POINT(vi != NULL, QString("Can't find view item to add state!"),);
    SAFE_POINT(vi->viewWindow != NULL, QString("View window is NULL: %1").arg(vi->viewName),);
    if (!vi->viewWindow->isPersistent()) {
        makeViewPersistent(vi->viewWindow);
    }
    assert(vi->viewWindow->isPersistent());

    QString stateName = GObjectViewUtils::genUniqueStateName(tr("New bookmark")); //todo: avoid localization here?
    QVariantMap state = vi->viewWindow->getObjectView()->saveState();
    GObjectViewState* s = new GObjectViewState(vi->viewWindow->getViewFactoryId(), vi->viewWindow->getViewName(), stateName, state);
    AppContext::getProject()->addGObjectViewState(s);

    vi->setExpanded(true);
}

void ObjectViewTreeController::makeViewTransient(GObjectViewWindow* w) {
    assert(w->isPersistent());
    w->setPersistent(false);
}

void ObjectViewTreeController::sl_removeState() {
    GCOUNTER(cvar, tvar, "Bookmarks::Remove Bookmark");
    OVTStateItem* si = currentStateItem();
    Project* p = AppContext::getProject();
    if (si!=NULL) {
        assert(si->state!=NULL);
        p->removeGObjectViewState(si->state);
    } else {
        OVTViewItem* vi = currentViewItem();
        SAFE_POINT(vi != NULL, QString("Can't find view item to remove its state!"),);
        int childs = vi->childCount();
        assert(childs > 0);
        for (int i=0; i< childs; i++) {
            OVTStateItem* si2 = static_cast<OVTStateItem*>(vi->child(0));
            p->removeGObjectViewState(si2->state);
        }
    }
}

void ObjectViewTreeController::sl_renameState() {
    GCOUNTER(cvar, tvar, "Bookmarks::Rename Bookmark");
    OVTStateItem* si = currentStateItem();
    SAFE_POINT(si != NULL, QString("Can't find state item to rename!"),);

    si->setFlags(si->flags() | Qt::ItemIsEditable);
    tree->editItem(si);
    //tree->disconnect(this, SLOT(sl_onItemChanged(QTreeWidgetItem*, int)));
    si->setFlags(si->flags() ^ Qt::ItemIsEditable);
}

void ObjectViewTreeController::sl_onItemChanged(QTreeWidgetItem* i, int c) {
    assert(c == 0); Q_UNUSED(c);
    OVTItem* oi = static_cast<OVTItem*>(i);
    if (oi->isViewItem()) {
        OVTViewItem* vi = static_cast<OVTViewItem*>(oi);
        assert(vi->text(0).endsWith(vi->viewName)); Q_UNUSED(vi);
        return;
    }
    assert(oi->isStateItem());
    OVTStateItem* si = static_cast<OVTStateItem*>(oi);
    QString newName = si->text(0);
    GObjectViewState* state = GObjectViewUtils::findStateByName(si->state->getViewName(), newName);
    if (state == si->state) {
        return;
    }
    if (state != NULL) {
        //todo: show error!
        return;
    }
    if (newName.isEmpty()) {
        //todo: show error
        return;
    }
    si->state->setStateName(newName);
}

void ObjectViewTreeController::sl_onStateModified(GObjectViewState* s) {
    OVTStateItem* si = findStateItem(s);
    SAFE_POINT(si != NULL, QString("Can't find state item to update: %1 -> %2").arg(s->getViewName()).arg(s->getStateName()),);
    si->updateVisual();
}

void ObjectViewTreeController::sl_onViewNameChanged(const QString& oldName) {
    OVTViewItem* vi = findViewItem(oldName);
    SAFE_POINT(vi, QString("Can't find view item to rename: %1").arg(oldName),);
    vi->updateVisual();
}

//////////////////////////////////////////////////////////////////////////
/// tree items

OVTViewItem::OVTViewItem(GObjectViewWindow* v, ObjectViewTreeController* c)
: OVTItem (c), viewName(v->getViewName()), viewWindow(v), isActive(false)
{
    updateVisual();
}

OVTViewItem::OVTViewItem(const QString& _viewName, ObjectViewTreeController* c)
: OVTItem (c), viewName(_viewName), viewWindow(NULL), isActive(false)
{
    updateVisual();
}

void OVTViewItem::updateVisual() {
    setIcon(0, viewWindow == NULL ? controller->getInactiveBookmarkIcon() : controller->getActiveBookmarkIcon());
    viewName = viewWindow == NULL ? viewName : viewWindow->getViewName();

    QString text = viewName;
    setText(0, text);
}
void OVTViewItem::markAsActive(bool _isActive) {
    isActive = _isActive;
    
    QFont curFont = font(0);
    curFont.setBold(isActive);
    setFont(0, curFont);
}

OVTStateItem::OVTStateItem(GObjectViewState* _state, OVTViewItem* parent, ObjectViewTreeController* c)
: OVTItem(c), state(_state)
{
    updateVisual();
    parent->addChild(this);
}

void OVTStateItem::updateVisual() {
    setText(0, state->getStateName());
}


}//namespace
