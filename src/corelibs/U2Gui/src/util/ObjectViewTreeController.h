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

#ifndef _U2_OBJECTVIEW_TREE_CONTROLLER_
#define _U2_OBJECTVIEW_TREE_CONTROLLER_

#include <U2Gui/ObjectViewModel.h>
#include <U2Core/SelectionModel.h>

#include <QAction>
#include <QTreeWidget>

namespace U2 {

class OVTItem;
class OVTViewItem;
class OVTStateItem;

class U2GUI_EXPORT ObjectViewTreeController : public QObject  {
    Q_OBJECT
public:
    ObjectViewTreeController(QTreeWidget* w);

    QAction* getAddStateAction() const {return addStateAction;}
    QAction* getRenameStateAction() const {return renameStateAction;}
    QAction* getRemoveStateAction() const {return removeStateAction;}

    const QIcon& getActiveBookmarkIcon() const {return bookmarkActiveIcon;}
    const QIcon& getInactiveBookmarkIcon() const {return bookmarkInactiveIcon;}
    OVTViewItem* findViewItem(const QString& name);

private slots:
    void sl_onMdiWindowAdded(MWMDIWindow*);
    void sl_onMdiWindowClosing(MWMDIWindow*);
    void sl_onMdiWindowActivated(MWMDIWindow*);
    void sl_onViewStateAdded(GObjectViewState*);
    void sl_onStateModified(GObjectViewState*);
    void sl_onViewStateRemoved(GObjectViewState*);
    void sl_onViewPersistentStateChanged(GObjectViewWindow* );
    void sl_onContextMenuRequested(const QPoint &);
    void sl_onTreeCurrentChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void sl_onItemActivated(QTreeWidgetItem*, int);
    void sl_onItemChanged(QTreeWidgetItem*, int);
    void sl_onViewNameChanged(const QString&);
    
    void sl_activateView();
    void sl_addState();
    void sl_removeState();
    void sl_renameState();

private:
    void updateActions();
    void buildTree();
    void connectModel();
    void makeViewPersistent(GObjectViewWindow* w);
    void makeViewTransient(GObjectViewWindow* w);
    
    
    OVTStateItem* findStateItem(GObjectViewState* s);

    OVTItem* currentItem() const;
    OVTViewItem* currentViewItem(bool deriveFromState=false) const;
    OVTStateItem* currentStateItem() const;
    OVTViewItem* activeViewItem() const;


    GObjectViewState* findStateToOpen() const;
    
    void addViewWindow(GObjectViewWindow*);
    OVTStateItem* addState(GObjectViewState*);
    void removeState(GObjectViewState* s);
    

private:
    QTreeWidget* tree;

    QAction* activateViewAction;
    QAction* addStateAction;
    QAction* removeStateAction;
    QAction* renameStateAction;

    QIcon   bookmarkStateIcon;
    QIcon   bookmarkActiveIcon;
    QIcon   bookmarkInactiveIcon;

};

class OVTItem : public QTreeWidgetItem {
public:
    OVTItem(ObjectViewTreeController* c) : controller(c) {}
    bool isRootItem() {return parent() == NULL;}
    virtual bool isViewItem() const {return false;}
    virtual bool isStateItem() const {return false;}
    virtual void updateVisual() = 0;
    ObjectViewTreeController* controller;
};

class OVTViewItem: public OVTItem {
public:
    OVTViewItem(GObjectViewWindow* view, ObjectViewTreeController* c);
    OVTViewItem(const QString& viewName, ObjectViewTreeController* c);
    virtual bool isViewItem() const {return true;}
    virtual void updateVisual();
    virtual void markAsActive(bool _isActive);
    virtual bool isActiveItem(){return isActive;}

    QString viewName;//BUG:416: remove this field?
    GObjectViewWindow* viewWindow;
private:
    bool isActive;
};

class OVTStateItem : public OVTItem {
public:
    OVTStateItem(GObjectViewState* state, OVTViewItem* parent, ObjectViewTreeController* c);
    virtual bool isStateItem() const {return true;}
    virtual void updateVisual();
    GObjectViewState* state;
};



}//namespace

#endif
