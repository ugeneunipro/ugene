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

#ifndef _U2_WORKFLOW_VIEW_CONTROLLER_H_
#define _U2_WORKFLOW_VIEW_CONTROLLER_H_

#include <QAction>
#include <QGraphicsScene>

#include <U2Lang/ActorModel.h>
#include <U2Lang/Aliasing.h>
#include <U2Lang/AttributeInfo.h>
#include <U2Lang/Schema.h>
#include <U2Lang/SchemaConfig.h>
#include <U2Lang/WorkflowRunTask.h>

#include <U2Gui/MainWindow.h>

class QComboBox;
class QSplitter;
class QDomDocument;
class QListWidget;
class QListWidgetItem;
class QTabWidget;
class QGroupBox;

namespace U2 {
using namespace Workflow;
class DashboardManagerHelper;
class SamplesWidget;
class WorkflowView;
class WorkflowScene;
class WorkflowPalette;
class WorkflowEditor;
class WorkflowGObject;
class WorkflowBusItem;
class WorkflowPortItem;
class WorkflowProcessItem;
class WorkflowRunTask;
class WorkflowDebugStatus;
class BreakpointManagerView;
class WorkflowInvestigationWidgetsController;
class WorkflowTabView;
class ExternalToolLogParser;

class WorkflowScene : public QGraphicsScene {
    Q_OBJECT
    Q_DISABLE_COPY(WorkflowScene)
public:
    WorkflowScene(WorkflowView *parent = 0);
    virtual ~WorkflowScene();
    bool isModified() const {return modified;}
    bool isLocked() const {return locked;}

    WorkflowView* getController() const { return controller; }
    WorkflowAbstractRunner* getRunner() const {return runner;}
    void setRunner(WorkflowAbstractRunner* r) {runner = r;}

    const QPointF& getLastMousePressPoint() const {return lastMousePressPoint;}

    QList<Actor*> getSelectedActors() const;

    void clearScene();
    void onModified();

    void setupLinkCtxMenu(const QString& href, Actor* actor, const QPoint& pos);

    WorkflowBusItem * addFlow(WorkflowPortItem *from, WorkflowPortItem *to, Link *link);

public slots:
    void sl_deleteItem();
    void sl_selectAll();
    void sl_deselectAll();
    void sl_reset();
    void setLocked(bool b) {locked = b;}
    void setModified(bool b);
    void setModified();
    void centerView();
    void setHint(int i) {hint=i; update();}
    void sl_openDocuments();
    void sl_updateDocs() {
        emit configurationChanged();
    }
    void connectConfigurationEditors();

signals:
    void processItemAdded();
    void processDblClicked();
    void configurationChanged();
    void si_itemDeleted(const ActorId &id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * contextMenuEvent);

    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
    void dropEvent(QGraphicsSceneDragDropEvent * event);
    void keyPressEvent (QKeyEvent* event);
    void keyReleaseEvent (QKeyEvent* event);


    void drawBackground(QPainter * painter, const QRectF & rect );

private:
    WorkflowView* controller;
    bool modified, locked;

    WorkflowAbstractRunner* runner;
    int hint;
    QAction* openDocumentsAction;


    /* Point stores last mouse press position on the scene.  Used to insert new elements from context menu */
    QPointF lastMousePressPoint;
}; // WorkflowScene

class WorkflowView : public MWMDIWindow, public SchemaConfig {
    friend class DashboardManagerHelper;
    friend class WorkflowScene;
    Q_OBJECT
    Q_DISABLE_COPY(WorkflowView)

    WorkflowView(WorkflowGObject* go);
    static WorkflowView * createInstance(WorkflowGObject *go);

public:
    static WorkflowView * openWD(WorkflowGObject *go);
    ~WorkflowView();

    WorkflowEditor* getPropertyEditor() const { return propertyEditor; }

    virtual void setupMDIToolbar(QToolBar* tb);
    virtual void setupViewMenu(QMenu* n);
    void setupContextMenu(QMenu* menu);

    void onModified();
    bool confirmModified();

    ActorPrototype* selectedProto() const {
        return currentProto;
    }

    Actor* getActor() {
        return currentActor;
    }
    void addProcess(Actor *proc, const QPointF &pos);
    void removeProcessItem(WorkflowProcessItem *item);
    void removeBusItem(WorkflowBusItem *item);
    void onBusRemoved(Link *link);
    Actor * createActor(ActorPrototype *proto, const QVariantMap &params) const;
    WorkflowBusItem * tryBind(WorkflowPortItem *port1, WorkflowPortItem *port2);

    WorkflowScene * getScene() const {return scene;}
    Workflow::Schema * getSchema() const;
    const Workflow::Metadata & getMeta();
    const Workflow::Metadata & updateMeta();
    Workflow::Metadata getMeta(const QList<WorkflowProcessItem*> &items);

    void refreshView() {sl_refreshActorDocs();}

    // SchemaConfig
    virtual RunFileSystem * getRFS();
    virtual QVariant getAttributeValue(const AttributeInfo &info) const;
    virtual void setAttributeValue(const AttributeInfo &info, const QVariant &value);

    bool isShowSamplesHint() const;

public slots:
    void sl_updateUi();
    void sl_rescaleScene(const QString &scale);
    void sl_loadScene(const QString &url, bool fromDashboard = true);

private slots:
    void sl_editItem();
    void sl_onSelectionChanged();
    void sl_showEditor();
    void sl_selectPrototype(Workflow::ActorPrototype*, bool);
    void sl_exportScene();
    void sl_saveScene();
    void sl_saveSceneAs();
    void sl_loadScene();
    void sl_newScene();
    void sl_updateTitle();
    void sl_onSceneLoaded();
    void sl_onSceneSaved();

    void sl_copyItems();
    void sl_cutItems();
    void sl_pasteItems(const QString& = QString(), bool updateSchemaInfo = false);
    void sl_pasteSample(const QString&);

    void sl_setStyle();
    void sl_toggleStyle();
    void sl_refreshActorDocs();

    void sl_changeScriptMode();

    bool sl_validate(bool notify = true);
    void sl_estimate();
    void sl_estimationTaskFinished();
    void sl_pickInfo(QListWidgetItem*);
    void sl_launch();
    void sl_stop();
    void sl_pause(bool isPause = true);
    void sl_configureParameterAliases();
    void sl_createGalaxyConfig();
    void sl_configurePortAliases();
    void sl_importSchemaToElement();

    void sl_toggleLock(bool = true);
    void sl_createScript();
    void sl_editScript();
    void sl_createCmdlineBasedWorkerAction();
    void sl_appendExternalToolWorker();
    void sl_prototypeIsAboutToBeRemoved(Workflow::ActorPrototype *proto);
    void sl_xmlSchemaLoaded(Task*);
    void sl_editExternalTool();
    void sl_findPrototype();
    void sl_protoListModified();

    void sl_updateSchema();
    void sl_showWizard();
    void sl_toggleDashboard();
    void sl_dashboardCountChanged();

    void sl_toggleBreakpointManager();

    void sl_breakpointAdded(const ActorId &actor);
    void sl_breakpointRemoved(const ActorId &actor);
    void sl_breakpointIsReached(const U2::ActorId &actor);
    void sl_processOneMessage();
    void sl_highlightingRequested(const ActorId &actor);
    void sl_breakpointEnabled(const ActorId &actor);
    void sl_breakpointDisabled(const ActorId &actor);
    void sl_convertMessages2Documents(const Workflow::Link *bus, const QString &messageType,
        int messageNumber);

protected:
    bool onCloseEvent();
    virtual void paintEvent(QPaintEvent *event);

private:
    void createActions();
    void saveState();
    void recreateScene();
    void localHostLaunch();
    void rescale(bool updateGui = true);

    void toggleDebugActionsState(bool enable);
    void changeBreakpointState(const ActorId &actor, bool isBreakpointBeingAdded,
    bool isBreakpointStateBeingChanged = false);
    WorkflowProcessItem *findItemById(ActorId actor) const;
    void addBottomWidgetsToInfoSplitter();
    void setInvestigationWidgetsVisible(bool visible);
    void propagateBreakpointToSceneItem(ActorId actor);

    void removeWizards();
    void removeEstimations();

    // setup ui
    void setupScene();
    void setupPalette();
    void setupPropertyEditor();
    void setupErrorList();
    void setupMainSplitter();
    void loadSceneFromObject();
    void loadUiSettings();

    // dashboards
    void setupActions();
    void hideDashboards();
    void showDashboards();
    void setDashboardActionDecoration(bool isDashboardsViewActive);
    void setDashboardActionVisible(bool visible);
    void commitWarningsToMonitor(WorkflowAbstractRunner *t);

    void runWizard(Wizard *w);
    void checkAutoRunWizard();
    void loadWizardResult(const QString &result);
    void procItemAdded();

    DashboardManagerHelper * getDMHInstance();

private:
    bool running;
    bool sceneRecreation;
    WorkflowGObject* go;
    Schema *schema;
    Workflow::Metadata meta;
    ActorPrototype* currentProto;
    Actor* currentActor;

    QString lastPaste;
    int pasteCount;

    bool scriptingMode;

    QPointer<QMenu> elementsMenu;

    QAction* deleteAction;
    QAction* deleteShortcut;
    QAction* selectAction;
    QAction* copyAction;
    QAction* pasteAction;
    QAction* cutAction;
    QAction* exportAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* loadAction;
    QAction* newAction;
    QAction* createScriptAction;
    QAction* editScriptAction;
    QAction* createCmdlineBasedWorkerAction;
    QAction* appendExternalTool;
    QAction* editExternalToolAction;
    QAction* configureParameterAliasesAction;
    QAction* createGalaxyConfigAction;
    QAction* configurePortAliasesAction;
    QAction* importSchemaToElement;
    QAction* runAction;
    QAction* stopAction;
    QAction* validateAction;
    QAction *pauseAction;
    QAction *nextStepAction;
    QAction *toggleBreakpointAction;
    QAction *tickReadyAction;
    QAction *estimateAction;

    QAction* findPrototypeAction;
    QAction* unlockAction;
    QAction* showWizard;
    QAction* toggleDashboard;
    QAction* loadSep;
    QAction* runSep;
    QAction* confSep;
    QAction* extSep;
    QAction* scaleSep;
    QAction* scaleAction;
    QAction* scriptAction;
    QAction* dmAction;
    QList<QAction*> styleActions;
    QList<QAction*> scriptingActions;
    QComboBox *scaleComboBox;

    QAction *toggleBreakpointManager;

    QSplitter*              splitter;
    WorkflowPalette*        palette;
    WorkflowEditor*         propertyEditor;
    WorkflowTabView*        tabView;
    WorkflowScene*          scene;
    QGraphicsView*          sceneView;
    SamplesWidget*          samples;
    QTabWidget*             tabs;
    QGroupBox*              errorList;
    QListWidget*            infoList;

    QSplitter*      infoSplitter;

    WorkflowDebugStatus *debugInfo;
    QList<QAction *> debugActions;
    BreakpointManagerView *breakpointView;
    QTabWidget *bottomTabs;
    WorkflowInvestigationWidgetsController *investigationWidgets;

    ExternalToolLogParser* rLogParser;
};

class SceneCreator {
public:
    SceneCreator(Schema *schema, const Workflow::Metadata &meta);
    virtual ~SceneCreator();

    WorkflowScene * createScene(WorkflowView *controller);
    WorkflowScene * recreateScene(WorkflowScene *scene);

private:
    Schema *schema;
    Workflow::Metadata meta;
    WorkflowScene *scene;

    WorkflowScene * createScene();
    WorkflowProcessItem * createProcess(Actor *actor);
    void createBus(const QMap<Port*, WorkflowPortItem*> &ports, Link *link);
};

class DashboardManagerHelper : public QObject {
    Q_OBJECT
public:
    DashboardManagerHelper(QAction *dmAction, WorkflowView *parent);

private slots:
    void sl_result(int result);
    void sl_showDashboardsManagerDialog();
    void sl_dashboardsScanningStarted();
    void sl_dashboardsScanningFinished();

private:
    QAction *dmAction;
    WorkflowView *parent;
};

}//namespace


#endif
