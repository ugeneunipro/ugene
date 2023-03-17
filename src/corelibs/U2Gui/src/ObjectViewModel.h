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

#pragma once

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QVariantMap>

#include <U2Core/SelectionModel.h>

#include "MainWindow.h"

namespace U2 {

class Document;
class GObject;
class GObjectViewController;
class GObjectViewAction;
class GObjectViewActionsProvider;
class GObjectViewFactory;
class GObjectViewObjectHandler;
class MultiGSelection;
class Task;

class U2GUI_EXPORT GObjectViewFactoryRegistry : public QObject {
public:
    GObjectViewFactoryRegistry(QObject* parent = nullptr)
        : QObject(parent) {
    }

    void registerGObjectViewFactory(GObjectViewFactory* f);

    void unregisterGObjectViewFactory(GObjectViewFactory* f);

    GObjectViewFactory* getFactoryById(const GObjectViewFactoryId& id) const;

    QList<GObjectViewFactory*> getAllFactories() const {
        return mapping.values();
    }

private:
    QMap<GObjectViewFactoryId, GObjectViewFactory*> mapping;
};

class GObjectReference;
class U2GUI_EXPORT GObjectViewState : public QObject {
    Q_OBJECT
public:
    GObjectViewState(const GObjectViewFactoryId& _factoryId, const QString& viewName, const QString& stateName, const QVariantMap& stateData, QObject* parent = nullptr)
        : QObject(parent), factoryId(_factoryId), viewName(viewName), stateName(stateName), stateData(stateData) {
    }

    GObjectViewFactoryId getViewFactoryId() const {
        return factoryId;
    }

    const QString& getViewName() const {
        return viewName;
    }

    void setViewName(const QString& newName);

    const QString& getStateName() const {
        return stateName;
    }

    void setStateName(const QString& newName);

    const QVariantMap& getStateData() const {
        return stateData;
    }

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

    GObjectViewFactory(const GObjectViewFactoryId& id, const QString& name, QObject* parent = nullptr);

    const GObjectViewFactoryId& getId() const;

    const QString& getName() const;

    virtual bool canCreateView(const MultiGSelection& multiSelection) = 0;

    virtual bool supportsSavedStates() const;

    virtual bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData);

    virtual Task* createViewTask(const MultiGSelection& multiSelection, bool single = false) = 0;

    virtual Task* createViewTask(const QString& viewName, const QVariantMap& stateData);

protected:
    GObjectViewFactoryId id;
    QString name;
};

class GObjectViewCloseInterface;
class OptionsPanelController;
class GObjectViewWindow;

class U2GUI_EXPORT GObjectViewController : public QObject {
    friend class GObjectViewWindow;
    Q_OBJECT
public:
    GObjectViewController(const GObjectViewFactoryId& factoryId, const QString& viewName, QObject* parent = nullptr);

    const GObjectViewFactoryId& getFactoryId() const;

    const QString& getName() const;

    void setName(const QString& name);

    QWidget* getWidget() const;

    const QList<GObject*>& getObjects() const;

    // Returns true if view  contains this object
    bool containsObject(GObject* obj) const;

    // Returns true if view  contains any objects from the document
    bool containsDocumentObjects(Document* doc) const;

    virtual QVariantMap saveState();

    virtual Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) = 0;

    void setClosingInterface(GObjectViewCloseInterface* i);

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildMenu(QMenu* menu, const QString& type);

    virtual bool canAddObject(GObject* obj);

    // returns error string if object can't be added by some reason
    virtual QString addObject(GObject* o);

    virtual void removeObject(GObject* o);

    virtual void saveWidgetState();

    virtual void addObjectHandler(GObjectViewObjectHandler* oh);

    virtual void removeObjectHandler(GObjectViewObjectHandler* oh);

    virtual bool onCloseEvent();

    /** Registers a new actions provider to the view. */
    void registerActionProvider(GObjectViewActionsProvider* actionsProvider);

    /** Unregisters an actions provider from the view. */
    void unregisterActionProvider(GObjectViewActionsProvider* actionsProvider);

    OptionsPanelController* getOptionsPanelController() const;

protected:
    /** if 'true' is returned -> view will be closed */
    virtual bool onObjectRemoved(GObject* o);

    virtual void onObjectAdded(GObject* o);

    /** Handles object rename event. Does nothing by default. */
    virtual void onObjectRenamed(GObject* obj, const QString& oldName);

    virtual void _removeObject(GObject* o);

    /** Creates a new view widget. Called only once. */
    virtual QWidget* createViewWidget(QWidget* parent) = 0;

    /**
     * Adds all actions with the given menu types into the menu.
     * The action are queried from the current list of actions provider.
     * Different menu sections (menu types) are divided by menu separator items.
     */
    void buildActionMenu(QMenu* menu, const QList<QString>& menuTypes);

    /** Calls 'buildActionMenu' with a menu type wrapped with the QList<>. */
    void buildActionMenu(QMenu* menu, const QString& menuType);

    /**
     * Called by GObjectViewWindow after view widget & layout initialization is finished.
     * Default implementation does nothing.
     */
    virtual void onAfterViewWindowInit();

private:
    QWidget* createWidget(QWidget* parent);

signals:
    /**
     * Emitted when menu of the given type is about to be shown.
     * A client should add related menu items to the menu.
     */
    void si_buildMenu(GObjectViewController* v, QMenu* m, const QString& type);

    void si_buildStaticToolbar(GObjectViewController* v, QToolBar* tb);
    void si_objectAdded(GObjectViewController* v, GObject* obj);
    void si_objectRemoved(GObjectViewController* v, GObject* obj);
    void si_nameChanged(const QString& oldName);

protected slots:
    void sl_onObjectRemovedFromDocument(GObject* o);
    void sl_onObjectNameChanged(const QString&);

    virtual void sl_onDocumentAdded(Document*);
    virtual void sl_onDocumentRemoved(Document*);
    virtual void sl_onDocumentLoadedStateChanged();

protected:
    GObjectViewFactoryId factoryId;
    QString viewName;
    QWidget* viewWidget;
    QList<GObject*> objects;
    QList<GObject*> requiredObjects;
    GObjectViewCloseInterface* closeInterface;
    bool closing;
    QList<GObjectViewObjectHandler*> objectHandlers;
    QList<GObjectViewActionsProvider*> actionsProviders;
    OptionsPanelController* optionsPanelController = nullptr;
};

class U2GUI_EXPORT GObjectViewActionsProvider {
public:
    /** Returns list of actions available to the view. */
    virtual QList<GObjectViewAction*> getViewActions(GObjectViewController* view) const = 0;
};

/** Constants for known GObject view menu types. */
class U2GUI_EXPORT GObjectViewMenuType {
public:
    /** A constant for building a contextual (popup) menu. See buildMenu for details. */
    const static QString CONTEXT;

    /** A constant for building a static (toolbar-level) menu. See buildMenu for details. */
    const static QString STATIC;
};

class U2GUI_EXPORT GObjectViewCloseInterface {
public:
    virtual ~GObjectViewCloseInterface() {
    }
    virtual void closeView() = 0;
};

class U2GUI_EXPORT GObjectViewWindow : public MWMDIWindow, public GObjectViewCloseInterface {
    Q_OBJECT

public:
    GObjectViewWindow(GObjectViewController* viewController, const QString& _viewName, bool _persistent = false);

    const QList<GObject*>& getObjects() const {
        return viewController->getObjects();
    }

    GObjectViewFactoryId getViewFactoryId() const {
        return viewController->getFactoryId();
    }

    bool isPersistent() const {
        return persistent;
    }

    void setPersistent(bool v);

    QString getViewName() const {
        return viewController->getName();
    }

    GObjectViewController* getObjectView() const {
        return viewController;
    }

    virtual void closeView();

    GObjectViewFactory* getViewFactory() const;

    virtual void setupMDIToolbar(QToolBar* tb);

    virtual void setupViewMenu(QMenu* n);

protected:
    virtual bool onCloseEvent();

signals:
    void si_persistentStateChanged(GObjectViewWindow* thiz);
    void si_windowClosed(GObjectViewWindow* viewWindow);

protected:
    GObjectViewController* viewController;
    bool persistent;
};

//////////////////////////////////////////////////////////////////////////
// Customization

class U2GUI_EXPORT GObjectViewUtils {
public:
    static QList<GObjectViewState*> findStatesByViewName(const QString& viewName);

    static GObjectViewState* findStateByName(const QString& viewName, const QString& stateName);

    static GObjectViewState* findStateInList(const QString& viewName, const QString& stateName, const QList<GObjectViewState*>& states);

    static QList<GObjectViewWindow*> getAllActiveViews();

    static QList<GObjectViewWindow*> findViewsByFactoryId(const GObjectViewFactoryId& id);

    static QList<GObjectViewWindow*> findViewsWithObject(GObject* obj);

    static QList<GObjectViewWindow*> findViewsWithAnyOfObjects(const QList<GObject*>& objs);

    static GObjectViewWindow* findViewByName(const QString& name);

    static QString genUniqueViewName(const Document* doc, const GObject* obj);

    static QString genUniqueViewName(const QString& viewName);

    static QString genUniqueStateName(const QString& stateName);

    static QList<GObjectViewState*> selectStates(const MultiGSelection& ms, const QList<GObjectViewState*>& states);

    static QList<GObjectViewState*> selectStates(GObjectViewFactory* f, const MultiGSelection& ms, const QList<GObjectViewState*>& states);

    // Returns active object view window.
    // Returns nullptr if active window is not object view window
    static GObjectViewWindow* getActiveObjectViewWindow();
};

#define GObjectViewAction_DefaultOrder 100
class U2GUI_EXPORT GObjectViewAction : public QAction {
    Q_OBJECT
public:
    GObjectViewAction(QObject* p, GObjectViewController* v, const QString& text, int order = GObjectViewAction_DefaultOrder);

    GObjectViewController* getObjectView() const;

    int getActionOrder() const;

    void addToMenuWithOrder(QMenu* menu);

    /** Returns true if the action is present in the menu of the given type. */
    bool isInMenu(const QString& menuType) const;

    /** Sets menu type ids this action should be present in. */
    void setMenuTypes(const QList<QString>& menuTypes);

private:
    GObjectViewController* viewController;

    // Action order can be used to set-up relative action position in GUI elements
    int actionOrder;

    /** Set of view action menu types this action belongs to. */
    QList<QString> menuTypes;
};

/**
 * Abstraction of the object handler for a view.
 * If a view has a handler that can 'handle' an object, this object can be added to the view.
 */
class U2GUI_EXPORT GObjectViewObjectHandler {
public:
    virtual ~GObjectViewObjectHandler() = default;

    /**
     * Checks if the handler can 'handle' the object.
     * If there is at least one handler that returns 'yes', the object can be added to the view.
     */
    virtual bool canHandle(GObjectViewController* view, GObject* obj);

    /** Called when an object is added to the view. */
    virtual void onObjectAdded(GObjectViewController* view, GObject* obj);

    /** Called when an object is removed from the view. */
    virtual void onObjectRemoved(GObjectViewController* view, GObject* obj);
};

class U2GUI_EXPORT GObjectViewWindowContext : public QObject, public GObjectViewObjectHandler, public GObjectViewActionsProvider {
    Q_OBJECT

public:
    GObjectViewWindowContext(QObject* p, const GObjectViewFactoryId& id);
    virtual ~GObjectViewWindowContext();
    virtual void init();

    QList<GObjectViewAction*> getViewActions(GObjectViewController* view) const override;

    void onObjectRemoved(GObjectViewController* v, GObject* obj) override;

protected:
    /// init context associated with 'view'
    virtual void initViewContext(GObjectViewController* view) = 0;
    void addViewResource(GObjectViewController* view, QObject* r);
    void addViewAction(GObjectViewAction* a);
    GObjectViewAction* findViewAction(GObjectViewController* v, const QString& actionName) const;

protected slots:
    virtual void sl_windowAdded(MWMDIWindow*);
    virtual void sl_windowClosing(MWMDIWindow*);

    virtual void sl_buildMenu(GObjectViewController* view, QMenu* menu, const QString& type);

protected:
    /**
     * Populates static global application menu with actions added by the class instance.
     * This method is called when the view builds GObjectViewMenuType::Static menu.
     */
    virtual void buildStaticMenu(GObjectViewController* view, QMenu* menu);

    /**
     * Populates context popup menu with actions added by the class instance.
     * This method is called when the view builds GObjectViewMenuType::Context menu.
     */
    virtual void buildContextMenu(GObjectViewController* view, QMenu* menu);

    /**
     * Populates menu with type specific actions.
     * This method is called during construction of the type specific popup menu in the view like 'Align' button menu in MSA Editor.
     * The method is never called for the generic 'GObjectViewMenuType::Context' or 'GObjectViewMenuType::Static' menu types:
     *  use 'buildContextMenu' or 'buildStaticMenu' for that types.
     */
    virtual void buildActionMenu(GObjectViewController* view, QMenu* menu, const QString& menuType);

    /**
     * Builds both 'GObjectViewMenuType::Static' and 'GObjectViewMenuType::Context' menus.
     *
     * This convenience method is called by default by the base impls of buildStaticMenu() and buildContextMenu() and
     * exists because most of the UGENE plugins create equal static & context menus.
     *
     * This method is never called for menu types other than 'GObjectViewMenuType::Static' and 'GObjectViewMenuType::Context'.
     */
    virtual void buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu);

    virtual void disconnectView(GObjectViewController* v);

    QMap<GObjectViewController*, QList<QObject*>> viewResources;
    GObjectViewFactoryId id;
};

}  // namespace U2
