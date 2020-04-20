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

#include <QDir>
#include <QMessageBox>
#include <QMenu>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/L10n.h>
#include <U2Core/ServiceTypes.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Gui/ToolsMenu.h>

#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowTasksRegistry.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include "WorkflowDesignerPlugin.h"
#include "WorkflowDocument.h"
#include "WorkflowSamples.h"
#include "WorkflowSettingsController.h"
#include "WorkflowViewController.h"
#include "cmdline/GalaxyConfigTask.h"
#include "cmdline/WorkflowCMDLineTasks.h"
#include "library/CoreLib.h"
#include "library/IncludedProtoFactoryImpl.h"
#include "tasks/ReadAssemblyTask.h"
#include "util/DatasetsCountValidator.h"
#include "util/SaveSchemaImageUtils.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    WorkflowDesignerPlugin * plug = new WorkflowDesignerPlugin();
    return plug;
}

#define PLUGIN_SETTINGS QString("workflowview/")

const QString WorkflowDesignerPlugin::RUN_WORKFLOW               = "task";
const QString WorkflowDesignerPlugin::REMOTE_MACHINE             = "task-remote-machine";
const QString WorkflowDesignerPlugin::PRINT                      = "print";
const QString WorkflowDesignerPlugin::CUSTOM_EL_WITH_SCRIPTS_DIR = "custom-element-script-dir";
const QString WorkflowDesignerPlugin::CUSTOM_EXTERNAL_TOOL_DIR   = "custom-element-external-tool-dir";
const QString WorkflowDesignerPlugin::INCLUDED_ELEMENTS_DIR      = "imported-workflow-element-dir";
const QString WorkflowDesignerPlugin::WORKFLOW_OUTPUT_DIR        = "workfow-output-dir";

WorkflowDesignerPlugin::WorkflowDesignerPlugin()
: Plugin(tr("Workflow Designer"), tr("Workflow Designer allows one to create complex computational workflows.")){
    if (AppContext::getMainWindow()) {
        services << new WorkflowDesignerService();
        AppContext::getAppSettingsGUI()->registerPage(new WorkflowSettingsPageController());
        AppContext::getObjectViewFactoryRegistry()->registerGObjectViewFactory(new WorkflowViewFactory(this));
    }
    IncludedProtoFactory::init(new IncludedProtoFactoryImpl());

    AppContext::getDocumentFormatRegistry()->registerFormat(new WorkflowDocFormat(this));

    // xml workflow tests removed. commented for future uses

    //GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    //XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    //assert(xmlTestFormat!=NULL);

    //GAutoDeleteList<XMLTestFactory>* l = new GAutoDeleteList<XMLTestFactory>(this);
    //l->qlist = WorkflowTests::createTestFactories();

    //foreach(XMLTestFactory* f, l->qlist) {
    //    bool res = xmlTestFormat->registerTestFactory(f);
    //    assert(res);
    //    Q_UNUSED(res);
    //}

    registerCMDLineHelp();
    processCMDLineOptions();
    WorkflowEnv::getActorValidatorRegistry()->addValidator(DatasetsCountValidator::ID, new DatasetsCountValidator());

    CHECK(AppContext::getPluginSupport(), );
    connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_initWorkers()));

    DashboardInfoRegistry *dashboardsInfoRegistry = AppContext::getDashboardInfoRegistry();
    SAFE_POINT(nullptr != dashboardsInfoRegistry, "dashboardsInfoRegistry is nullptr", );
    AppContext::getDashboardInfoRegistry()->scanDashboardsDir();
}

void WorkflowDesignerPlugin::processCMDLineOptions() {
    CMDLineRegistry * cmdlineReg = AppContext::getCMDLineRegistry();
    assert(cmdlineReg != nullptr);

    if (cmdlineReg->hasParameter(CUSTOM_EL_WITH_SCRIPTS_DIR)) {
        WorkflowSettings::setUserDirectory(FileAndDirectoryUtils::getAbsolutePath(cmdlineReg->getParameterValue(CUSTOM_EL_WITH_SCRIPTS_DIR)));
    }
    if (cmdlineReg->hasParameter(CUSTOM_EXTERNAL_TOOL_DIR)) {
        WorkflowSettings::setExternalToolDirectory(FileAndDirectoryUtils::getAbsolutePath(cmdlineReg->getParameterValue(CUSTOM_EXTERNAL_TOOL_DIR)));
    }
    if (cmdlineReg->hasParameter(INCLUDED_ELEMENTS_DIR)) {
        WorkflowSettings::setIncludedElementsDirectory(FileAndDirectoryUtils::getAbsolutePath(cmdlineReg->getParameterValue(INCLUDED_ELEMENTS_DIR)));
    }
    if (cmdlineReg->hasParameter(WORKFLOW_OUTPUT_DIR)) {
        WorkflowSettings::setWorkflowOutputDirectory(FileAndDirectoryUtils::getAbsolutePath(cmdlineReg->getParameterValue(WORKFLOW_OUTPUT_DIR)));
    }

    bool consoleMode = !AppContext::isGUIMode(); // only in console mode we run workflows by default. Otherwise we show them
    if (cmdlineReg->hasParameter( RUN_WORKFLOW ) || (consoleMode && !CMDLineRegistryUtils::getPureValues().isEmpty()) ) {
        Task * t = new WorkflowRunFromCMDLineTask();
        connect(AppContext::getTaskScheduler(), SIGNAL(si_ugeneIsReadyToWork()), new TaskStarter(t), SLOT(registerTask()));
    } else {
        if( cmdlineReg->hasParameter(GalaxyConfigTask::GALAXY_CONFIG_OPTION) && consoleMode ) {
            Task *t = nullptr;
            const QString schemePath =  cmdlineReg->getParameterValue( GalaxyConfigTask::GALAXY_CONFIG_OPTION );
            const QString ugenePath = cmdlineReg->getParameterValue( GalaxyConfigTask::UGENE_PATH_OPTION );
            const QString galaxyPath = cmdlineReg->getParameterValue( GalaxyConfigTask::GALAXY_PATH_OPTION );
            const QString destinationPath = nullptr;
            t = new GalaxyConfigTask( schemePath, ugenePath, galaxyPath, destinationPath );
            connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(t), SLOT(registerTask()));
        }
    }
}

void WorkflowDesignerPlugin::sl_saveSchemaImageTaskFinished() {
    ProduceSchemaImageLinkTask * saveImgTask = qobject_cast<ProduceSchemaImageLinkTask*>(sender());
    assert(saveImgTask != NULL);
    if(saveImgTask->getState() != Task::State_Finished) {
        return;
    }

    QString imgUrl = saveImgTask->getImageLink();
    fprintf(stdout, "%s", imgUrl.toLocal8Bit().constData());
}

void WorkflowDesignerPlugin::registerWorkflowTasks() {
    WorkflowTasksRegistry *registry = WorkflowEnv::getWorkflowTasksRegistry();

    ReadDocumentTaskFactory *readAssemblyFactory = new ReadAssemblyTaskFactory();
    bool ok = registry->registerReadDocumentTaskFactory(readAssemblyFactory);
    if (!ok) {
        coreLog.error("Can not register read assembly task");
    }
}

void WorkflowDesignerPlugin::registerCMDLineHelp() {
    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();
    assert( NULL != cmdLineRegistry );

    CMDLineHelpProvider * taskSection = new CMDLineHelpProvider(
        RUN_WORKFLOW,
        tr("Runs the specified task."),
        tr("Runs the specified task. A path to a user-defined UGENE workflow"
           " be used as a task name."),
        tr("<task_name> [<task_parameter>=value ...]"));

    cmdLineRegistry->registerCMDLineHelpProvider( taskSection );

    CMDLineHelpProvider * printSection = new CMDLineHelpProvider(
        PRINT,
        tr("Prints the content of the specified slot."),
        tr("Prints the content of the specified slot. The incoming/outcoming content of"
        " specified slot is printed to the standard output."),
        tr("<actor_name>.<port_name>.<slot_name>"));
    Q_UNUSED(printSection);

    CMDLineHelpProvider * galaxyConfigSection = new CMDLineHelpProvider(
        GalaxyConfigTask::GALAXY_CONFIG_OPTION,
        tr("Creates new Galaxy tool config."),
        tr("Creates new Galaxy tool config from existing workflow. Paths to UGENE"
        " and Galaxy can be set"),
        tr("<uwl-file> [--ugene-path=value] [--galaxy-path=value]"));

    cmdLineRegistry->registerCMDLineHelpProvider( galaxyConfigSection );

    //CMDLineHelpProvider * remoteMachineSectionArguments = new CMDLineHelpProvider( REMOTE_MACHINE, "<path-to-machine-file>");
    //CMDLineHelpProvider * remoteMachineSection = new CMDLineHelpProvider( REMOTE_MACHINE, tr("run provided tasks on given remote machine") );
    //TODO: bug UGENE-23
    //cmdLineRegistry->registerCMDLineHelpProvider( remoteMachineSectionArguments );
    //cmdLineRegistry->registerCMDLineHelpProvider( remoteMachineSection );
}

void WorkflowDesignerPlugin::sl_initWorkers() {
    Workflow::CoreLib::init();
    registerWorkflowTasks();
    Workflow::CoreLib::initIncludedWorkers();
}

WorkflowDesignerPlugin::~WorkflowDesignerPlugin() {
    Workflow::CoreLib::cleanup();
}

class CloseDesignerTask : public Task {
public:
    CloseDesignerTask(WorkflowDesignerService* s) :
      Task(U2::WorkflowDesignerPlugin::tr("Close Designer"), TaskFlag_NoRun),
          service(s) {}
    virtual void prepare();
private:
    WorkflowDesignerService* service;
};

void CloseDesignerTask::prepare() {
    if (!service->closeViews()) {
        stateInfo.setError(  U2::WorkflowDesignerPlugin::tr("Close Designer canceled") );
    }
}

Task* WorkflowDesignerService::createServiceDisablingTask(){
    return new CloseDesignerTask(this);
}

WorkflowDesignerService::WorkflowDesignerService()
: Service(Service_WorkflowDesigner, tr("Workflow Designer"), ""),
designerAction(NULL), managerAction(NULL), newWorkflowAction(NULL)
{

}

void WorkflowDesignerService::serviceStateChangedCallback(ServiceState , bool enabledStateChanged) {
    IdRegistry<WelcomePageAction> *welcomePageActions = AppContext::getWelcomePageActionRegistry();
    SAFE_POINT(NULL != welcomePageActions, L10N::nullPointerError("Welcome Page Actions"), );

    if (!enabledStateChanged) {
        return;
    }
    if (isEnabled()) {
        SAFE_POINT(NULL == designerAction, "Illegal WD service state", );
        SAFE_POINT(NULL == newWorkflowAction, "Illegal WD service state", );

        if(!AppContext::getPluginSupport()->isAllPluginsLoaded()) {
            connect( AppContext::getPluginSupport(), SIGNAL( si_allStartUpPluginsLoaded() ), SLOT(sl_startWorkflowPlugin()));
        } else {
            sl_startWorkflowPlugin();
        }

        welcomePageActions->registerEntry(new WorkflowWelcomePageAction(this));
    } else {
        welcomePageActions->unregisterEntry(BaseWelcomePageActions::CREATE_WORKFLOW);
        delete newWorkflowAction;
        newWorkflowAction = NULL;
        delete designerAction;
        designerAction = NULL;
    }
}

void WorkflowDesignerService::sl_startWorkflowPlugin() {
    initDesignerAction();
    initNewWorkflowAction();
    initSampleActions();
}

void WorkflowDesignerService::initDesignerAction() {
    designerAction = new QAction( QIcon(":/workflow_designer/images/wd.png"), tr("Workflow Designer..."), this);
    designerAction->setObjectName(ToolsMenu::WORKFLOW_DESIGNER);
#ifdef _DEBUG
    designerAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
#endif
    connect(designerAction, SIGNAL(triggered()), SLOT(sl_showDesignerWindow()));
    ToolsMenu::addAction(ToolsMenu::TOOLS, designerAction);
}

void WorkflowDesignerService::initNewWorkflowAction() {
    newWorkflowAction = new QAction(QIcon(":/workflow_designer/images/wd.png"), tr("New workflow..."), this);
    newWorkflowAction->setObjectName("New workflow");
    connect(newWorkflowAction, SIGNAL(triggered()), SLOT(sl_showDesignerWindow()));

    QMenu *fileMenu = AppContext::getMainWindow()->getTopLevelMenu(MWMENU_FILE);
    QAction *beforeAction = NULL;
    foreach (QAction *action, fileMenu->actions()) {
        if (action->objectName() == ACTION_PROJECTSUPPORT__NEW_SECTION_SEPARATOR) {
            beforeAction = action;
            break;
        }
    }
    fileMenu->insertAction(beforeAction, newWorkflowAction);
}

bool WorkflowDesignerService::closeViews() {
    MWMDIManager* wm = AppContext::getMainWindow()->getMDIManager();
    assert(wm);
    foreach(MWMDIWindow* w, wm->getWindows()) {
        WorkflowView* view = qobject_cast<WorkflowView*>(w);
        if (view) {
            if (!AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(view)) {
                return false;
            }
        }
    }
    return true;
}

bool WorkflowDesignerService::checkServiceState() const {
    if (isDisabled()) {
        QMessageBox::warning(QApplication::activeWindow(), L10N::warningTitle(), L10N::internalError(tr("Can not open Workflow Designer. Please, try to reload UGENE.")));
        return false;
    }
    return true;
}

void WorkflowDesignerService::sl_showDesignerWindow() {
    CHECK(checkServiceState(), );
    WorkflowView::openWD(NULL); //FIXME
}

void WorkflowDesignerService::sl_sampleActionClicked(const SampleAction &action) {
    CHECK(checkServiceState(), );

    WorkflowView *view = WorkflowView::openWD(NULL);
    CHECK(nullptr != view, );

    view->sl_loadScene(QDir("data:workflow_samples").path() + "/" + action.samplePath, false);
}

void WorkflowDesignerService::sl_showManagerWindow() {

}

Task* WorkflowDesignerService::createServiceEnablingTask()
{
    QString defaultDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/workflow_samples";

    return SampleRegistry::init(QStringList(defaultDir));
}

void WorkflowDesignerService::initSampleActions() {
    SampleActionsManager *samples = new SampleActionsManager(this);
    connect(samples, SIGNAL(si_clicked(const SampleAction &)), SLOT(sl_sampleActionClicked(const SampleAction &)));

    const QString externalToolsPlugin = "external_tool_support";

    SampleAction ngsControl(ToolsMenu::NGS_CONTROL, ToolsMenu::NGS_MENU, "NGS/fastqc.uwl", tr("Reads quality control..."));
    ngsControl.requiredPlugins << externalToolsPlugin;
    SampleAction ngsDenovo(ToolsMenu::NGS_DENOVO, ToolsMenu::NGS_MENU, "NGS/from_tools_menu_only/ngs_assembly.uwl", tr("Reads de novo assembly (with SPAdes)..."));
    ngsDenovo.requiredPlugins << externalToolsPlugin;
    SampleAction ngsScaffold(ToolsMenu::NGS_SCAFFOLD, ToolsMenu::NGS_MENU, "Scenarios/length_filter.uwl", tr("Filter short scaffolds..."));
    ngsScaffold.requiredPlugins << externalToolsPlugin;
    SampleAction ngsRawDna(ToolsMenu::NGS_RAW_DNA, ToolsMenu::NGS_MENU, "NGS/raw_dna.uwl", tr("Raw DNA-Seq data processing..."));
    ngsRawDna.requiredPlugins << externalToolsPlugin;
    SampleAction ngsVariants(ToolsMenu::NGS_CALL_VARIANTS, ToolsMenu::NGS_MENU, "NGS/ngs_variant_calling.uwl", tr("Variant calling..."));
    ngsVariants.requiredPlugins << externalToolsPlugin;
    SampleAction ngsEffect(ToolsMenu::NGS_VARIANT_EFFECT, ToolsMenu::NGS_MENU, "NGS/ngs_variant_annotation.uwl", tr("Annotate variants and predict effects..."));
    ngsEffect.requiredPlugins << externalToolsPlugin;
    SampleAction ngsRawRna(ToolsMenu::NGS_RAW_RNA, ToolsMenu::NGS_MENU, "NGS/raw_rna.uwl", tr("Raw RNA-Seq data processing..."));
    ngsRawRna.requiredPlugins << externalToolsPlugin;
    SampleAction ngsRna(ToolsMenu::NGS_RNA, ToolsMenu::NGS_MENU, "NGS/ngs_transcriptomics_tophat_stringtie.uwl", tr("RNA-Seq data analysis..."));
    ngsRna.requiredPlugins << externalToolsPlugin;
    SampleAction ngsTranscript(ToolsMenu::NGS_TRANSCRIPT, ToolsMenu::NGS_MENU, "NGS/extract_transcript_seq.uwl", tr("Extract transcript sequences..."));
    ngsTranscript.requiredPlugins << externalToolsPlugin;
    SampleAction ngsRawChip(ToolsMenu::NGS_RAW_CHIP, ToolsMenu::NGS_MENU, "NGS/raw_chip.uwl", tr("Raw ChIP-Seq data processing..."));
    ngsRawChip.requiredPlugins << externalToolsPlugin;
    SampleAction ngsChip(ToolsMenu::NGS_CHIP, ToolsMenu::NGS_MENU, "NGS/cistrome.uwl", tr("ChIP-Seq data analysis..."));
    ngsChip.requiredPlugins << externalToolsPlugin;
    SampleAction ngsClassification(ToolsMenu::NGS_CLASSIFICATION, ToolsMenu::NGS_MENU, "NGS/from_tools_menu_only/ngs_classification.uwl", tr("Metagenomics classification..."));
    ngsChip.requiredPlugins << externalToolsPlugin << "kraken_support" << "clark_support" << "diamond_support" << "wevote_support" << "ngs_reads_classification";
    SampleAction ngsCoverage(ToolsMenu::NGS_COVERAGE, ToolsMenu::NGS_MENU, "NGS/extract_coverage.uwl", tr("Extract coverage from assemblies..."));
    ngsCoverage.requiredPlugins << externalToolsPlugin;
    SampleAction ngsConsensus(ToolsMenu::NGS_CONSENSUS, ToolsMenu::NGS_MENU, "NGS/consensus.uwl", tr("Extract consensus from assemblies..."));
    ngsConsensus.requiredPlugins << externalToolsPlugin;

    SampleAction blastNcbi(ToolsMenu::BLAST_NCBI, ToolsMenu::BLAST_MENU, "Scenarios/remote_blasting.uwl", tr("Remote NCBI BLAST..."));
    blastNcbi.requiredPlugins << "remote_blast";

    samples->registerAction(ngsControl);
    samples->registerAction(ngsDenovo);
    samples->registerAction(ngsScaffold);
    samples->registerAction(ngsRawDna);
    samples->registerAction(ngsVariants);
    samples->registerAction(ngsEffect);
    samples->registerAction(ngsRawRna);
    samples->registerAction(ngsRna);
    samples->registerAction(ngsTranscript);
    samples->registerAction(ngsRawChip);
    samples->registerAction(ngsChip);
    samples->registerAction(ngsClassification);
    samples->registerAction(ngsCoverage);
    samples->registerAction(ngsConsensus);
    samples->registerAction(blastNcbi);
}

/************************************************************************/
/* WorkflowWelcomePageAction */
/************************************************************************/
WorkflowWelcomePageAction::WorkflowWelcomePageAction(WorkflowDesignerService *service)
: WelcomePageAction(BaseWelcomePageActions::CREATE_WORKFLOW), service(service)
{

}

void WorkflowWelcomePageAction::perform() {
    SAFE_POINT(!service.isNull(), L10N::nullPointerError("Workflow Service"), );
    service->sl_showDesignerWindow();
}

}//namespace
