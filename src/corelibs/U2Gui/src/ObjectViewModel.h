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

#ifndef _U2_OBJECT_VIEW_MODEL_H_
#define _U2_OBJECT_VIEW_MODEL_H_

#include <U2Core/SelectionModel.h>
#include "MainWindow.h"

#include <QMap>
#include <QVariantMap>
#include <QAction>
#include <QMenu>

namespace U2 {

class GObjectViewFactory;
class GObjectView;
class Task;
class GObject;
class Document;
class MultiGSelection;
class GObjectViewObjectHandler;

class U2GUI_EXPORT GObjectViewFactoryRegistry: public QObject {
public:
    GObjectViewFactoryRegistry(QObject* p = NULL) : QObject(p){}

    void registerGObjectViewFactory(GObjectViewFactory* f);

    void unregisterGObjectViewFactory(GObjectViewFactory* f);

    GObjectViewFactory* getFactoryById(GObjectViewFactoryId id) const;

    QList<GObjectViewFactory*> getAllFactories() const {return mapping.values();}

private:
    QMap<GObjectViewFactoryId, GObjectViewFactory*> mapping;
};

class GObjectReference;
class U2GUI_EXPORT GObjectViewState : public QObject {
    Q_OBJECT
public:
    GObjectViewState(GObjectViewFactoryId _factoryId, const QString& _viewName, const QString& _stateName,
        const QVariantMap& _stateData, QObject* p = NULL)
        : QObject(p), factoryId(_factoryId), viewName(_viewName), stateName(_stateName), stateData(_stateData){}

    GObjectViewFactoryId getViewFactoryId() const {return factoryId;}

    const QString& getViewName() const {return viewName;}

    void setViewName(const QString& newName);

    const QString& getStateName() const {return stateName;}

    void setStateName(const QString& newName);

    const QVariantMap& getStateData() const {return stateData;}

    void setStateData(const QVariantMap& data);

    static const QString APP_CLOSING_STATE_NAME;

signals:
    void si_stateModified(GObjectViewState* thiz);

private:
    GObjectViewFactoryId factoryId;
    QString viewName;
    QString stateName;
    QVariantMap stateData;
};

class U2GUI_EXPORT GObjectViewFactory : public QObject {
public:
    static const GObjectViewFactoryId SIMPLE_TEXT_FACTORY;

    GObjectViewFactory(GObjectViewFactoryId _id, const QString& _name, QObject* p = NULL)
        : QObject(p), id(_id), name(_name){}

    GObjectViewFactoryId getId() const {return id;}

    QString getName() const {return name;}

    virtual bool canCreateView(const MultiGSelection& multiSelection) = 0;

    virtual bool supportsSavedStates() const {return false;}

    virtual bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
        Q_UNUSED(multiSelection); Q_UNUSED(stateData); return false;
    }

    virtual Task* createViewTask(const MultiGSelection& multiSelection, bool single = false) = 0;

    virtual Task* createViewTask(const QString& viewName, const QVariantMap& stateData) {
        Q_UNUSED(viewName); Q_UNUSED(stateData); assert(0); return NULL;
    }

protected:
    GObjectViewFactoryId id;
    QString name;
};

class GObjectViewCloseInterface;
class OptionsPanel;

class U2GUI_EXPORT GObjectView : public QObject {
    Q_OBJECT
public:
    GObjectView(GObjectViewFactoryId factoryId, const QString& viewName, QObject* p=NULL);

    GObjectViewFactoryId getFactoryId() const {return factoryId;}

    const QString& getName() const {return viewName;}

    void setName(const QString& name);

    QWidget*        getWidget();

    /** Returns the options panel object, or 0 if it is not defined */
    virtual OptionsPanel*   getOptionsPanel();

    const QList<GObject*>& getObjects() const {return objects;}

    // Returns true if view  contains this object
    bool containsObject(GObject* obj) const;

    // Returns true if view  contains any objects from the document
    bool containsDocumentObjects(Document* doc) const;

    virtual QVariantMap saveState() {return QVariantMap();}

    virtual Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) {assert(0); Q_UNUSED(stateName); Q_UNUSED(stateData); return NULL;}

    virtual bool checkAddToView(const MultiGSelection& multiSelection) {Q_UNUSED(multiSelection); return false;}

    virtual Task* addToViewTask(const MultiGSelection& multiSelection) {assert(0); Q_UNUSED(multiSelection); return NULL;}

    void setClosingInterface(GObjectViewCloseInterface* i);

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildStaticMenu(QMenu* n);

    virtual bool canAddObject(GObject* obj);

    // returns error string if object can't be added by some reason
    virtual QString addObject(GObject* o);

    virtual void removeObject(GObject* o);

    virtual void saveWidgetState() {}

    virtual void addObjectHandler(GObjectViewObjectHandler* oh) {objectHandlers.append(oh);}
    virtual void removeObjectHandler(GObjectViewObjectHandler* oh) {objectHandlers.removeOne(oh);}

    virtual bool onCloseEvent() { return true; }

protected:
    /** if 'true' is returned -> view will be closed */
    virtual bool onObjectRemoved(GObject* o);
    virtual void onObjectAdded(GObject* o);
    virtual void onObjectRenamed(GObject* obj, const QString& oldName) {Q_UNUSED(obj); Q_UNUSED(oldName);}

protected:
    virtual void _removeObject(GObject* o);
    virtual QWidget* createWidget() = 0;

signals:
    void si_buildPopupMenu(GObjectView* v, QMenu* m);
    void si_buildStaticMenu(GObjectView* v, QMenu* m);
    void si_buildStaticToolbar(GObjectView* v, QToolBar* tb);
    void si_objectAdded(GObjectView* v, GObject* obj);
    void si_objectRemoved(GObjectView* v, GObject* obj);
    void si_nameChanged(const QString& oldName);

protected slots:
    void sl_onObjectRemovedFromDocument(GObject* o);
    void sl_onObjectNameChanged(const QString&);

    virtual void sl_onDocumentAdded(Document*);
    virtual void sl_onDocumentRemoved(Document*);
    virtual void sl_onDocumentLoadedStateChanged() {}

protected:
    GObjectViewFactoryId                factoryId;
    QString                             viewName;
    QWidget*                            widget;
    QList<GObject*>                     objects;
    QList<GObject*>                     requiredObjects;
    GObjectViewCloseInterface*          closeInterface;
    bool                                closing;
    QList<GObjectViewObjectHandler*>    objectHandlers;
    OptionsPanel*                       optionsPanel;
};

class U2GUI_EXPORT GObjectViewCloseInterface {
public:
    virtual ~GObjectViewCloseInterface(){}
    virtual void closeView() = 0;
};


class U2GUI_EXPORT GObjectViewWindow : public MWMDIWindow, public GObjectViewCloseInterface {
    Q_OBJECT

public:
    GObjectViewWindow(GObjectView* view, const QString& _viewName, bool _persistent = false);

    const QList<GObject*>& getObjects() const  {return view->getObjects();}

    GObjectViewFactoryId getViewFactoryId() const {return view->getFactoryId();}

    bool isObjectView() const {return true;}

    bool isPersistent() const {return persistent;}

    void setPersistent(bool v);

    QString getViewName() const {return view->getName();}

    GObjectView* getObjectView() const {return view;}

    virtual void closeView();

    GObjectViewFactory* getViewFactory() const;

    virtual void setupMDIToolbar(QToolBar* tb);

    virtual void setupViewMenu(QMenu* n);

protected:
    virtual bool onCloseEvent();
signals:
    void si_persistentStateChanged(GObjectViewWindow* thiz);
    void si_windowClosed(GObjectViewWindow* viewWindow);

private:
    void updateDocumentConnections(Document* o, bool added);

protected:
    GObjectView*        view;
    bool                persistent;
};

//////////////////////////////////////////////////////////////////////////
// Customization


class U2GUI_EXPORT GObjectViewUtils {
public:
    static QList<GObjectViewState*> findStatesByViewName(const QString& viewName);

    static GObjectViewState* findStateByName(const QString& viewName, const QString& stateName);

    static GObjectViewState* findStateInList(const QString& viewName, const QString& stateName, const QList<GObjectViewState*>& states);

    static QList<GObjectViewWindow*> getAllActiveViews();

    static QList<GObjectViewWindow*> findViewsByFactoryId(GObjectViewFactoryId id);

    static QList<GObjectViewWindow*> findViewsWithObject(GObject* obj);

    static QList<GObjectViewWindow*> findViewsWithAnyOfObjects(const QList<GObject*>& objs);

    static GObjectViewWindow* findViewByName(const QString& name);

    static QString genUniqueViewName(const Document* doc, const GObject* obj);

    static QString genUniqueViewName(const QString& viewName);

    static QString genUniqueStateName(const QString& stateName);

    static QList<GObjectViewState*> selectStates(const MultiGSelection& ms, const QList<GObjectViewState*>& states);

    static QList<GObjectViewState*> selectStates(GObjectViewFactory* f, const MultiGSelection& ms, const QList<GObjectViewState*>& states);

    // Returns active object view window.
    // Returns NULL if active window is not object view window
    static GObjectViewWindow* getActiveObjectViewWindow();
};

#define GObjectViewAction_DefaultOrder 100
class U2GUI_EXPORT GObjectViewAction : public QAction {
    Q_OBJECT
public:
    GObjectViewAction(QObject* p, GObjectView* v, const QString& text, int order = GObjectViewAction_DefaultOrder);

    GObjectView* getObjectView() const;
    int getActionOrder() const;
    void addToMenuWithOrder(QMenu* menu);

private:
    GObjectView* view;

    // Action order can be used to set-up relative action position in GUI elements
    int actionOrder;
};


class U2GUI_EXPORT GObjectViewObjectHandler {
public:
    virtual ~GObjectViewObjectHandler(){}
    virtual bool canHandle(GObjectView* v, GObject* obj) {Q_UNUSED(v); Q_UNUSED(obj); return false;}

    virtual void onObjectAdded(GObjectView* v, GObject* obj) {Q_UNUSED(v); Q_UNUSED(obj);}
    virtual void onObjectRemoved(GObjectView* v, GObject* obj) {Q_UNUSED(v); Q_UNUSED(obj);}
};

class U2GUI_EXPORT GObjectViewWindowContext : public QObject, public GObjectViewObjectHandler {
    Q_OBJECT

public:
    GObjectViewWindowContext(QObject* p, const GObjectViewFactoryId& id);
    virtual ~GObjectViewWindowContext();
    virtual void init();

    QList<GObjectViewAction*> getViewActions(GObjectView* view) const;

    // GObjectViewObjectHandler
    virtual void onObjectRemoved(GObjectView* v, GObject* obj);

protected:
    /// init context associated with 'view'
    virtual void initViewContext(GObjectView* view) = 0;
    void addViewResource(GObjectView* view, QObject* r);
    void addViewAction(GObjectViewAction* a);
    GObjectViewAction * findViewAction(GObjectView* v, const QString &actionName) const;

protected slots:
    virtual void sl_windowAdded(MWMDIWindow*);
    virtual void sl_windowClosing(MWMDIWindow*);
    virtual void sl_buildContextMenu(GObjectView* v, QMenu* m);
    virtual void sl_buildStaticMenu(GObjectView* v, QMenu* m);

protected:
    virtual void buildMenu(GObjectView* v, QMenu* m){Q_UNUSED(v); Q_UNUSED(m);}
    virtual void disconnectView(GObjectView* v);


    QMap<GObjectView*, QList<QObject*> > viewResources;
    GObjectViewFactoryId id;
private:
    bool initialzed;
};

}//namespace

#endif
