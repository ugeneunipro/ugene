/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <algorithm>
#include <functional>

#include <QBoxLayout>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QShortcut>
#include <QSplitter>
#include <QSvgGenerator>
#include <QTableWidget>
#include <QToolBar>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectService.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GUrlUtils.h>

#include <U2Designer/Dashboard.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Designer/DesignerUtils.h>
#include <U2Designer/EstimationReporter.h>
#include <U2Designer/GrouperEditor.h>
#include <U2Designer/MarkerEditor.h>
#include <U2Designer/WizardController.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GlassView.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseAttributes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/GrouperSlotAttribute.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/MapDatatypeEditor.h>
#include <U2Lang/SchemaEstimationTask.h>
#include <U2Lang/WorkflowDebugStatus.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowManager.h>
#include <U2Lang/WorkflowRunTask.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "BreakpointManagerView.h"
#include "ChooseItemDialog.h"
#include "CreateScriptWorker.h"
#include "DashboardsManagerDialog.h"
#include "EstimationDialog.h"
#include "GalaxyConfigConfigurationDialogImpl.h"
#include "ImportSchemaDialog.h"
#include "ItemViewStyle.h"
#include "PortAliasesConfigurationDialog.h"
#include "SceneSerializer.h"
#include "SchemaAliasesConfigurationDialogImpl.h"
#include "StartupDialog.h"
#include "WorkflowDesignerPlugin.h"
#include "WorkflowDocument.h"
#include "WorkflowEditor.h"
#include "WorkflowInvestigationWidgetsController.h"
#include "WorkflowMetaDialog.h"
#include "WorkflowPalette.h"
#include "WorkflowSamples.h"
#include "WorkflowSceneIOTasks.h"
#include "WorkflowTabView.h"
#include "WorkflowViewController.h"
#include "WorkflowViewItems.h"
#include "debug_messages_translation/WorkflowDebugMessageParserImpl.h"
#include "library/CreateExternalProcessDialog.h"
#include "library/ExternalProcessWorker.h"
#include "library/ScriptWorker.h"

namespace U2 {

#define LAST_DIR SETTINGS + "lastdir"
#define SPLITTER_STATE SETTINGS + "splitter"
#define EDITOR_STATE SETTINGS + "editor"
#define PALETTE_STATE SETTINGS + "palette"
#define TABS_STATE SETTINGS + "tabs"
#define CHECK_R_PACKAGE SETTINGS + "check_r_for_cistrome"

enum {ElementsTab,SamplesTab};

#define WS 1000
#define MAX_FILE_SIZE 1000000

static const QString XML_SCHEMA_WARNING = QObject::tr("You opened obsolete workflow in XML format. It is strongly recommended"
                                                           " to clear working space and create workflow from scratch.");

static const QString XML_SCHEMA_APOLOGIZE = QObject::tr("Sorry! This workflow is obsolete and cannot be opened.");

static const QString BREAKPOINT_MANAGER_LABEL = QObject::tr( "Breakpoints" );
static const int ABSENT_WIDGET_TAB_NUMBER = -1;

/************************************************************************/
/* Utilities */
/************************************************************************/
static QString percentStr = QObject::tr("%");
class PercentValidator : public QRegExpValidator {
public:
    PercentValidator(QObject* parent) : QRegExpValidator(QRegExp("[1-9][0-9]*"+percentStr), parent) {}
    void fixup(QString& input) const {
        if (!input.endsWith(percentStr)) {
            input.append(percentStr);
        }
    }
}; // PercentValidator

static QComboBox * scaleCombo(WorkflowView *parent) {
    QComboBox *sceneScaleCombo = new QComboBox(parent);
    sceneScaleCombo->setEditable(true);
    sceneScaleCombo->setValidator(new PercentValidator(parent));
    QStringList scales;
    scales << "25%" << "50%" << "75%" << "100%" << "125%" << "150%" << "200%";
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(3);
    QObject::connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)), parent, SLOT(sl_rescaleScene(const QString &)));
    // Some visual modifications for Mac:
    sceneScaleCombo->lineEdit()->setStyleSheet("QLineEdit {margin-right: 1px;}");
    sceneScaleCombo->setObjectName( "wdScaleCombo" );
    return sceneScaleCombo;
}

static void addToggleDashboardAction(QToolBar *toolBar, QAction *action) {
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    toolBar->addAction(action);
    QToolButton *b = dynamic_cast<QToolButton*>(toolBar->widgetForAction(action));
    CHECK(NULL != b, );
    b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    b->setAutoRaise(false);

#ifdef Q_OS_MAC
    b->setStyleSheet("QToolButton {"
                     "font-size: 13px;"
                     "border: 1px solid gray;"
                     "border-radius: 6px;"
                     "margin-right: 5px;"
                     "height: 25px;"
                     "padding: 0px 5px;"
                     "}");
#endif
}

static QToolButton * styleMenu(WorkflowView *parent, const QList<QAction*> &actions) {
    QToolButton *tt = new QToolButton(parent);
    tt->setObjectName("Element style");
    QMenu *ttMenu = new QMenu( QObject::tr("Element style"), parent );
    foreach(QAction *a, actions) {
        ttMenu->addAction( a );
    }
    tt->setDefaultAction(ttMenu->menuAction());
    tt->setPopupMode(QToolButton::InstantPopup);
    return tt;
}

static QToolButton * scriptMenu(WorkflowView *parent, const QList<QAction*> &scriptingActions) {
    QToolButton *scriptingModeButton = new QToolButton(parent);
    QMenu *scriptingModeMenu = new QMenu( QObject::tr( "Scripting mode" ), parent );
    foreach( QAction * a, scriptingActions ) {
        scriptingModeMenu->addAction( a );
    }
    scriptingModeButton->setDefaultAction( scriptingModeMenu->menuAction() );
    scriptingModeButton->setPopupMode( QToolButton::InstantPopup );
    return scriptingModeButton;
}

DashboardManagerHelper::DashboardManagerHelper(QAction *_dmAction, WorkflowView *_parent)
: dmAction(_dmAction), parent(_parent)
{
    connect(dmAction, SIGNAL(triggered()), SLOT(sl_runScanTask()));
}

void DashboardManagerHelper::sl_runScanTask() {
    dmAction->setEnabled(false);
    ScanDashboardsDirTask *t = new ScanDashboardsDirTask();
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_scanTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void DashboardManagerHelper::sl_scanTaskFinished() {
    ScanDashboardsDirTask *t = dynamic_cast<ScanDashboardsDirTask*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished(), );

    if (t->getResult().isEmpty()) {
        dmAction->setEnabled(true);
        QMessageBox *d = new QMessageBox(QMessageBox::Information, tr("No Dashboards Found"),
            tr("You do not have any dashboards yet. You need to run some workflow to use Dashboards Manager."), QMessageBox::NoButton, parent);
        d->show();
        return;
    }

    DashboardsManagerDialog *d = new DashboardsManagerDialog(t, parent);
    connect(d, SIGNAL(finished(int)), SLOT(sl_result(int)));
    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}

void DashboardManagerHelper::sl_result(int result) {
    DashboardsManagerDialog *d = dynamic_cast<DashboardsManagerDialog*>(sender());
    if (QDialog::Accepted == result) {
        parent->tabView->updateDashboards(d->selectedDashboards());
        if (!d->removedDashboards().isEmpty()) {
            RemoveDashboardsTask *rt = new RemoveDashboardsTask(d->removedDashboards());
            connect(rt, SIGNAL(si_stateChanged()), SLOT(sl_removeTaskFinished()));
            AppContext::getTaskScheduler()->registerTopLevelTask(rt);
            return;
        }
    }
    dmAction->setEnabled(true);
}

void DashboardManagerHelper::sl_removeTaskFinished() {
    RemoveDashboardsTask *t = dynamic_cast<RemoveDashboardsTask*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished(), );
    dmAction->setEnabled(true);
}

/********************************
* WorkflowView
********************************/
WorkflowView * WorkflowView::createInstance(WorkflowGObject *go) {
    MWMDIManager *mdiManager = AppContext::getMainWindow()->getMDIManager();
    SAFE_POINT(NULL != mdiManager, "NULL MDI manager", NULL);

    WorkflowView *view = new WorkflowView(go);
    view->setWindowIcon(QIcon(":/workflow_designer/images/wd.png"));
    mdiManager->addMDIWindow(view);
    mdiManager->activateWindow(view);
    return view;
}

WorkflowView * WorkflowView::openWD(WorkflowGObject *go) {
    if (WorkflowSettings::isOutputDirectorySet()) {
        return createInstance(go);
    }

    QObjectScopedPointer<StartupDialog> d = new StartupDialog(AppContext::getMainWindow()->getQMainWindow());
    d->exec();
    CHECK(!d.isNull(), NULL);

    if (QDialog::Accepted == d->result()) {
        return createInstance(go);
    }
    return NULL;
}

WorkflowView::WorkflowView(WorkflowGObject* go)
: MWMDIWindow(tr("Workflow Designer")), running(false), sceneRecreation(false), go(go), currentProto(NULL), currentActor(NULL),
pasteCount(0), debugInfo(new WorkflowDebugStatus(this)), debugActions()
{
    scriptingMode = WorkflowSettings::getScriptingMode();
    elementsMenu = NULL;
    schema = new Schema();
    schema->setDeepCopyFlag(true);

    setupScene();
    setupPalette();
    setupPropertyEditor();
    setupErrorList();

    infoSplitter = new QSplitter(Qt::Vertical);
    infoSplitter->addWidget(sceneView);
    infoSplitter->addWidget(errorList);
    addBottomWidgetsToInfoSplitter();
    setupMainSplitter();

    loadUiSettings();

    createActions();
    sl_changeScriptMode();

    if(go) {
        loadSceneFromObject();
    } else {
        sl_newScene();
    }

    propertyEditor->reset();
}

WorkflowView::~WorkflowView() {
    uiLog.trace("~WorkflowView");
    if(AppContext::getProjectService()) {
        AppContext::getProjectService()->enableSaveAction(true);
    }
    WorkflowSettings::setScriptingMode(scriptingMode);
}

void WorkflowView::setupScene() {
    SceneCreator sc(schema, meta);
    scene = sc.createScene(this);

    sceneView = new GlassView(scene);
    sceneView->setObjectName("sceneView");
    sceneView->setAlignment(Qt::AlignCenter);

    scene->views().at(0)->setDragMode(QGraphicsView::RubberBandDrag);

    connect(scene, SIGNAL(processItemAdded()), SLOT(sl_procItemAdded()));
    connect(scene, SIGNAL(processDblClicked()), SLOT(sl_toggleStyle()));
    connect(scene, SIGNAL(selectionChanged()), SLOT(sl_editItem()));
    connect(scene, SIGNAL(selectionChanged()), SLOT(sl_onSelectionChanged()));
    connect(scene, SIGNAL(configurationChanged()), SLOT(sl_refreshActorDocs()));
    connect(WorkflowSettings::watcher, SIGNAL(changed()), scene, SLOT(update()));
}

void WorkflowView::setupPalette() {
    palette = new WorkflowPalette(WorkflowEnv::getProtoRegistry());
    palette->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
    connect(palette, SIGNAL(processSelected(Workflow::ActorPrototype*, bool)), SLOT(sl_selectPrototype(Workflow::ActorPrototype*, bool)));
    connect(palette, SIGNAL(si_protoDeleted(const QString&)), SLOT(sl_protoDeleted(const QString&)));
    connect(palette, SIGNAL(si_protoListModified()), SLOT(sl_protoListModified()));
    connect(palette, SIGNAL(si_protoChanged()), scene, SLOT(sl_updateDocs()));

    tabs = new QTabWidget(this);
    tabs->setObjectName("tabs");
    tabs->insertTab(ElementsTab, palette, tr("Elements"));
    samples = new SamplesWidget(scene);
    samples->setObjectName("samples");
    tabs->insertTab(SamplesTab, new SamplesWrapper(samples, this), tr("Samples"));
    tabs->setTabPosition(QTabWidget::North);

    connect(samples, SIGNAL(setupGlass(GlassPane*)), sceneView, SLOT(setGlass(GlassPane*)));
    connect(samples, SIGNAL(sampleSelected(QString)), this, SLOT(sl_pasteSample(QString)));
    connect(tabs, SIGNAL(currentChanged(int)), samples, SLOT(cancelItem()));
    connect(tabs, SIGNAL(currentChanged(int)), palette, SLOT(resetSelection()));
    connect(tabs, SIGNAL(currentChanged(int)), scene, SLOT(setHint(int)));
}

void WorkflowView::setupErrorList() {
    infoList = new QListWidget(this);
    connect(infoList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(sl_pickInfo(QListWidgetItem*)));
    errorList = new QGroupBox();
    {
        errorList->setFlat(true);
        errorList->setTitle(tr("Error list"));
        QVBoxLayout* vl = new QVBoxLayout(errorList);
        vl->setSpacing(0);
        vl->setMargin(0);
        vl->setContentsMargins(0,0,0,0);
        vl->addWidget(infoList);
    }
    errorList->hide();
}

void WorkflowView::setupPropertyEditor() {
    propertyEditor = new WorkflowEditor(this);
}

void WorkflowView::loadSceneFromObject() {
    LoadWorkflowTask::FileFormat format = LoadWorkflowTask::detectFormat(go->getSceneRawData());
    go->setView(this);
    QString err;
    if(format == LoadWorkflowTask::HR) {
        err = HRSchemaSerializer::string2Schema(go->getSceneRawData(), schema, &meta);
    } else if(format == LoadWorkflowTask::XML) {
        QDomDocument xml;
        QMap<ActorId, ActorId> remapping;
        xml.setContent(go->getSceneRawData().toUtf8());
        err = SchemaSerializer::xml2schema(xml.documentElement(), schema, remapping);
        SchemaSerializer::readMeta(&meta, xml.documentElement());
        scene->setModified(false);
        if(err.isEmpty()) {
            QMessageBox::warning(this, tr("Warning!"), XML_SCHEMA_WARNING);
        } else {
            QMessageBox::warning(this, tr("Warning!"), XML_SCHEMA_APOLOGIZE);
        }
    } else {
        coreLog.error(tr("Undefined workflow format for %1").arg(go->getDocument() ? go->getDocument()->getURLString() : tr("file")));
        sl_newScene();
    }
    scene->connectConfigurationEditors();

    if (!err.isEmpty()) {
        sl_newScene();
        coreLog.error(err);
    } else {
        SceneCreator sc(schema, meta);
        sc.recreateScene(scene);
        if (go->getDocument()) {
            meta.url = go->getDocument()->getURLString();
        }
        sl_updateTitle();
        scene->setModified(false);
        rescale();
        sl_refreshActorDocs();
    }
}

void WorkflowView::loadUiSettings() {
    Settings* settings = AppContext::getSettings();
    if (settings->contains(SPLITTER_STATE)) {
        splitter->restoreState(settings->getValue(SPLITTER_STATE).toByteArray());
    }
    if (settings->contains(PALETTE_STATE)) {
        palette->restoreState(settings->getValue(PALETTE_STATE));
    }
    tabs->setCurrentIndex(settings->getValue(TABS_STATE, SamplesTab).toInt());
}

void WorkflowView::setupMainSplitter() {
    splitter = new QSplitter(this);
    splitter->setObjectName("splitter");
    {
        splitter->addWidget(tabs);
        splitter->addWidget(infoSplitter);
        splitter->addWidget(propertyEditor);

        splitter->setStretchFactor(0, 0);
        splitter->setStretchFactor(1, 1);
        splitter->setStretchFactor(2, 0);
    }

    tabView = new WorkflowTabView(this);
    tabView->hide();
    connect(tabView, SIGNAL(si_countChanged()), SLOT(sl_dashboardCountChanged()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(tabView);
    layout->addWidget(splitter);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(debugInfo, SIGNAL(si_pauseStateChanged(bool)), scene, SLOT(update()));
    connect(debugInfo, SIGNAL(si_pauseStateChanged(bool)), SLOT(sl_pause(bool)));
    connect(investigationWidgets,
        SIGNAL(si_updateCurrentInvestigation(const Workflow::Link *, int)), debugInfo,
        SIGNAL(si_busInvestigationIsRequested(const Workflow::Link *, int)));
    connect(investigationWidgets, SIGNAL(si_countOfMessagesRequested(const Workflow::Link *)),
        debugInfo, SIGNAL(si_busCountOfMessagesIsRequested(const Workflow::Link *)));
    connect(debugInfo,
        SIGNAL(si_busInvestigationRespond(const WorkflowInvestigationData &, const Workflow::Link *)),
        investigationWidgets,
        SLOT(sl_currentInvestigationUpdateResponse(const WorkflowInvestigationData &, const Workflow::Link *)));
    connect(debugInfo, SIGNAL(si_busCountOfMessagesResponse(const Workflow::Link *, int)),
        investigationWidgets, SLOT(sl_countOfMessagesResponse(const Workflow::Link *, int)));
    connect(investigationWidgets, SIGNAL(si_convertionMessages2DocumentsIsRequested(
        const Workflow::Link *, const QString &, int)),
        SLOT(sl_convertMessages2Documents(const Workflow::Link *, const QString &, int)));
    connect(debugInfo, SIGNAL(si_breakpointAdded(const ActorId &)),
        SLOT(sl_breakpointAdded(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointEnabled(const ActorId &)), SLOT(sl_breakpointEnabled(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointRemoved(const ActorId &)),
        SLOT(sl_breakpointRemoved(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointDisabled(const ActorId &)),
        SLOT(sl_breakpointDisabled(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointIsReached(const U2::ActorId &)),
        SLOT(sl_breakpointIsReached(const U2::ActorId &)));
}

void WorkflowView::sl_breakpointIsReached(const U2::ActorId &actor) {
    propagateBreakpointToSceneItem(actor);
    breakpointView->onBreakpointReached(actor);
}

void WorkflowView::addBottomWidgetsToInfoSplitter() {
    bottomTabs = new QTabWidget(infoSplitter);

    infoList = new QListWidget(this);
    infoList->setObjectName("infoList");
    connect(infoList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(sl_pickInfo(QListWidgetItem*)));

    QWidget* w = new QWidget(bottomTabs);
    QVBoxLayout* vl = new QVBoxLayout(w);
    vl->setSpacing(0);
    vl->setMargin(0);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addWidget(infoList);
    w->hide();
    bottomTabs->addTab(w, tr("Error list"));

    breakpointView = new BreakpointManagerView(debugInfo, schema, scene);
    connect(breakpointView, SIGNAL(si_highlightingRequested(const ActorId &)),
        SLOT(sl_highlightingRequested(const ActorId &)));
    connect(scene, SIGNAL(si_itemDeleted(const ActorId &)),
            breakpointView, SLOT(sl_breakpointRemoved(const ActorId &)));
    if ( WorkflowSettings::isDebuggerEnabled( ) ) {
        bottomTabs->addTab( breakpointView, BREAKPOINT_MANAGER_LABEL );
    }

    investigationWidgets = new WorkflowInvestigationWidgetsController(bottomTabs);

    infoSplitter->addWidget(bottomTabs);
    bottomTabs->hide();
}

void WorkflowView::sl_rescaleScene(const QString &scale)
{
    int percentPos = scale.indexOf(percentStr);
    meta.scalePercent = scale.left(percentPos).toInt();
    rescale(false);
}

static void updateComboBox(QComboBox *scaleComboBox, int scalePercent) {
    QString value = QString("%1%2").arg(scalePercent).arg(percentStr);
    bool isOk = true;
    for (int i=0; i<scaleComboBox->count(); i++) {
        if (scaleComboBox->itemText(i) == value) {
            scaleComboBox->setCurrentIndex(i);
            return;
        } else{
            QString itemText = scaleComboBox->itemText(i).mid(0, scaleComboBox->itemText(i).size() - percentStr.size());
            if (itemText.toInt(&isOk) > scalePercent && isOk){
                scaleComboBox->insertItem(i, value);
                scaleComboBox->setCurrentIndex(i);
                return;
            }
        }
    }
    scaleComboBox->addItem(value);
    scaleComboBox->setCurrentIndex(scaleComboBox->count() - 1);
}

void WorkflowView::rescale(bool updateGui) {
    double newScale = meta.scalePercent / 100.0;
    QMatrix oldMatrix = scene->views().at(0)->matrix();
    scene->views().at(0)->resetMatrix();
    scene->views().at(0)->translate(oldMatrix.dx(), oldMatrix.dy());
    scene->views().at(0)->scale(newScale, newScale);
    QRectF rect = scene->sceneRect();
    qreal w = rect.width()/newScale;
    qreal h = rect.height()/newScale;
    rect.setWidth(w);
    rect.setHeight(h);
    scene->setSceneRect(rect);
    if (updateGui) {
        updateComboBox(scaleComboBox, meta.scalePercent);
    }
}

void WorkflowView::createActions() {
    runAction = new QAction(tr("&Run workflow"), this);
    runAction->setObjectName("Run workflow");
    runAction->setIcon(QIcon(":workflow_designer/images/run.png"));
    runAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(runAction, SIGNAL(triggered()), SLOT(sl_launch()));
    connect(runAction, SIGNAL(triggered()), debugInfo, SLOT(sl_resumeTriggerActivated()));

    stopAction = new QAction(tr("S&top workflow"), this);
    stopAction->setObjectName("Stop workflow");
    stopAction->setIcon(QIcon(":workflow_designer/images/stopTask.png"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()), debugInfo, SLOT(sl_executionFinished()));
    connect(stopAction, SIGNAL(triggered()), SLOT(sl_stop()));

    validateAction = new QAction(tr("&Validate workflow"), this);
    validateAction->setObjectName("Validate workflow");
    validateAction->setIcon(QIcon(":workflow_designer/images/ok.png"));
    validateAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(validateAction, SIGNAL(triggered()), SLOT(sl_validate()));

    estimateAction = new QAction(tr("&Estimate workflow"), this);
    estimateAction->setObjectName("Estimate workflow");
    estimateAction->setIcon(QIcon(":core/images/sum.png"));
    estimateAction->setObjectName("Estimate workflow");
    connect(estimateAction, SIGNAL(triggered()), SLOT(sl_estimate()));

    pauseAction = new QAction(tr("&Pause workflow"), this);
    pauseAction->setObjectName("Pause workflow");
    pauseAction->setIcon(QIcon(":workflow_designer/images/pause.png"));
    pauseAction->setShortcut(QKeySequence("Ctrl+P"));
    pauseAction->setEnabled(false);
    connect(pauseAction, SIGNAL(triggered()), debugInfo, SLOT(sl_pauseTriggerActivated()));
    debugActions.append(pauseAction);

    nextStepAction = new QAction(tr("&Next step"), this);
    nextStepAction->setIcon(QIcon(":workflow_designer/images/next_step.png"));
    nextStepAction->setShortcut(QKeySequence("F10"));
    nextStepAction->setEnabled(false);
    connect(nextStepAction, SIGNAL(triggered()), debugInfo, SLOT(sl_isolatedStepTriggerActivated()));
    debugActions.append(nextStepAction);

    toggleBreakpointAction = breakpointView->getNewBreakpointAction();
    toggleBreakpointAction->setEnabled(false);

    tickReadyAction = new QAction(tr("Process one &message"), this);
    tickReadyAction->setIcon(QIcon(":workflow_designer/images/process_one_message.png"));
    tickReadyAction->setShortcut(QKeySequence("Ctrl+M"));
    tickReadyAction->setEnabled(false);
    connect(tickReadyAction, SIGNAL(triggered()), SLOT(sl_processOneMessage()));
    connect(tickReadyAction, SIGNAL(triggered()), scene, SLOT(update()));
    connect(tickReadyAction, SIGNAL(triggered()), SLOT(sl_onSelectionChanged()));
    connect(tickReadyAction, SIGNAL(triggered()), bottomTabs, SLOT(update()));
    debugActions.append(tickReadyAction);

    newAction = new QAction(tr("&New workflow..."), this);
    newAction->setIcon(QIcon(":workflow_designer/images/filenew.png"));
    newAction->setShortcuts(QKeySequence::New);
    newAction->setObjectName("New workflow action");
    connect(newAction, SIGNAL(triggered()), SLOT(sl_newScene()));

    saveAction = new QAction(tr("&Save workflow"), this);
    saveAction->setObjectName("Save workflow");
    saveAction->setIcon(QIcon(":workflow_designer/images/filesave.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setShortcutContext(Qt::WindowShortcut);
    connect(saveAction, SIGNAL(triggered()), SLOT(sl_saveScene()));

    saveAsAction = new QAction(tr("&Save workflow as..."), this);
    saveAsAction->setIcon(QIcon(":workflow_designer/images/filesaveas.png"));
    connect(saveAsAction, SIGNAL(triggered()), SLOT(sl_saveSceneAs()));
    saveAsAction->setObjectName( "Save workflow action" );

    showWizard = new QAction(tr("Show wizard"), this);
    showWizard->setObjectName("Show wizard");
    QPixmap pm = QPixmap(":workflow_designer/images/wizard.png").scaled(16, 16);
    showWizard->setIcon(QIcon(pm));
    connect(showWizard, SIGNAL(triggered()), SLOT(sl_showWizard()));

    toggleBreakpointManager = new QAction("Show or hide breakpoint manager", this);
    toggleBreakpointManager->setIcon(QIcon(":workflow_designer/images/show_breakpoint_manager.png"));
    toggleBreakpointManager->setObjectName("Show or hide breakpoint manager");
    connect(toggleBreakpointManager, SIGNAL(triggered()), SLOT(sl_toggleBreakpointManager()));


    { // toggle dashboard action
        toggleDashboard = new QAction(this);
        toggleDashboard->setObjectName("toggleDashboard");
        connect(toggleDashboard, SIGNAL(triggered()), SLOT(sl_toggleDashboard()));
    }

    loadAction = new QAction(tr("&Load workflow"), this);
    loadAction->setIcon(QIcon(":workflow_designer/images/fileopen.png"));
    loadAction->setShortcut(QKeySequence("Ctrl+L"));
    loadAction->setObjectName("Load workflow");
    connect(loadAction, SIGNAL(triggered()), SLOT(sl_loadScene()));

    exportAction = new QAction(tr("&Export workflow as image"), this);
    exportAction->setIcon(QIcon(":workflow_designer/images/export.png"));
    exportAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(exportAction, SIGNAL(triggered()), SLOT(sl_exportScene()));

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setIcon(QIcon(":workflow_designer/images/delete.png"));
    connect(deleteAction, SIGNAL(triggered()), scene, SLOT(sl_deleteItem()));

    dmAction = new QAction(tr("Dashboards manager"), this);
    dmAction->setIcon(QIcon(":workflow_designer/images/settings.png"));
    dmAction->setObjectName("Dashboards manager");
    new DashboardManagerHelper(dmAction, this);

    { // Delete shortcut
        deleteShortcut = new QAction(sceneView);
        deleteShortcut->setShortcuts(QKeySequence::Delete);
        deleteShortcut->setShortcutContext(Qt::WidgetShortcut);
        connect(deleteShortcut, SIGNAL(triggered()), scene, SLOT(sl_deleteItem()));
        sceneView->addAction(deleteShortcut);
    }

    { // Ctrl+A shortcut
        QAction *selectShortcut = new QAction(sceneView);
        selectShortcut->setShortcuts(QKeySequence::SelectAll);
        selectShortcut->setShortcutContext(Qt::WidgetShortcut);
        connect(selectShortcut, SIGNAL(triggered()), scene, SLOT(sl_selectAll()));
        sceneView->addAction(selectShortcut);
    }

    configureParameterAliasesAction = new QAction(tr("Set parameter aliases..."), this);
    configureParameterAliasesAction->setObjectName("Set parameter aliases");
    configureParameterAliasesAction->setIcon(QIcon(":workflow_designer/images/table_relationship.png"));
    connect(configureParameterAliasesAction, SIGNAL(triggered()), SLOT(sl_configureParameterAliases()));

    configurePortAliasesAction = new QAction(tr("Set port and slot aliases..."), this);
    configurePortAliasesAction->setIcon(QIcon(":workflow_designer/images/port_relationship.png"));
    connect(configurePortAliasesAction, SIGNAL(triggered()), SLOT(sl_configurePortAliases()));

    importSchemaToElement = new QAction(tr("Import workflow to element..."), this);
    importSchemaToElement->setIcon(QIcon(":workflow_designer/images/import.png"));
    connect(importSchemaToElement, SIGNAL(triggered()), SLOT(sl_importSchemaToElement()));

    createGalaxyConfigAction = new QAction(tr("Create Galaxy tool config..."), this);
    createGalaxyConfigAction->setObjectName("Create Galaxy tool config");
    createGalaxyConfigAction->setIcon(QIcon(":workflow_designer/images/galaxy.png"));
    connect(createGalaxyConfigAction, SIGNAL(triggered()), SLOT(sl_createGalaxyConfig()));

    selectAction = new QAction(tr("Select all elements"), this);
    connect(selectAction, SIGNAL(triggered()), scene, SLOT(sl_selectAll()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":workflow_designer/images/editcopy.png"));
    copyAction->setShortcut(QKeySequence("Ctrl+C"));
    copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    copyAction->setObjectName("Copy action");
    connect(copyAction, SIGNAL(triggered()), SLOT(sl_copyItems()));
    addAction(copyAction);

    cutAction = new QAction(tr("Cu&t"), sceneView);
    cutAction->setIcon(QIcon(":workflow_designer/images/editcut.png"));
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setShortcutContext(Qt::WidgetShortcut);
    connect(cutAction, SIGNAL(triggered()), SLOT(sl_cutItems()));
    addAction(cutAction);

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon(":workflow_designer/images/editpaste.png"));
    pasteAction->setShortcuts(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(pasteAction, SIGNAL(triggered()), SLOT(sl_pasteItems()));
    addAction(pasteAction);

    { // style
        QAction* simpleStyle = new QAction(tr("Minimal"), this);
        simpleStyle->setObjectName("Minimal");
        simpleStyle->setData(QVariant(ItemStyles::SIMPLE));
        styleActions << simpleStyle;
        connect(simpleStyle, SIGNAL(triggered()), SLOT(sl_setStyle()));

        QAction* extStyle = new QAction(tr("Extended"), this);
        extStyle->setObjectName("Extended");
        extStyle->setData(QVariant(ItemStyles::EXTENDED));
        styleActions << extStyle;
        connect(extStyle, SIGNAL(triggered()), SLOT(sl_setStyle()));
    }

    { // scripting mode
        QAction * notShowScriptAction = new QAction( tr( "Hide scripting options" ), this );
        notShowScriptAction->setObjectName("Hide scripting options");
        notShowScriptAction->setCheckable( true );
        scriptingActions << notShowScriptAction;
        notShowScriptAction->setChecked(!scriptingMode);
        connect( notShowScriptAction, SIGNAL( triggered() ), SLOT( sl_changeScriptMode() ) );

        QAction * showScriptAction = new QAction( tr( "Show scripting options" ), this );
        showScriptAction->setObjectName("Show scripting options");
        showScriptAction->setCheckable( true );
        showScriptAction->setChecked(scriptingMode);
        scriptingActions << showScriptAction;
        connect( showScriptAction, SIGNAL( triggered() ), SLOT( sl_changeScriptMode() ) );
    }

    unlockAction = new QAction(tr("Unlock Scene"), this);
    unlockAction->setCheckable(true);
    unlockAction->setChecked(true);
    connect(unlockAction, SIGNAL(toggled(bool)), SLOT(sl_toggleLock(bool)));

    createScriptAction = new QAction(tr("Create element with script..."), this);
    createScriptAction->setObjectName("createScriptAction");
    createScriptAction->setIcon(QIcon(":workflow_designer/images/script.png"));
    connect(createScriptAction, SIGNAL(triggered()), SLOT(sl_createScript()));

    editScriptAction = new QAction(tr("Edit script of the element..."),this);
    editScriptAction->setObjectName("editScriptAction");
    editScriptAction->setIcon(QIcon(":workflow_designer/images/script_edit.png"));
    editScriptAction->setEnabled(false); // because user need to select actor with script to enable it
    connect(editScriptAction, SIGNAL(triggered()), SLOT(sl_editScript()));

    externalToolAction = new QAction(tr("Create element with command line tool..."), this);
    externalToolAction->setObjectName("createElementWithCommandLineTool");
    externalToolAction->setIcon(QIcon(":workflow_designer/images/external_cmd_tool.png"));
    connect(externalToolAction, SIGNAL(triggered()), SLOT(sl_externalAction()));

    editExternalToolAction = new QAction(tr("Edit configuration..."),this);
    editExternalToolAction->setObjectName("editConfiguration");
    editExternalToolAction->setIcon(QIcon(":workflow_designer/images/external_cmd_tool.png"));
    editExternalToolAction->setEnabled(false); // because user need to select actor with script to enable it
    connect(editExternalToolAction, SIGNAL(triggered()), SLOT(sl_editExternalTool()));

    appendExternalTool = new QAction(tr("Add element with command line tool..."), this);
    appendExternalTool->setObjectName("AddElementWithCommandLineTool");
    appendExternalTool->setIcon(QIcon(":workflow_designer/images/external_cmd_tool_add.png"));
    connect(appendExternalTool, SIGNAL(triggered()), SLOT(sl_appendExternalToolWorker()));

    findPrototypeAction = new QAction(this);
    findPrototypeAction->setShortcut(QKeySequence::Find);
    connect(findPrototypeAction, SIGNAL(triggered()), SLOT(sl_findPrototype()));
    this->addAction(findPrototypeAction);

    foreach(QAction *action, debugActions) {
        action->setVisible(false);
    }

    scaleComboBox = scaleCombo(this);
}

void WorkflowView::sl_findPrototype(){
    tabs->currentWidget()->setFocus();
    CHECK(tabs->currentWidget() == palette, );

    static const int MIN_SIZE_FIND = 260;
    QList<int> sizes = splitter->sizes();
    int idx = splitter->indexOf(tabs);
    CHECK(idx >= 0 && idx < sizes.size(),);
    if(sizes.at(idx) < MIN_SIZE_FIND / 2){
        sizes[idx] = MIN_SIZE_FIND;
        splitter->setSizes(sizes);
    }
}

void WorkflowView::sl_createScript() {
    QObjectScopedPointer<CreateScriptElementDialog> dlg = new CreateScriptElementDialog(this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        QList<DataTypePtr > input = dlg->getInput();
        QList<DataTypePtr > output = dlg->getOutput();
        QList<Attribute*> attrs = dlg->getAttributes();
        QString name = dlg->getName();
        QString desc = dlg->getDescription();
        if(LocalWorkflow::ScriptWorkerFactory::init(input, output, attrs, name, desc, dlg->getActorFilePath())) {
            ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(LocalWorkflow::ScriptWorkerFactory::ACTOR_ID + name);
            QRectF rect = scene->sceneRect();
            addProcess(createActor(proto, QVariantMap()), rect.center());
        }
    }
}

void WorkflowView::sl_externalAction() {
    QObjectScopedPointer<CreateExternalProcessDialog> dlg = new CreateExternalProcessDialog(this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        ExternalProcessConfig *cfg = dlg->config();
        if(LocalWorkflow::ExternalProcessWorkerFactory::init(cfg)) {
            ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(cfg->name);
            QRectF rect = scene->sceneRect();
            addProcess(createActor(proto, QVariantMap()), rect.center());
        }
    }
}

namespace {
    QString copyIntoUgene(const QString &url, U2OpStatus &os) {
        QDir dir(WorkflowSettings::getExternalToolDirectory());
        if (!dir.exists()) {
            bool created = dir.mkpath(dir.absolutePath());
            if (!created) {
                os.setError(QObject::tr("Can not create the folder: ") + dir.absolutePath());
                return "";
            }
        }
        QString filePath = dir.absolutePath() + "/" + QFileInfo(url).fileName();
        if (QFile::exists(filePath)) {
            os.setError(QObject::tr("The file '%1' already exists").arg(filePath));
            return "";
        }
        bool copied = QFile::copy(url, filePath);
        if (!copied) {
            os.setError(QObject::tr("Can not copy the file here: ") + filePath);
            return "";
        }
        return filePath;
    }
}

void WorkflowView::sl_appendExternalToolWorker() {
    QString filter = DialogUtils::prepareFileFilter(WorkflowUtils::tr("UGENE workflow element"), QStringList() << "etc", true);
    QString url = U2FileDialog::getOpenFileName(this, tr("Add element"), QString(), filter);
    if (!url.isEmpty()) {
        IOAdapter *io = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(GUrl(url)))->createIOAdapter();
        if(!io->open(url, IOAdapterMode_Read)) {
            coreLog.error(tr("Can't load element."));
            return;
        }
        QByteArray data;
        data.resize(MAX_FILE_SIZE);
        data.fill(0);
        io->readBlock(data.data(), MAX_FILE_SIZE);
        io->close();

        QScopedPointer<ExternalProcessConfig> cfg(HRSchemaSerializer::string2Actor(data.data()));
        if(cfg.data()) {
            if (WorkflowEnv::getProtoRegistry()->getProto(cfg->name)) {
                coreLog.error("Element with this name already exists");
            } else {
                U2OpStatus2Log os;
                QString internalUrl = copyIntoUgene(url, os);
                CHECK_OP(os, );
                cfg->filePath = internalUrl;
                LocalWorkflow::ExternalProcessWorkerFactory::init(cfg.data());
                ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(cfg->name);
                QRectF rect = scene->sceneRect();
                addProcess(createActor(proto, QVariantMap()), rect.center());
                cfg.take();
            }
        } else {
            coreLog.error(tr("Can't load element."));
        }
    }
}

void WorkflowView::sl_editScript() {
    QList<Actor *> selectedActors = scene->getSelectedActors();
    if(selectedActors.size() == 1) {
        Actor *scriptActor = selectedActors.first();
        AttributeScript *script = scriptActor->getScript();
        if(script!= NULL) {
            QObjectScopedPointer<ScriptEditorDialog> scriptDlg = new ScriptEditorDialog(this, AttributeScriptDelegate::createScriptHeader(*script), script->getScriptText());
            scriptDlg->exec();
            CHECK(!scriptDlg.isNull(), );

            if (scriptDlg->result() == QDialog::Accepted) {
                script->setScriptText(scriptDlg->getScriptText());
                scriptActor->setScript(script);
            }
        }
    }
}

void WorkflowView::sl_editExternalTool() {
    QList<Actor *> selectedActors = scene->getSelectedActors();
    if (selectedActors.size() == 1) {
        ActorPrototype *proto = selectedActors.first()->getProto();

        ExternalProcessConfig *oldCfg = WorkflowEnv::getExternalCfgRegistry()->getConfigByName(proto->getId());
        ExternalProcessConfig *cfg = new ExternalProcessConfig(*oldCfg);
        QObjectScopedPointer<CreateExternalProcessDialog> dlg = new CreateExternalProcessDialog(this, cfg, true);
        dlg->exec();
        CHECK(!dlg.isNull(), );

        if(dlg->result() == QDialog::Accepted) {
            cfg = dlg->config();

            if (!(*oldCfg == *cfg)) {
                if (oldCfg->name != cfg->name) {
                    if (!QFile::remove(proto->getFilePath())) {
                        uiLog.error(tr("Can't remove element %1").arg(proto->getDisplayName()));
                    }
                }
                sl_protoDeleted(proto->getId());
                WorkflowEnv::getProtoRegistry()->unregisterProto(proto->getId());
                delete proto;

                LocalWorkflow::ExternalProcessWorkerFactory::init(cfg);
            }
            WorkflowEnv::getExternalCfgRegistry()->unregisterConfig(oldCfg->name);
            WorkflowEnv::getExternalCfgRegistry()->registerExternalTool(cfg);
            scene->sl_updateDocs();
        }
    }
}

void WorkflowView::sl_protoDeleted(const QString &id) {
    QList<WorkflowProcessItem*> deleteList;
    foreach(QGraphicsItem *i, scene->items()) {
        if(i->type() == WorkflowProcessItemType) {
            WorkflowProcessItem *wItem = static_cast<WorkflowProcessItem *>(i);
            if(wItem->getProcess()->getProto()->getId() == id) {
                deleteList << wItem;
            }
        }
    }

    foreach(WorkflowProcessItem *item, deleteList) {
        removeProcessItem(item);
    }
    scene->update();
}

void WorkflowView::sl_protoListModified() {
    CHECK(NULL != elementsMenu, );
    palette->createMenu(elementsMenu);
}

void WorkflowView::addProcess(Actor *proc, const QPointF &pos) {
    schema->addProcess(proc);
    removeEstimations();

    WorkflowProcessItem *it = new WorkflowProcessItem(proc);
    it->setPos(pos);
    scene->addItem(it);
    scene->setModified();

    ConfigurationEditor *editor = proc->getEditor();
    if (NULL != editor) {
        connect(editor, SIGNAL(si_configurationChanged()), scene, SIGNAL(configurationChanged()));
    }
    GrouperEditor *g = dynamic_cast<GrouperEditor*>(editor);
    MarkerEditor *m = dynamic_cast<MarkerEditor*>(editor);
    if (NULL != g || NULL != m) {
        connect(editor, SIGNAL(si_configurationChanged()), scene, SLOT(sl_refreshBindings()));
    }

    sl_procItemAdded();
    update();
}

void WorkflowView::removeProcessItem(WorkflowProcessItem *item) {
    CHECK(NULL != item, );
    Actor *actor = item->getProcess();
    scene->removeItem(item);
    delete item;

    scene->setModified();
    schema->removeProcess(actor);
    meta.removeActorMeta(actor->getId());
    delete actor;

    removeWizards();
    removeEstimations();
}

void WorkflowView::removeWizards() {
    qDeleteAll(schema->takeWizards());
    sl_updateUi();
}

void WorkflowView::removeEstimations() {
    meta.estimationsCode.clear();
    sl_updateUi();
}

void WorkflowView::removeBusItem(WorkflowBusItem *item) {
    Link *link = item->getBus();
    scene->removeItem(item);
    delete item;
    removeEstimations();
    scene->setModified();
    onBusRemoved(link);
}

void WorkflowView::onBusRemoved(Link *link) {
    if (!sceneRecreation) {
        schema->removeFlow(link);
        schema->update();
    }
}

void WorkflowView::sl_toggleLock(bool b) {
    running = !b;
    if (sender() != unlockAction) {
        unlockAction->setChecked(!running);
        breakpointView->setEnabled(b);
        toggleDebugActionsState(!b);
        investigationWidgets->deleteBusInvestigations();
        investigationWidgets->resetInvestigations();
        return;
    }

    if (!running) {
        scene->setRunner(NULL);
    }

    newAction->setEnabled(!running);
    loadAction->setEnabled(!running);
    deleteAction->setEnabled(!running);
    deleteShortcut->setEnabled(!running);
    selectAction->setEnabled(!running);
    copyAction->setEnabled(!running);
    pasteAction->setEnabled(!running);
    cutAction->setEnabled(!running);

    stopAction->setEnabled(running);
    runAction->setEnabled(!running);
    validateAction->setEnabled(!running);
    estimateAction->setEnabled(!running);
    configureParameterAliasesAction->setEnabled(!running);
    configurePortAliasesAction->setEnabled(!running);
    importSchemaToElement->setEnabled(!running);

    propertyEditor->setEnabled(!running);
    propertyEditor->setSpecialPanelEnabled(!running);
    palette->setEnabled(!running);
    toggleDebugActionsState(!b);
    breakpointView->setEnabled(b);
    samples->setEnabled(!running);

    setupActions();
    scene->setLocked(running);
    scene->update();
}

void WorkflowView::sl_setStyle() {
    StyleId s = qobject_cast<QAction* >(sender())->data().value<StyleId>();
    QList<QGraphicsItem*> lst = scene->selectedItems();
    if (lst.isEmpty()) {
        lst = scene->items();
    }
    foreach(QGraphicsItem* it, lst) {
        switch (it->type()) {
            case WorkflowProcessItemType:
            case WorkflowPortItemType:
            case WorkflowBusItemType:
            (static_cast<StyledItem*>(it))->setStyle(s);
        }
    }
    scene->update();
}

void WorkflowView::sl_changeScriptMode() {
    QAction *a = qobject_cast<QAction*>(sender());
    if(a != NULL) {
        if(a == scriptingActions[0]){
            scriptingMode = false;
        } else if(a == scriptingActions[1]) {
            scriptingMode = true;
        }
    } // else invoked from constructor

    scriptingActions[0]->setChecked(!scriptingMode);
    scriptingActions[1]->setChecked(scriptingMode);
    propertyEditor->changeScriptMode(scriptingMode);
}

void WorkflowView::sl_toggleStyle() {
    foreach(QGraphicsItem* it, scene->selectedItems()) {
        StyleId s = (static_cast<StyledItem*>(it))->getStyle();
        if (s == ItemStyles::SIMPLE) {
            s = ItemStyles::EXTENDED;
        } else {
            s = ItemStyles::SIMPLE;
        }
        (static_cast<StyledItem*>(it))->setStyle(s);
    }
    scene->update();
}

void WorkflowView::sl_refreshActorDocs() {
    foreach(QGraphicsItem* it, scene->items()) {
        if (it->type() == WorkflowProcessItemType) {
            Actor* a = qgraphicsitem_cast<WorkflowProcessItem*>(it)->getProcess();
            a->getDescription()->update(a->getValues());
        }
    }
}

void WorkflowView::setupMDIToolbar(QToolBar* tb) {
    tb->addAction(newAction);
    tb->addAction(loadAction);
    tb->addAction(saveAction);
    tb->addAction(saveAsAction);
    loadSep = tb->addSeparator();
    tb->addAction(showWizard);
    tb->addAction(validateAction);
    tb->addAction(estimateAction);
    tb->addAction(runAction);
    tb->addAction(pauseAction);
    tb->addAction(toggleBreakpointAction);
    tb->addAction(toggleBreakpointManager);
    tb->addAction(nextStepAction);
    tb->addAction(tickReadyAction);
    tb->addAction(stopAction);
    runSep = tb->addSeparator();
    tb->addAction(configureParameterAliasesAction);
    confSep = tb->addSeparator();
    tb->addAction(createScriptAction);
    tb->addAction(editScriptAction);
    scriptSep = tb->addSeparator();
    tb->addAction(externalToolAction);
    tb->addAction(appendExternalTool);
    extSep = tb->addSeparator();
    tb->addAction(copyAction);
    tb->addAction(pasteAction);
    pasteAction->setEnabled(!lastPaste.isEmpty());
    tb->addAction(cutAction);
    tb->addAction(deleteAction);
    editSep = tb->addSeparator();
    scaleAction = tb->addWidget(scaleComboBox);
    scaleSep = tb->addSeparator();
    styleAction = tb->addWidget(styleMenu(this, styleActions));
    scriptAction = tb->addWidget(scriptMenu(this, scriptingActions));
    tb->addAction(dmAction);

    addToggleDashboardAction(tb, toggleDashboard);

    sl_dashboardCountChanged();
    setupActions();
}

void WorkflowView::setupActions() {
    bool dashboard = tabView->isVisible();
    bool editMode = !running && !dashboard;

    newAction->setVisible(!dashboard);
    loadAction->setVisible(!dashboard);
    saveAction->setVisible(!dashboard);
    saveAsAction->setVisible(!dashboard);
    loadSep->setVisible(!dashboard);

    showWizard->setVisible(editMode && !schema->getWizards().isEmpty());
    validateAction->setVisible(editMode);
    estimateAction->setVisible(editMode && !meta.estimationsCode.isEmpty());
    stopAction->setVisible(running);
    runSep->setVisible(editMode);

    configureParameterAliasesAction->setVisible(editMode);
    confSep->setVisible(editMode);

    createScriptAction->setVisible(editMode);
    editScriptAction->setVisible(editMode);
    scriptSep->setVisible(editMode);

    externalToolAction->setVisible(editMode);
    appendExternalTool->setVisible(editMode);
    extSep->setVisible(editMode);

    copyAction->setVisible(editMode);
    pasteAction->setVisible(editMode);
    cutAction->setVisible(editMode);
    deleteAction->setVisible(editMode);
    editSep->setVisible(editMode);

    scaleAction->setVisible(!dashboard);
    scaleSep->setVisible(!dashboard);

    styleAction->setVisible(!dashboard);
    scriptAction->setVisible(editMode);
}

void WorkflowView::setupViewMenu(QMenu* m) {
    elementsMenu = palette->createMenu(tr("Add element"));
    m->addMenu(elementsMenu);
    m->addAction(copyAction);
    m->addAction(pasteAction);
    pasteAction->setEnabled(!lastPaste.isEmpty());
    m->addAction(cutAction);
    m->addAction(deleteAction);
    m->addAction(selectAction);
    m->addSeparator();
    m->addAction(newAction);
    m->addAction(loadAction);
    m->addAction(saveAction);
    m->addAction(saveAsAction);
    m->addAction(exportAction);
    m->addSeparator();
    m->addAction(validateAction);
    m->addAction(estimateAction);
    m->addAction(runAction);
    m->addAction(stopAction);
    m->addSeparator();
    m->addAction(configureParameterAliasesAction);
    m->addAction(createGalaxyConfigAction);
    m->addAction(configurePortAliasesAction);
    m->addAction(importSchemaToElement);
    m->addSeparator();
    m->addSeparator();
    m->addAction(createScriptAction);
    m->addAction(editScriptAction);
    m->addSeparator();
    m->addAction(externalToolAction);
    m->addAction(appendExternalTool);
    m->addSeparator();

    QMenu* ttMenu = new QMenu(tr("Element style"));
    foreach(QAction* a, styleActions) {
        ttMenu->addAction(a);
    }
    m->addMenu(ttMenu);

    QMenu* scriptMenu = new QMenu(tr("Scripting mode"));
    foreach(QAction* a, scriptingActions) {
        scriptMenu->addAction(a);
    }
    m->addMenu(scriptMenu);

    if (!unlockAction->isChecked()) {
        m->addSeparator();
        m->addAction(unlockAction);
    }
    m->addSeparator();
    m->addAction(dmAction);
}

void WorkflowView::setupContextMenu(QMenu* m) {
    if(!debugInfo->isPaused()) {
        if (!unlockAction->isChecked()) {
            return;
        }

        if(!lastPaste.isEmpty()) {
            m->addAction(pasteAction);
        }
        QList<QGraphicsItem*> sel = scene->selectedItems();
        if (!sel.isEmpty()) {
            if(!((sel.size() == 1 && sel.first()->type() == WorkflowBusItemType) || sel.first()->type() == WorkflowPortItemType)) {
                m->addAction(copyAction);
                m->addAction(cutAction);
            }
            if(!(sel.size() == 1 && sel.first()->type() == WorkflowPortItemType)) {
                m->addAction(deleteAction);
            }
            m->addSeparator();
            if (sel.size() == 1 && sel.first()->type() == WorkflowProcessItemType) {
                WorkflowProcessItem* wit = qgraphicsitem_cast<WorkflowProcessItem*>(sel.first());
                Actor *scriptActor = wit->getProcess();
                AttributeScript *script = scriptActor->getScript();
                if(script) {
                    m->addAction(editScriptAction);
                }

                ActorPrototype *p = scriptActor->getProto();
                if (p->isExternalTool()) {
                    m->addAction(editExternalToolAction);
                }

                m->addSeparator();

                QMenu* itMenu = new QMenu(tr("Element properties"));
                foreach(QAction* a, wit->getContextMenuActions()) {
                    itMenu->addAction(a);
                }
                m->addMenu(itMenu);
            }
            if(!(sel.size() == 1 && (sel.first()->type() == WorkflowBusItemType || sel.first()->type() == WorkflowPortItemType))) {
                QMenu* ttMenu = new QMenu(tr("Element style"));
                foreach(QAction* a, styleActions) {
                    ttMenu->addAction(a);
                }
                m->addMenu(ttMenu);
            }
        }
        m->addSeparator();

        m->addAction(selectAction);
        m->addMenu(palette->createMenu(tr("Add element")));
    }

    foreach(QGraphicsItem *item, scene->selectedItems()) {
        if(WorkflowProcessItemType == item->type()) {
            m->addAction(toggleBreakpointAction);
            break;
        }
    }
}

void WorkflowView::sl_pickInfo(QListWidgetItem* info) {
    ActorId id = info->data(ACTOR_REF).value<ActorId>();
    foreach(QGraphicsItem* it, scene->items()) {
        if (it->type() == WorkflowProcessItemType)
        {
            WorkflowProcessItem* proc = static_cast<WorkflowProcessItem*>(it);
            if (proc->getProcess()->getId() != id) {
                continue;
            }
            scene->clearSelection();
            QString pid = info->data(PORT_REF).toString();
            WorkflowPortItem* port = proc->getPort(pid);
            if (port) {
                port->setSelected(true);
            } else {
                proc->setSelected(true);
            }
            return;
        }
    }
}

bool WorkflowView::sl_validate(bool notify) {
    if(schema->getProcesses().isEmpty()) {
        QMessageBox::warning(this, tr("Empty workflow!"), tr("Nothing to run: empty workflow"));
        return false;
    }

    propertyEditor->commit();
    infoList->clear();
    QList<QListWidgetItem*> lst;
    bool good = WorkflowUtils::validate(*schema, lst);

    if (lst.count() != 0) {
        foreach(QListWidgetItem* wi, lst) {
            infoList->addItem(wi);
        }
        bottomTabs->show();
        bottomTabs->setCurrentWidget(infoList->parentWidget());
        infoList->parentWidget()->show();
        QList<int> s = infoSplitter->sizes();
        if (s[s.size() - 1] == 0) {
            s[s.size() - 1] = qMin(infoList->sizeHint().height(), 300);
            infoSplitter->setSizes(s);
        }
    } else if(bottomTabs->currentWidget() == infoList->parentWidget()) {
        bottomTabs->hide();
    }
    if (!good) {
        QMessageBox::warning(this, tr("Workflow cannot be executed"),
            tr("Please fix issues listed in the error list (located under workflow)."));
    } else {
        if (notify) {
            QString message = tr("Workflow is valid. \n");
            if (lst.isEmpty()) {
                message += tr("Well done!");
            } else {
                message += tr("There are non-critical warnings.");
            }
            QMessageBox::information(this, tr("Workflow is valid"), message);
        }
    }
    return good;
}

void WorkflowView::sl_estimate() {
    CHECK(sl_validate(false /*don't notify*/), );
    SAFE_POINT(!meta.estimationsCode.isEmpty(), "No estimation code", );
    estimateAction->setEnabled(false);

    SchemaEstimationTask *t = new SchemaEstimationTask(schema, &meta);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_estimationTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void WorkflowView::sl_estimationTaskFinished() {
    SchemaEstimationTask *t = dynamic_cast<SchemaEstimationTask*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished(), );
    estimateAction->setEnabled(true);
    CHECK(!t->hasError(), );
    QMessageBox *d = EstimationReporter::createTimeMessage(t->result());
    QPushButton *rb = d->addButton(QObject::tr("Run workflow"), QMessageBox::AcceptRole);
    rb->setObjectName("Run workflow");
    connect(rb, SIGNAL(clicked()), SLOT(sl_launch()));
    d->setParent(this);
    d->setWindowModality(Qt::ApplicationModal);
    d->show();
}

void WorkflowView::localHostLaunch() {
    if (!sl_validate(false)) {
        return;
    }

    if (schema->getDomain().isEmpty()) {
        // TODO: user choice
        schema->setDomain(WorkflowEnv::getDomainRegistry()->getAllIds().value(0));
    }

    if (meta.isSample()) {
        GRUNTIME_NAMED_COUNTER(cvar, tvar, meta.name, "WDSample:run");
    }

    const Schema *s = getSchema();
    debugInfo->setMessageParser( new WorkflowDebugMessageParserImpl( ) );
    WorkflowAbstractRunner * t = new WorkflowRunTask(*s, ActorMap(), debugInfo);

    t->setReportingEnabled(true);
    if (WorkflowSettings::monitorRun()) {
        commitWarningsToMonitor(t);
        unlockAction->setChecked(false);
        scene->setRunner(t);
        connect(t, SIGNAL(si_ticked()), scene, SLOT(update()));
        TaskSignalMapper *signalMapper = new TaskSignalMapper(t);
        connect(signalMapper, SIGNAL(si_taskFinished(Task*)), debugInfo,
            SLOT(sl_executionFinished()));
        connect(signalMapper, SIGNAL(si_taskFinished(Task*)), SLOT(sl_toggleLock()));
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    foreach (WorkflowMonitor *m, t->getMonitors()) {
        m->setSaveSchema(meta);
        tabView->addDashboard(m, meta.name);
        showDashboards();
    }
}

void WorkflowView::sl_launch() {
    if(!debugInfo->isPaused()) {
        localHostLaunch();
        if(NULL != scene->getRunner()) {
            stopAction->setEnabled(true);
            pauseAction->setEnabled(true);
            propertyEditor->setEnabled(false);
            toggleDebugActionsState(true);
        }
    }
}

void WorkflowView::sl_pause(bool isPause) {
    pauseAction->setEnabled(!isPause);
    runAction->setEnabled(isPause);
    nextStepAction->setEnabled(isPause);
    propertyEditor->setEnabled(isPause);
    scene->setLocked(!isPause);
    breakpointView->setEnabled(isPause);
    investigationWidgets->setInvestigationWidgetsVisible(isPause);
    WorkflowAbstractRunner *runningWorkflow = scene->getRunner();
    if (NULL != runningWorkflow && runningWorkflow->isRunning()) {
        foreach (WorkflowMonitor *m, runningWorkflow->getMonitors()) {
            if (isPause) {
                m->pause();
            } else {
                m->resume();
            }
        }
    }
    if ( isPause && tabView->isVisible() ) {
        hideDashboards();
    }
}

void WorkflowView::sl_stop() {
    Task *runningWorkflow = scene->getRunner();
    if (NULL != runningWorkflow) {
        runningWorkflow->cancel();
    }
    investigationWidgets->resetInvestigations();
}

void WorkflowView::toggleDebugActionsState(bool enable) {
    if(WorkflowSettings::isDebuggerEnabled()) {
        foreach(QAction *action, debugActions) {
            action->setVisible(enable);
        }
    }
}

void WorkflowView::propagateBreakpointToSceneItem(ActorId actor) {
    WorkflowProcessItem* processItem = findItemById(actor);
    Q_ASSERT(processItem->isBreakpointInserted());
    processItem->highlightItem();
}

void WorkflowView::sl_breakpointAdded(const ActorId &actor) {
    changeBreakpointState(actor, true);
}

void WorkflowView::sl_breakpointRemoved(const ActorId &actor) {
    changeBreakpointState(actor, false);
}

void WorkflowView::sl_breakpointEnabled(const ActorId &actor) {
    changeBreakpointState(actor, false, true);
}

void WorkflowView::sl_breakpointDisabled(const ActorId &actor) {
    changeBreakpointState(actor, false, true);
}

void WorkflowView::changeBreakpointState(const ActorId &actor, bool isBreakpointBeingAdded,
    bool isBreakpointStateBeingChanged)
{
    WorkflowProcessItem* processItem = findItemById(actor);
    Q_ASSERT(NULL != processItem);

    if(processItem->isBreakpointInserted()) {
        if(!isBreakpointBeingAdded) {
            if(!isBreakpointStateBeingChanged) {
                processItem->toggleBreakpoint();
            } else {
                processItem->toggleBreakpointState();
            }
        }
    } else {
        if(isBreakpointBeingAdded) {
            if(!isBreakpointStateBeingChanged){
                processItem->toggleBreakpoint();
            } else {
                Q_ASSERT(false);
            }
        }
    }
    scene->update();
}

void WorkflowView::sl_toggleBreakpointManager() {
    if(!breakpointView->isVisible()) {
        bottomTabs->setVisible(true);
        bottomTabs->setCurrentWidget(breakpointView);
    } else {
        bottomTabs->hide();
    }
}

void WorkflowView::sl_highlightingRequested(const ActorId &actor) {
    findItemById(actor)->highlightItem();
}

void WorkflowView::sl_processOneMessage() {
    Q_ASSERT(debugInfo->isPaused());
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    Q_ASSERT(1 == selectedItems.size());
    WorkflowProcessItem *processItem = qgraphicsitem_cast<WorkflowProcessItem *>(selectedItems.first());
    debugInfo->requestForSingleStep(processItem->getProcess()->getId());
}

void WorkflowView::sl_convertMessages2Documents(const Workflow::Link *bus,
    const QString &messageType, int messageNumber)
{
    debugInfo->convertMessagesToDocuments(bus, messageType, messageNumber, meta.name);
}

WorkflowProcessItem *WorkflowView::findItemById(ActorId actor) const {
    foreach(QGraphicsItem *item, scene->items()) {
        if(WorkflowProcessItemType == item->type()) {
            WorkflowProcessItem *processItem = qgraphicsitem_cast<WorkflowProcessItem*>(item);
            Q_ASSERT(NULL != processItem);
            if(actor == processItem->getProcess()->getId()) {
                return processItem;
            }
        }
    }
    return NULL;
}

void WorkflowView::paintEvent(QPaintEvent *event) {
    const bool isWorkflowRunning = ( NULL != scene->getRunner( ) );
    const bool isDebuggerEnabled = WorkflowSettings::isDebuggerEnabled( );
    if ( isDebuggerEnabled && ABSENT_WIDGET_TAB_NUMBER == bottomTabs->indexOf( breakpointView ) ) {
        bottomTabs->addTab( breakpointView, BREAKPOINT_MANAGER_LABEL );
    } else if ( !isDebuggerEnabled
        && ABSENT_WIDGET_TAB_NUMBER != bottomTabs->indexOf( breakpointView ) )
    {
        breakpointView->sl_deleteAllBreakpoints();
        bottomTabs->removeTab( bottomTabs->indexOf( breakpointView ) );
    }
    foreach ( QAction *action, debugActions ) {
        action->setVisible( WorkflowSettings::isDebuggerEnabled( ) && isWorkflowRunning );
    }
    toggleBreakpointAction->setVisible( isDebuggerEnabled );
    toggleBreakpointManager->setVisible( isDebuggerEnabled );

    if (isWorkflowRunning) {
        if(debugInfo->isPaused()) {
            sl_onSelectionChanged();
        } else {
            tickReadyAction->setEnabled(false);
        }
    }
    MWMDIWindow::paintEvent(event);
}

void WorkflowView::sl_configureParameterAliases() {
    QObjectScopedPointer<SchemaAliasesConfigurationDialogImpl> dlg = new SchemaAliasesConfigurationDialogImpl(*schema, this );
    int ret = QDialog::Accepted;
    do {
        ret = dlg->exec();
        CHECK(!dlg.isNull(), );
        if( ret == QDialog::Accepted ) {
            if(!dlg->validateModel()) {
                QMessageBox::critical( this, tr("Bad input!"), tr("Aliases for workflow parameters should be different!") );
                continue;
            }
            // clear aliases before inserting new
            foreach (Actor * actor, schema->getProcesses()) {
                actor->getParamAliases().clear();
            }
            SchemaAliasesCfgDlgModel model = dlg->getModel();
            foreach(const ActorId & id, model.aliases.keys()) {
                foreach(const Descriptor & d, model.aliases.value(id).keys()) {
                    Actor * actor = schema->actorById(id);
                    assert(actor != NULL);
                    QString alias = model.aliases.value(id).value(d);
                    assert(!alias.isEmpty());
                    actor->getParamAliases().insert(d.getId(), alias);
                    QString help = model.help.value(id).value(d);
                    if( !help.isEmpty() ) {
                        actor->getAliasHelp().insert(alias, help);
                    }
                }
            }
            break;
        } else if( ret == QDialog::Rejected ) {
            break;
        } else {
            assert(false);
        }
    } while( ret == QDialog::Accepted );
}

void WorkflowView::sl_createGalaxyConfig() {
    bool schemeContainsAliases = schema->hasParamAliases();
    if( !schemeContainsAliases ) {
        QMessageBox::critical( this, tr("Bad input!"), tr("Workflow does not contain any parameter aliases") );
        return;
    }
    if( meta.url.isEmpty() ) {
        return;
    }

    QObjectScopedPointer<GalaxyConfigConfigurationDialogImpl> dlg = new GalaxyConfigConfigurationDialogImpl( meta.url, this );
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if ( QDialog::Accepted == dlg->result() ) {
        bool created = dlg->createGalaxyConfigTask();
        if( !created ) {
            QMessageBox::critical( this, tr("Internal error!"), tr("Can not create Galaxy config") );
            return;
        }
    }
}

void WorkflowView::sl_configurePortAliases() {
    QObjectScopedPointer<PortAliasesConfigurationDialog> dlg = new PortAliasesConfigurationDialog(*schema, this);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dlg->result()) {
        PortAliasesCfgDlgModel model = dlg->getModel();

        QList<PortAlias> portAliases;
        foreach (Port *port, model.ports.keys()) {
            PortAlias portAlias(port, model.ports.value(port).first, model.ports.value(port).second);

            foreach (Descriptor slotDescr, model.aliases.value(port).keys()) {
                QString actorId;
                QString slotId;
                {
                    if (port->isInput()) {
                        actorId = port->owner()->getId();
                        slotId = slotDescr.getId();
                    } else {
                        QStringList tokens = slotDescr.getId().split(':');
                        assert(2 == tokens.size());
                        actorId = tokens[0];
                        slotId = tokens[1];
                    }
                }

                Port *sourcePort = NULL;
                foreach (Port *p, schema->actorById(actorId)->getPorts()) {
                    DataTypePtr dt = p->Port::getType();
                    QList<Descriptor> descs = dt->getAllDescriptors();
                    if(descs.contains(slotId)) {
                        sourcePort = p;
                        break;
                    }
                }
                assert(NULL != sourcePort);

                portAlias.addSlot(sourcePort, slotId, model.aliases.value(port).value(slotDescr));
            }
            portAliases.append(portAlias);
        }

        schema->setPortAliases(portAliases);
    }
}

void WorkflowView::sl_importSchemaToElement() {
    QString error;
    if (!schema->getWizards().isEmpty()) {
        error = WorkflowView::tr("The workflow contains a wizard. Sorry, but current version of "
            "UGENE doesn't support of wizards in the includes.");
        QMessageBox::critical(this, tr("Error"), error);
    } else if (WorkflowUtils::validateSchemaForIncluding(*schema, error)) {
        QObjectScopedPointer<ImportSchemaDialog> d = new ImportSchemaDialog(this);
        d->exec();
        CHECK(!d.isNull(), );

        if (QDialog::Accepted == d->result()) {
            Schema *s = new Schema();
            U2OpStatusImpl os;
            HRSchemaSerializer::deepCopy(*schema, s, os);
            SAFE_POINT_OP(os, );
            QString typeName = d->getTypeName();

            s->setTypeName(typeName);
            QString text = HRSchemaSerializer::schema2String(*s, NULL);

            QString path = WorkflowSettings::getIncludedElementsDirectory()
                + typeName + "." + WorkflowUtils::WD_FILE_EXTENSIONS.first();
            QFile file(path);
            file.open(QIODevice::WriteOnly);
            file.write(text.toLatin1());
            file.close();

            ActorPrototype *proto = IncludedProtoFactory::getSchemaActorProto(s, typeName, path);
            WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_INCLUDES(), proto);
            WorkflowEnv::getSchemaActorsRegistry()->registerSchema(typeName, s);
        }
    } else {
        QMessageBox::critical(this, tr("Error"), error);
    }
}

void WorkflowView::sl_selectPrototype(Workflow::ActorPrototype* p, bool putToScene) {
    propertyEditor->setEditable(!p);
    scene->clearSelection();
    currentProto = p;

    propertyEditor->reset();
    if (!p) {
        scene->views().at(0)->unsetCursor();
        propertyEditor->changeScriptMode(scriptingMode);
    } else {
        delete currentActor;
        currentActor = createActor(p, QVariantMap());
        if (putToScene) {
            addProcess(currentActor, scene->getLastMousePressPoint());
        } else {
            propertyEditor->setDescriptor(p, tr("Drag an element to the scene to add it to the workflow."));
            scene->views().at(0)->setCursor(Qt::CrossCursor);
        }
    }
}

void WorkflowView::sl_copyItems() {
    QList<WorkflowProcessItem*> procs;
    foreach(QGraphicsItem* item, scene->selectedItems()) {
        if (item->type() == WorkflowProcessItemType) {
            procs << qgraphicsitem_cast<WorkflowProcessItem*>(item);
        }
    }
    if (procs.isEmpty()) {
        return;
    }

    QList<Actor*> actors = scene->getSelectedActors();
    Metadata actorMeta = getMeta(procs);
    lastPaste = HRSchemaSerializer::items2String(actors, &actorMeta);
    pasteAction->setEnabled(true);
    QApplication::clipboard()->setText(lastPaste);
    pasteCount = 0;
}

void WorkflowView::sl_cutItems() {
    sl_copyItems();
    scene->sl_deleteItem();
}

void WorkflowView::sl_pasteSample(const QString& s) {
    tabs->setCurrentIndex(ElementsTab);
    if (scene->items().isEmpty()) {
        // fixing bug with pasting same schema 2 times
        {
            lastPaste.clear();
        }
        sl_pasteItems(s, true);
        sl_updateTitle();
        sl_updateUi();
        scene->connectConfigurationEditors();
        scene->sl_deselectAll();
        scene->update();
        rescale();
        sl_refreshActorDocs();
        meta.setSampleMark(true);
        GRUNTIME_NAMED_COUNTER(c, t, meta.name, "WDSample:open");
        checkAutoRunWizard();
    } else {
        breakpointView->clear();
        scene->clearScene();
        schema->reset();
        sl_pasteSample(s);
    }
}

static QMap<ActorId, ActorId> getUniquePastedActorIds(const QList<Actor*> &pasted, const QList<Actor*> &origin) {
    QMap<ActorId, ActorId> result;
    QStringList uniqueIds;
    foreach (Actor *a, origin) {
        uniqueIds << aid2str(a->getId());
    }
    foreach (Actor *a, pasted) {
        QString uniqId = WorkflowUtils::createUniqueString(aid2str(a->getId()), "-", uniqueIds);
        uniqueIds << uniqId;
        ActorId newId = str2aid(uniqId);
        if (newId != a->getId()) {
            result[a->getId()] = newId;
        }
    }
    return result;
}

static void renamePastedSchemaActors(Schema &pasted, Metadata &meta, Schema *origin) {
    QMap<ActorId, ActorId> mapping = getUniquePastedActorIds(pasted.getProcesses(), origin->getProcesses());
    foreach (const ActorId &id, mapping.keys()) {
        pasted.renameProcess(id, mapping[id]);
    }
    meta.renameActors(mapping);
}

void WorkflowView::sl_pasteItems(const QString &s, bool updateSchemaInfo) {
    QString tmp = s.isNull() ? QApplication::clipboard()->text() : s;
    if (tmp == lastPaste) {
        ++pasteCount;
    } else {
        pasteCount = 0;
        lastPaste = tmp;
    }
    QByteArray lpt = lastPaste.toLatin1();
    DocumentFormat* wf = AppContext::getDocumentFormatRegistry()->getFormatById(WorkflowDocFormat::FORMAT_ID);
    if (wf->checkRawData(lpt).score != FormatDetection_Matched) {
        return;
    }
    disconnect(scene, SIGNAL(selectionChanged()), this, SLOT(sl_editItem()));
    scene->clearSelection();
    connect(scene, SIGNAL(selectionChanged()), SLOT(sl_editItem()));

    Schema pastedS;
    pastedS.setDeepCopyFlag(true);
    Metadata pastedM;
    QString msg = HRSchemaSerializer::string2Schema(lastPaste, &pastedS, &pastedM);
    if (!msg.isEmpty()) {
        uiLog.error("Paste issues: " + msg);
        return;
    }
    renamePastedSchemaActors(pastedS, pastedM, schema);
    if (schema->getProcesses().isEmpty()) {
        schema->setWizards(pastedS.takeWizards());
    }
    schema->merge(pastedS);
    updateMeta();
    meta.mergeVisual(pastedM);
    if (updateSchemaInfo) {
        meta.name = pastedM.name;
        meta.comment = pastedM.comment;
        meta.scalePercent = pastedM.scalePercent;
        meta.estimationsCode = pastedM.estimationsCode;
    }
    pastedS.setDeepCopyFlag(false);
    recreateScene();
    scene->connectConfigurationEditors();

    foreach (QGraphicsItem *it, scene->items()) {
        WorkflowProcessItem *proc = qgraphicsitem_cast<WorkflowProcessItem*>(it);
        if (NULL != proc) {
            if (NULL != pastedS.actorById(proc->getProcess()->getId())) {
                it->setSelected(true);
            }
        }
    }

    int shift = GRID_STEP*(pasteCount);
    foreach(QGraphicsItem * it, scene->selectedItems()) {
        it->moveBy(shift, shift);
    }
}

void WorkflowView::recreateScene() {
    sceneRecreation = true;
    SceneCreator sc(schema, meta);
    sc.recreateScene(scene);
    sceneRecreation = false;
}

void WorkflowView::sl_procItemAdded() {
    currentActor = NULL;
    propertyEditor->setEditable(true);
    scene->invalidate(QRectF(), QGraphicsScene::BackgroundLayer);
    if (!currentProto) {
        return;
    }

    uiLog.trace(currentProto->getDisplayName() + " added");
    palette->resetSelection();
    currentProto = NULL;
    assert(scene->views().size() == 1);
    scene->views().at(0)->unsetCursor();
}

void WorkflowView::sl_showEditor() {
    propertyEditor->show();
    QList<int> s = splitter->sizes();
    if (s.last() == 0) {
        s.last() = propertyEditor->sizeHint().width();
        splitter->setSizes(s);
    }
}

void WorkflowView::sl_editItem() {
    QList<QGraphicsItem*> list = scene->selectedItems();
    if (list.size() == 1) {
        QGraphicsItem* it = list.at(0);
        if (it->type() == WorkflowProcessItemType) {
            Actor *a = qgraphicsitem_cast<WorkflowProcessItem*>(it)->getProcess();
            propertyEditor->editActor(a);
            return;
        }
        Port* p = NULL;

        if (it->type() == WorkflowBusItemType) {
            WorkflowBusItem *busItem = qgraphicsitem_cast<WorkflowBusItem *>(it);

            if(debugInfo->isPaused()) {
                investigationWidgets->setCurrentInvestigation(busItem->getBus());
            }
            p = busItem->getInPort()->getPort();
        } else if (it->type() == WorkflowPortItemType) {
            p = qgraphicsitem_cast<WorkflowPortItem*>(it)->getPort();
        }
        if (p) {
            if (qobject_cast<IntegralBusPort*>(p))
            {
                BusPortEditor* ed = new BusPortEditor(qobject_cast<IntegralBusPort*>(p));
                ed->setParent(p);
                p->setEditor(ed);
            }
        }
        propertyEditor->editPort(p);
    } else {
        propertyEditor->reset();
    }
}

void WorkflowView::sl_onSelectionChanged() {
    QList<Actor*> actorsSelected = scene->getSelectedActors();
    const int actorsCount = actorsSelected.size();
    editScriptAction->setEnabled(actorsCount == 1 && actorsSelected.first()->getScript() != NULL);
    editExternalToolAction->setEnabled(actorsCount == 1 && actorsSelected.first()->getProto()->isExternalTool());
    toggleBreakpointAction->setEnabled(scene->items().size() != 0);

    WorkflowAbstractRunner *runner = scene->getRunner();
    if(NULL != runner && !actorsSelected.isEmpty()) {
        QList<Workflow::WorkerState> workerStates = runner->getState(actorsSelected.first());
        tickReadyAction->setEnabled(debugInfo->isPaused() && 1 == actorsCount
            && workerStates.contains(WorkerReady));
    } else {
        tickReadyAction->setEnabled(false);
    }
}

void WorkflowView::sl_exportScene() {
    propertyEditor->commit();
    QString fileName = GUrlUtils::fixFileName(meta.name);
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(sceneView->viewport(), ExportImageDialog::WD,
                                                                           fileName, ExportImageDialog::SupportScaling,  sceneView->viewport());
    dialog->exec();
}

void WorkflowView::sl_saveScene() {
    if (meta.url.isEmpty()) {
        QObjectScopedPointer<WorkflowMetaDialog> md = new WorkflowMetaDialog(this, meta);
        const int rc = md->exec();
        CHECK(!md.isNull(), );

        if (rc != QDialog::Accepted) {
            return;
        }
        meta = md->meta;
        sl_updateTitle();
    }
    propertyEditor->commit();
    Task* t = new SaveWorkflowSceneTask(getSchema(), getMeta());
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_onSceneSaved()));
}

void WorkflowView::sl_saveSceneAs() {
    QObjectScopedPointer<WorkflowMetaDialog> md = new WorkflowMetaDialog(this, meta);
    const int rc = md->exec();
    CHECK(!md.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    propertyEditor->commit();
    meta = md->meta;
    Task* t = new SaveWorkflowSceneTask(getSchema(), getMeta());
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    sl_updateTitle();
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_onSceneSaved()));
}

void WorkflowView::runWizard(Wizard *w) {
    WizardController controller(schema, w);
    QWizard *gui = controller.createGui();
    if (gui->exec() && !controller.isBroken()) {
        QString result = w->getResult(controller.getVariables());
        if (!result.isEmpty()) {
            controller.applyChanges(meta);
            loadWizardResult(result);
            return;
        }

        const bool isSample = meta.isSample();
        updateMeta();
        meta.setSampleMark(isSample);

        WizardController::ApplyResult res = controller.applyChanges(meta);
        if (WizardController::ACTORS_REPLACED == res) {
            recreateScene();
            schema->setWizards(QList<Wizard*>());
        }
        scene->sl_updateDocs();
        scene->setModified();
        propertyEditor->update();
        if (controller.isRunAfterApply()) {
            sl_launch();
        }
    } else if (schema->getProcesses().isEmpty()) {
        sl_newScene();
    }
}

void WorkflowView::loadWizardResult(const QString &result) {
    QString url = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/workflow_samples/" + result;
    if (!QFile::exists(url)) {
        coreLog.error(tr("File is not found: %1").arg(url));
        return;
    }
    breakpointView->clear();
    schema->reset();
    meta.reset();
    U2OpStatus2Log os;
    WorkflowUtils::schemaFromFile(url, schema, &meta, os);
    recreateScene();
    sl_onSceneLoaded();
    if (!schema->getWizards().isEmpty() && !schema->getWizards().first()->isAutoRun()) {
        runWizard(schema->getWizards().first());
    }
}

void WorkflowView::checkAutoRunWizard() {
    foreach (Wizard *w, schema->getWizards()) {
        if (w->isAutoRun()) {
            runWizard(w);
            break;
        }
    }
}

void WorkflowView::sl_showWizard() {
    if (schema->getWizards().size() > 0) {
        runWizard(schema->getWizards().first());
    }
}

static QIcon getToolbarIcon(const QString &srcPath) {
    QPixmap pm = QPixmap(":workflow_designer/images/" + srcPath).scaled(16, 16);
    return QIcon(pm);
}

void WorkflowView::hideDashboards() {
    toggleDashboard->setIconText("Go to Dashboard");
    toggleDashboard->setIcon(getToolbarIcon("dashboard.png"));
    toggleDashboard->setToolTip(tr("Show dashboard"));
    tabView->setVisible(false);
    splitter->setVisible(true);
    setupActions();
}

void WorkflowView::showDashboards() {
    toggleDashboard->setIconText("To Workflow Designer");
    toggleDashboard->setIcon(getToolbarIcon("wd.png"));
    toggleDashboard->setToolTip(tr("Show workflow"));
    splitter->setVisible(false);
    tabView->setVisible(true);
    setupActions();
}

void WorkflowView::setDashboardActionVisible(bool visible) {
    toggleDashboard->setVisible(visible);
}

void WorkflowView::commitWarningsToMonitor(WorkflowAbstractRunner* t) {
    for (int i = 0; i < infoList->count(); i++) {
        QListWidgetItem* warning = infoList->item(i);
        foreach (WorkflowMonitor* monitor, t->getMonitors()) {
            monitor->addError(warning->data(TEXT_REF).toString(),
                              warning->data(ACTOR_REF).toString(),
                              warning->data(TYPE_REF).toString());
        }
    }
}

void WorkflowView::sl_toggleDashboard() {
    if (tabView->isVisible()) {
        hideDashboards();
    } else {
        showDashboards();
    }
}

void WorkflowView::sl_dashboardCountChanged() {
    setDashboardActionVisible(tabView->hasDashboards());
    if (!tabView->hasDashboards()) {
        hideDashboards();
    }
}

void WorkflowView::sl_loadScene() {
    if (!confirmModified()) {
        return;
    }

    QString dir = AppContext::getSettings()->getValue(LAST_DIR, QString("")).toString();
    QString filter = DesignerUtils::getSchemaFileFilter(true, true);
    QString url;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        url = U2FileDialog::getOpenFileName(0, tr("Open workflow file"), dir, filter, 0, QFileDialog::DontUseNativeDialog);
    }else
#endif
    url = U2FileDialog::getOpenFileName(0, tr("Open workflow file"), dir, filter);
    if (!url.isEmpty()) {
        AppContext::getSettings()->setValue(LAST_DIR, QFileInfo(url).absoluteDir().absolutePath());
        sl_loadScene(url, false);
    }
}

void WorkflowView::sl_loadScene(const QString &url, bool fromDashboard) {
    CHECK(!running, );
    if (fromDashboard && !confirmModified()) {
        return;
    }
    Task* t = new LoadWorkflowSceneTask(schema, &meta, scene, url, fromDashboard); //FIXME unsynchronized meta usage
    TaskSignalMapper* m = new TaskSignalMapper(t);
    connect(m, SIGNAL(si_taskFinished(Task*)), SLOT(sl_onSceneLoaded()));
    if(LoadWorkflowTask::detectFormat(IOAdapterUtils::readFileHeader(url)) == LoadWorkflowTask::XML) {
        connect(m, SIGNAL(si_taskFinished(Task*)), SLOT(sl_xmlSchemaLoaded(Task*)));
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void WorkflowView::sl_xmlSchemaLoaded(Task* t) {
    assert(t != NULL);
    if(!t->hasError()) {
        QMessageBox::warning(this, tr("Warning!"), XML_SCHEMA_WARNING);
    } else {
        QMessageBox::warning(this, tr("Warning!"), XML_SCHEMA_APOLOGIZE);
    }
}

void WorkflowView::sl_newScene() {
    if (!confirmModified()) {
        return;
    }
    breakpointView->clear();
    bottomTabs->hide();
    scene->sl_reset();
    meta.reset();
    meta.name = tr("New workflow");
    schema->reset();
    sl_updateTitle();
    scene->setModified(false);
    rescale();
    scene->update();
    sl_updateUi();
}

void WorkflowView::sl_onSceneLoaded() {
    breakpointView->clear();
    sl_updateTitle();
    sl_updateUi();
    scene->centerView();

    scene->setModified(false);
    rescale();
    sl_refreshActorDocs();
    checkAutoRunWizard();
    hideDashboards();
    tabs->setCurrentIndex(ElementsTab);
}

void WorkflowView::sl_onSceneSaved() {
    Task *t = dynamic_cast<Task*>(sender());
    CHECK(NULL != t, );
    if (t->isFinished() && !t->hasError()) {
        scene->setModified(false);
    }
}

void WorkflowView::sl_updateTitle() {
    setWindowTitle(tr("Workflow Designer - %1").arg(meta.name));
}

void WorkflowView::sl_updateUi() {
    scene->setModified(false);
    showWizard->setVisible(!schema->getWizards().isEmpty());
    estimateAction->setVisible(!meta.estimationsCode.isEmpty());
}

void WorkflowView::saveState() {
    AppContext::getSettings()->setValue(SPLITTER_STATE, splitter->saveState());
    AppContext::getSettings()->setValue(EDITOR_STATE, propertyEditor->saveState());
    AppContext::getSettings()->setValue(PALETTE_STATE, palette->saveState());
    AppContext::getSettings()->setValue(TABS_STATE, tabs->currentIndex());
}

bool WorkflowView::onCloseEvent() {
    saveState();
    if (!confirmModified()) {
        return false;
    }
    if (go) {
        go->setView(NULL);
    }
    return true;
}

bool WorkflowView::confirmModified() {
    propertyEditor->commit();
    if (scene->isModified() && !scene->items().isEmpty()) {
        AppContext::getMainWindow()->getMDIManager()->activateWindow(this);
        int ret = QMessageBox::question(this, tr("Workflow Designer"),
            tr("The workflow has been modified.\n"
            "Do you want to save changes?"),
            QMessageBox::Save | QMessageBox::Discard
            | QMessageBox::Cancel,
            QMessageBox::Save);
        if (QMessageBox::Cancel == ret) {
            return false;
        } else if (QMessageBox::Discard != ret) {
            sl_saveScene();
        }
    }
    return true;
}

static QString newActorLabel(ActorPrototype *proto, const QList<Actor*> &procs) {
    QStringList allLabels;
    foreach(Actor *actor, procs) {
        allLabels << actor->getLabel();
    }
    return WorkflowUtils::createUniqueString(proto->getDisplayName(), " ", allLabels);
}

Actor * WorkflowView::createActor(ActorPrototype *proto, const QVariantMap &params) const {
    assert(NULL != proto);
    QString pId = proto->getId().replace(QRegExp("\\s"), "-");
    ActorId id = Schema::uniqueActorId(pId, schema->getProcesses());
    Actor *actor = proto->createInstance(id, NULL, params);
    assert(NULL != actor);

    actor->setLabel(newActorLabel(proto, schema->getProcesses()));
    return actor;
}

void WorkflowView::onModified() {
    scene->onModified();
}

WorkflowBusItem * WorkflowView::tryBind(WorkflowPortItem *from, WorkflowPortItem *to) {
    WorkflowBusItem* dit = NULL;

    if (from->getPort()->canBind(to->getPort())) {
        Port *src = from->getPort();
        Port *dest = to->getPort();
        if (src->isInput()) {
            src = to->getPort();
            dest = from->getPort();
        }
        if (WorkflowUtils::isPathExist(src, dest)) {
            return NULL;
        }

        Link *link = new Link(src, dest);
        schema->addFlow(link);
        dit = scene->addFlow(from, to, link);
        removeEstimations();
    }
    return dit;
}

void WorkflowView::sl_updateSchema() {
    schema->update();
}

Workflow::Schema * WorkflowView::getSchema() const {
    return schema;
}

const Workflow::Metadata & WorkflowView::getMeta() {
    return updateMeta();
}

const Workflow::Metadata & WorkflowView::updateMeta() {
    meta.setSampleMark(false);
    meta.resetVisual();
    foreach (QGraphicsItem *it, scene->items()) {
        switch (it->type()) {
        case WorkflowProcessItemType:
            {
                WorkflowProcessItem *proc = qgraphicsitem_cast<WorkflowProcessItem*>(it);
                ActorVisualData visual(proc->getProcess()->getId());
                visual.setPos(proc->pos());
                ItemViewStyle *style = proc->getStyleById(proc->getStyle());
                if (NULL != style) {
                    visual.setStyle(style->getId());
                    if (style->getBgColor() != style->defaultColor()) {
                        visual.setColor(style->getBgColor());
                    }
                    if (style->defaultFont() != QFont()) {
                        visual.setFont(style->defaultFont());
                    }
                    if (ItemStyles::EXTENDED == style->getId()) {
                        ExtendedProcStyle *eStyle = dynamic_cast<ExtendedProcStyle*>(style);
                        if (!eStyle->isAutoResized()) {
                            visual.setRect(eStyle->boundingRect());
                        }
                    }
                }
                foreach (WorkflowPortItem *port, proc->getPortItems()) {
                    visual.setPortAngle(port->getPort()->getId(), port->getOrientarion());
                }
                meta.setActorVisualData(visual);
            }
            break;
        case WorkflowBusItemType:
            {
                WorkflowBusItem *bus = qgraphicsitem_cast<WorkflowBusItem*>(it);
                Port *src = bus->getBus()->source();
                Port *dst = bus->getBus()->destination();
                QPointF p = bus->getText()->pos();
                meta.setTextPos(src->owner()->getId(), src->getId(), dst->owner()->getId(), dst->getId(), p);
            }
            break;
        }
    }
    return meta;
}

Workflow::Metadata WorkflowView::getMeta(const QList<WorkflowProcessItem*> &items) {
    const Workflow::Metadata &meta = getMeta();
    Workflow::Metadata result;
    result.name = meta.name;
    result.url = meta.url;
    result.comment = meta.comment;

    foreach (WorkflowProcessItem *proc, items) {
        bool contains = false;
        ActorVisualData visual = meta.getActorVisualData(proc->getProcess()->getId(), contains);
        assert(contains);
        result.setActorVisualData(visual);
        foreach (WorkflowPortItem *port1, proc->getPortItems()) {
            foreach (WorkflowBusItem *bus, port1->getDataFlows()) {
                WorkflowPortItem *port2 = (bus->getInPort() == port1) ? bus->getOutPort() : bus->getInPort();
                WorkflowProcessItem *proc2 = port2->getOwner();
                if (!items.contains(proc2)) {
                    continue;
                }
                Port *src = bus->getBus()->source();
                Port *dst = bus->getBus()->destination();
                QPointF p = meta.getTextPos(src->owner()->getId(), src->getId(), dst->owner()->getId(), dst->getId(), contains);
                if (contains) {
                    result.setTextPos(src->owner()->getId(), src->getId(), dst->owner()->getId(), dst->getId(), p);
                }
            }
        }
    }
    return result;
}

RunFileSystem * WorkflowView::getRFS() {
    RunFileSystem *result = new RunFileSystem(this);
    RFSUtils::initRFS(*result, schema->getProcesses(), this);
    return result;
}

QVariant WorkflowView::getAttributeValue(const AttributeInfo &info) const {
    Actor *actor = schema->actorById(info.actorId);
    CHECK(NULL != actor, QVariant());
    Attribute *attr = actor->getParameter(info.attrId);
    CHECK(NULL != attr, QVariant());
    return attr->getAttributePureValue();
}

void WorkflowView::setAttributeValue(const AttributeInfo &info, const QVariant &value) {
    Actor *actor = schema->actorById(info.actorId);
    CHECK(NULL != actor, );
    Attribute *attr = actor->getParameter(info.attrId);
    CHECK(NULL != attr, );
    attr->setAttributeValue(value);
}

bool WorkflowView::isShowSamplesHint() const {
    SAFE_POINT(NULL != samples, "NULL samples widget", false);
    SAFE_POINT(NULL != schema, "NULL schema", false);
    const bool emptySchema = (0 == schema->getProcesses().size());
    return samples->isVisible() && emptySchema;

}

/********************************
 * WorkflowScene
 ********************************/
static bool canDrop(const QMimeData* m, QList<ActorPrototype*>& lst) {
    if (m->hasFormat(WorkflowPalette::MIME_TYPE)) {
        QString id(m->data(WorkflowPalette::MIME_TYPE));
        ActorPrototype* proto = WorkflowEnv::getProtoRegistry()->getProto(id);
        if (proto) {
            lst << proto;
        }
    } else {
        foreach(QList<ActorPrototype*> l, WorkflowEnv::getProtoRegistry()->getProtos().values()) {
            foreach(ActorPrototype* proto, l) {
                if (proto->isAcceptableDrop(m)) {
                    lst << proto;
                }
            }
        }
    }
    return !lst.isEmpty();
}

WorkflowScene::WorkflowScene(WorkflowView *parent)
: QGraphicsScene(parent), controller(parent), modified(false), locked(false), runner(NULL), hint(0) {
    openDocumentsAction = new QAction(tr("Open document(s)"), this);
    connect(openDocumentsAction, SIGNAL(triggered()), SLOT(sl_openDocuments()));
}

WorkflowScene::~WorkflowScene() {
    sl_reset();
}

void WorkflowScene::sl_deleteItem() {
    assert(!locked);
    QList<WorkflowProcessItem*> items;
    foreach(QGraphicsItem* it, selectedItems()) {
        WorkflowProcessItem *proc = qgraphicsitem_cast<WorkflowProcessItem*>(it);
        WorkflowBusItem *bus = qgraphicsitem_cast<WorkflowBusItem*>(it);
        switch (it->type()) {
            case WorkflowProcessItemType:
                items << proc;
                break;
            case WorkflowBusItemType:
                controller->removeBusItem(bus);
                setModified();
                break;
        }
    }
    foreach(WorkflowProcessItem *it, items) {
        if ( it->getProcess() != NULL ) {
            emit si_itemDeleted( it->getProcess()->getId() );
        }
        controller->removeProcessItem(it);
        setModified();
    }

    controller->update();
    emit configurationChanged();
    update();
}

QList<Actor*> WorkflowScene::getSelectedActors() const {
    QList<Actor*> list;
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == WorkflowProcessItemType) {
            list << static_cast<WorkflowProcessItem*>(item)->getProcess();
        }
    }
    return list;
}

void WorkflowScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * e) {
    QGraphicsScene::contextMenuEvent(e);
    if (!e->isAccepted()) {
        QMenu menu;
        controller->setupContextMenu(&menu);
        e->accept();
        menu.exec(e->screenPos());
    }
}

void WorkflowScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent) {
    if (!mouseEvent->isAccepted() && (mouseEvent->button() == Qt::LeftButton) && !selectedItems().isEmpty()) {
        emit processDblClicked();
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void WorkflowScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event) {
    QList<ActorPrototype*> lst;
    if (!locked && canDrop(event->mimeData(), lst)) {
        event->acceptProposedAction();
    } else {
        QGraphicsScene::dragEnterEvent(event);
    }
}

void WorkflowScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event) {
    QList<ActorPrototype*> lst;
    if (!locked && canDrop(event->mimeData(), lst)) {
        event->acceptProposedAction();
    } else {
        QGraphicsScene::dragMoveEvent(event);
    }
}

void WorkflowScene::dropEvent(QGraphicsSceneDragDropEvent * event) {
    QList<ActorPrototype*> lst;
    if (!locked && canDrop(event->mimeData(), lst))
    {
        QList<QGraphicsItem *> targets = items(event->scenePos());
        bool done = false;
        foreach(QGraphicsItem* it, targets) {
            WorkflowProcessItem* target = qgraphicsitem_cast<WorkflowProcessItem*>(it);
            if (target && lst.contains(target->getProcess()->getProto())) {
                clearSelection();
                QVariantMap params;
                Actor* a = target->getProcess();
                a->getProto()->isAcceptableDrop(event->mimeData(), &params);
                QMapIterator<QString, QVariant> cfg(params);
                while (cfg.hasNext())
                {
                    cfg.next();
                    a->setParameter(cfg.key(),cfg.value());
                }
                target->setSelected(true);
                done = true;
                break;
            }
        }
        if (!done) {
            ActorPrototype* proto = lst.size() > 1 ? ChooseItemDialog(controller).select(lst) : lst.first();
            if (proto) {
                Actor* a = controller->getActor();
                if (a) {
                    controller->addProcess( a, event->scenePos());
                }
                event->setDropAction(Qt::CopyAction);
            }
        }
    }
    QGraphicsScene::dropEvent(event);
}

void WorkflowScene::clearScene() {
    sl_reset();
}

void WorkflowScene::setupLinkCtxMenu(const QString& href, Actor* actor, const QPoint& pos) {
    const QString attributeId = WorkflowUtils::getParamIdFromHref(href);
    bool isInput = attributeId == BaseAttributes::URL_IN_ATTRIBUTE().getId();
    bool isOutput = attributeId == BaseAttributes::URL_OUT_ATTRIBUTE().getId();
    if (isInput || isOutput) {
        Attribute *attribute = actor->getParameter(attributeId);
        QString urlStr;
        const QStringList urlList = WorkflowUtils::getAttributeUrls(attribute);

        foreach (const QString &url, urlList) {
            if (QFileInfo(url).isFile()) {
                urlStr.append(url).append(';');
            }
        }
        urlStr = urlStr.left(urlStr.size() - 1);

        if (!urlStr.isEmpty()) {
            QMenu menu;
            openDocumentsAction->setData(urlStr);
            menu.addAction(openDocumentsAction);
            menu.exec(pos);
        }
    }
}

void WorkflowScene::sl_openDocuments() {
    const QString& urlStr = openDocumentsAction->data().value<QString>();
    const QStringList& _urls = WorkflowUtils::expandToUrls(urlStr);
    QList<GUrl> urls;
    foreach(const QString& url, _urls) {
        urls.append(url);
    }
    Task* t = AppContext::getProjectLoader()->openWithProjectTask(urls);
    if (t) {
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    } else {
        QMessageBox::critical(controller, tr("Workflow Designer"), tr("Unable to open specified documents. Watch log for details."));
    }
}

void WorkflowScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (!locked && !mouseEvent->isAccepted() && controller->selectedProto() && (mouseEvent->button() == Qt::LeftButton)) {
        controller->addProcess(controller->getActor(), mouseEvent->scenePos());
    }
    lastMousePressPoint = mouseEvent->scenePos();
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void WorkflowScene::sl_selectAll() {
    foreach (QGraphicsItem *it, items()) {
        it->setSelected(true);
    }
}

void WorkflowScene::sl_deselectAll() {
    foreach (QGraphicsItem *it, items()) {
        it->setSelected(false);
    }
}

void WorkflowScene::sl_reset() {
    QList<QGraphicsItem*> list;
    QList<QGraphicsItem*> itemss = items();
    foreach (QGraphicsItem *it, itemss) {
        if (it->type() == WorkflowProcessItemType) {
            list << it;
        }
    }
    modified = false;
    foreach(QGraphicsItem* it, list) {
        removeItem(it);
        delete it;
    }
}

void WorkflowScene::setModified(bool b) {
    modified = b;
    update();
}

void WorkflowScene::setModified() {
    setModified(true);
}

void WorkflowScene::drawBackground(QPainter * painter, const QRectF & rect)
{
    if (WorkflowSettings::showGrid()) {
        int step = GRID_STEP;
        painter->setPen(QPen(QColor(200, 200, 255, 125)));
        // draw horizontal grid
        qreal start = round(rect.top(), step);
        if (start > rect.top()) {
            start -= step;
        }
        for (qreal y = start - step; y < rect.bottom(); ) {
            y += step;
            painter->drawLine(rect.left(), y, rect.right(), y);
        }
        // now draw vertical grid
        start = round(rect.left(), step);
        if (start > rect.left()) {
            start -= step;
        }
        for (qreal x = start - step; x < rect.right(); ) {
            x += step;
            painter->drawLine(x, rect.top(), x, rect.bottom());
        }
    }

    if (items().size() == 0) {
        // draw a hint on empty scene
        painter->setPen(Qt::darkGray);
        QFont f = painter->font();
        if (hint == SamplesTab) {
        } else {
            QTransform trans = painter->combinedTransform();
            f.setFamily("Courier New");
            f.setPointSizeF(f.pointSizeF()* 2./trans.m11());
            painter->setFont(f);
            QRectF res;
            painter->drawText(sceneRect(), Qt::AlignCenter, tr("Drop an element from the palette here"), &res);
            QPixmap pix(":workflow_designer/images/leftarrow.png");
            QPointF pos(res.left(), res.center().y());
            pos.rx() -= pix.width() + GRID_STEP;
            pos.ry() -= pix.height()/2;
            painter->drawPixmap(pos, pix);
        }
    }
}

void WorkflowScene::onModified() {
    assert(!locked);
    modified = true;
    emit configurationChanged();
}

void WorkflowScene::centerView() {
    QRectF childRect;
    foreach (QGraphicsItem *child, items()) {
        QPointF childPos = child->pos();
        QTransform matrix = child->transform() * QTransform().translate(childPos.x(), childPos.y());
        childRect |= matrix.mapRect(child->boundingRect() | child->childrenBoundingRect());
    }
    update();
}

WorkflowBusItem * WorkflowScene::addFlow(WorkflowPortItem *from, WorkflowPortItem *to, Link *link) {
    WorkflowBusItem *dit = new WorkflowBusItem(from, to, link);
    from->addDataFlow(dit);
    to->addDataFlow(dit);

    addItem(dit);
    dit->updatePos();
    setModified(true);
    return dit;
}

void WorkflowScene::connectConfigurationEditors() {
    foreach(QGraphicsItem *i, items()) {
        if(i->type() == WorkflowProcessItemType) {
            Actor *proc = static_cast<WorkflowProcessItem *>(i)->getProcess();
            ConfigurationEditor *editor = proc->getEditor();
            if (NULL != editor) {
                connect(editor, SIGNAL(si_configurationChanged()), this, SIGNAL(configurationChanged()));
            }
            GrouperEditor *g = dynamic_cast<GrouperEditor*>(editor);
            MarkerEditor *m = dynamic_cast<MarkerEditor*>(editor);
            if (NULL != g || NULL != m) {
                connect(editor, SIGNAL(si_configurationChanged()), controller, SLOT(sl_updateSchema()));
            }
        }
    }
}

/************************************************************************/
/* SceneCreator */
/************************************************************************/
SceneCreator::SceneCreator(Schema *_schema, const Workflow::Metadata &_meta)
: schema(_schema), meta(_meta), scene(NULL)
{

}

SceneCreator::~SceneCreator() {
    delete scene;
}

WorkflowScene * SceneCreator::recreateScene(WorkflowScene *_scene) {
    scene = _scene;
    scene->sl_reset();
    return createScene();
}

WorkflowScene * SceneCreator::createScene(WorkflowView *controller) {
    scene = new WorkflowScene(controller);
    scene->setSceneRect(QRectF(-3*WS, -3*WS, 5*WS, 5*WS));
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    return createScene();
    scene->setObjectName("scene");
}

WorkflowScene * SceneCreator::createScene() {
    QMap<Port*, WorkflowPortItem*> ports;
    foreach (Actor *actor, schema->getProcesses()) {
        WorkflowProcessItem *procItem = createProcess(actor);
        scene->addItem(procItem);
        foreach (WorkflowPortItem *portItem, procItem->getPortItems()) {
            ports[portItem->getPort()] = portItem;
        }
    }

    foreach (Link *link, schema->getFlows()) {
        createBus(ports, link);
    }

    WorkflowScene *result = scene;
    scene = NULL;
    return result;
}

WorkflowProcessItem * SceneCreator::createProcess(Actor *actor) {
    WorkflowProcessItem *procItem = new WorkflowProcessItem(actor);
    bool contains = false;
    ActorVisualData visual = meta.getActorVisualData(actor->getId(), contains);
    if (!contains) {
        return procItem;
    }
    QPointF p = visual.getPos(contains);
    if (contains) {
        procItem->setPos(p);
    }
    QString s = visual.getStyle(contains);
    if (contains) {
        procItem->setStyle(s);
        {
            ItemViewStyle *eStyle = procItem->getStyleById(ItemStyles::EXTENDED);
            ItemViewStyle *sStyle = procItem->getStyleById(ItemStyles::SIMPLE);
            QColor c = visual.getColor(contains);
            if (contains) {
                eStyle->setBgColor(c);
                sStyle->setBgColor(c);
            }
            QFont f = visual.getFont(contains);
            if (contains) {
                eStyle->setDefaultFont(f);
                sStyle->setDefaultFont(f);
            }
            QRectF r = visual.getRect(contains);
            if (contains) {
                qobject_cast<ExtendedProcStyle*>(eStyle)->setFixedBounds(r);
            }
        }
    }
    foreach (WorkflowPortItem *portItem, procItem->getPortItems()) {
        Port *port = portItem->getPort();
        qreal a = visual.getPortAngle(port->getId(), contains);
        if (contains) {
            portItem->setOrientation(a);
        }
    }
    return procItem;
}

void SceneCreator::createBus(const QMap<Port*, WorkflowPortItem*> &ports, Link *link) {
    WorkflowPortItem *src = ports[link->source()];
    WorkflowPortItem *dst = ports[link->destination()];
    WorkflowBusItem *busItem = scene->addFlow(src, dst, link);
    ActorId srcActorId = src->getOwner()->getProcess()->getId();
    ActorId dstActorId = dst->getOwner()->getProcess()->getId();

    bool contains = false;
    QPointF p = meta.getTextPos(srcActorId, link->source()->getId(),
        dstActorId, link->destination()->getId(), contains);
    if (contains) {
        busItem->getText()->setPos(p);
    }
}

}//namespace
