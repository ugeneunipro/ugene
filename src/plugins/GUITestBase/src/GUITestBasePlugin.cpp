/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "GUITestBasePlugin.h"
#include <harness/UGUITestBase.h>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ToolsMenu.h>

#include "harness/GUITestService.h"
#include "tests/PosteriorActions.h"
#include "tests/PosteriorChecks.h"
#include "tests/PreliminaryActions.h"
#include "tests/common_scenarios/Assembling/Assembly_browser/GTTestsAssemblyBrowser.h"
#include "tests/common_scenarios/Assembling/bowtie2/GTTestsBowtie2.h"
#include "tests/common_scenarios/Assembling/dna_assembly/GTTestsDnaAssembly.h"
#include "tests/common_scenarios/Assembling/dna_assembly/GTTestsIndexReuse.h"
#include "tests/common_scenarios/Assembling/dna_assembly/conversions/GTTestsDnaAssemblyConversions.h"
#include "tests/common_scenarios/Assembling/extract_consensus/GTTestsAssemblyExtractConsensus.h"
#include "tests/common_scenarios/Assembling/sam/GTTestsSAM.h"
#include "tests/common_scenarios/annotations/GTTestsAnnotations.h"
#include "tests/common_scenarios/annotations/GTTestsCreateAnnotationWidget.h"
#include "tests/common_scenarios/annotations/edit/GTTestsAnnotationsEdit.h"
#include "tests/common_scenarios/annotations/qualifiers/GTTestsAnnotationsQualifiers.h"
#include "tests/common_scenarios/annotations_import/GTTestsAnnotationsImport.h"
#include "tests/common_scenarios/circular_view/GTTestsCvGeneral.h"
#include "tests/common_scenarios/cloning/GTTestsCloning.h"
#include "tests/common_scenarios/create_shortcut/GTTestsCreateShortcut.h"
#include "tests/common_scenarios/document_from_text/GTTestsDocumentFromText.h"
#include "tests/common_scenarios/dp_view/GTTestsDpView.h"
#include "tests/common_scenarios/fasttree/GTTestsFastTree.h"
#include "tests/common_scenarios/genecut/GTTestsGeneCut.h"
#include "tests/common_scenarios/iqtree/GTTestsIQTree.h"
#include "tests/common_scenarios/mca_editor/GTTestsMcaEditor.h"
#include "tests/common_scenarios/msa_editor/GTTestsMsaEditor.h"
#include "tests/common_scenarios/msa_editor/align/GTTestsAlignSequenceToMsa.h"
#include "tests/common_scenarios/msa_editor/colors/GTTestsMSAEditorColors.h"
#include "tests/common_scenarios/msa_editor/consensus/GTTestsMSAEditorConsensus.h"
#include "tests/common_scenarios/msa_editor/edit/GTTestsMSAEditorEdit.h"
#include "tests/common_scenarios/msa_editor/exclude_list/GTTestsMsaExcludeList.h"
#include "tests/common_scenarios/msa_editor/multiline/GTTestsMSAMultiline.h"
#include "tests/common_scenarios/msa_editor/multiline/colors/GTTestsColorsMSAMultiline.h"
#include "tests/common_scenarios/msa_editor/multiline/options/GTTestsOptionPanelMSAMultiline.h"
#include "tests/common_scenarios/msa_editor/overview/GTTestsMSAEditorOverview.h"
#include "tests/common_scenarios/msa_editor/replace_character/GTTestsMSAEditorReplaceCharacter.h"
#include "tests/common_scenarios/msa_editor/tree/GTTestsMSAEditorTree.h"
#include "tests/common_scenarios/options_panel/GTTestsOptionPanel.h"
#include "tests/common_scenarios/options_panel/msa/GTTestsOptionPanelMSA.h"
#include "tests/common_scenarios/options_panel/sequence_view/GTTestsOptionPanelSequenceView.h"
#include "tests/common_scenarios/pcr/GTTestsInSilicoPcr.h"
#include "tests/common_scenarios/pcr/GTTestsPrimerLibrary.h"
#include "tests/common_scenarios/phyml/GTTestsCommonScenariosPhyml.h"
#include "tests/common_scenarios/primer3/GTTestsCommonScenariosPrimer3.h"
#include "tests/common_scenarios/project/GTTestsProject.h"
#include "tests/common_scenarios/project/anonymous_project/GTTestsProjectAnonymousProject.h"
#include "tests/common_scenarios/project/bookmarks/GTTestsBookmarks.h"
#include "tests/common_scenarios/project/document_modifying/GTTestsProjectDocumentModifying.h"
#include "tests/common_scenarios/project/multiple_docs/GTTestsProjectMultipleDocs.h"
#include "tests/common_scenarios/project/project_filtering/GTTestsProjectFiltering.h"
#include "tests/common_scenarios/project/relations/GTTestsProjectRelations.h"
#include "tests/common_scenarios/project/remote_request/GTTestsProjectRemoteRequest.h"
#include "tests/common_scenarios/project/sequence_exporting/GTTestsProjectSequenceExporting.h"
#include "tests/common_scenarios/project/sequence_exporting/from_project_view/GTTestsFromProjectView.h"
#include "tests/common_scenarios/project/user_locking/GTTestsProjectUserLocking.h"
#include "tests/common_scenarios/query_designer/GTTestsQueryDesigner.h"
#include "tests/common_scenarios/repeat_finder/GTTestsRepeatFinder.h"
#include "tests/common_scenarios/sanger/GTTestsSanger.h"
#include "tests/common_scenarios/sequence_edit/GTTestsSequenceEdit.h"
#include "tests/common_scenarios/sequence_edit/GTTestsSequenceEditMode.h"
#include "tests/common_scenarios/sequence_selection/GTTestsSequenceSelection.h"
#include "tests/common_scenarios/sequence_view/GTTestsSequenceView.h"
#include "tests/common_scenarios/smith_waterman_dialog/GTTestsSWDialog.h"
#include "tests/common_scenarios/start_page/GTTestsStartPage.h"
#include "tests/common_scenarios/toggle_view/GTTestsToggleView.h"
#include "tests/common_scenarios/tree_viewer/GTTestsCommonScenariousTreeviewer.h"
#include "tests/common_scenarios/undo_redo/GTTestsUndoRedo.h"
#include "tests/common_scenarios/workflow_designer/GTTestsWorkflowDesigner.h"
#include "tests/common_scenarios/workflow_designer/dashboard/GTTestsWorkflowDashboard.h"
#include "tests/common_scenarios/workflow_designer/elements/GTTestsTrimmomaticElement.h"
#include "tests/common_scenarios/workflow_designer/estimating/GTTestsWorkflowEstimating.h"
#include "tests/common_scenarios/workflow_designer/name_filter/GTTestsWorkflowNameFilter.h"
#include "tests/common_scenarios/workflow_designer/parameters_validation/GTTestsWorkflowParameterValidation.h"
#include "tests/common_scenarios/workflow_designer/samples/GTTestsWorkflowSamples.h"
#include "tests/common_scenarios/workflow_designer/scripting/GTTestsWorkflowScripting.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_1001_2000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_1_1000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_2001_3000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_3001_4000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_4001_5000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_5001_6000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_6001_7000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_7001_8000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_8001_9000.h"

namespace U2 {

/** Returns the given labels list as is. Used to improve readability of macros. */
static QStringList labels(const QStringList& labelList) {
    return labelList;
}

/** Converts list of label args into QStringList and adds 'Nightly' and all supported platform labels to the list. */
static QStringList nightly(const QStringList& labelList = QStringList()) {
    QStringList resultLabelList = labelList;
    resultLabelList << UGUITestLabels::Precommit << UGUITestLabels::Linux << UGUITestLabels::MacOS << UGUITestLabels::Windows;
    return resultLabelList;
}

/** Registers a GUI test included into nightly build with a default timeout. */
#define REGISTER_TEST(TestClass) \
    guiTestBase->registerTest(new TestClass(DEFAULT_GUI_TEST_TIMEOUT, nightly()));

/** Registers a GUI test with the TIMEOUT and LABELS provided. */
#define REGISTER_TEST_TL(TestClass, TIMEOUT, LABELS) \
    guiTestBase->registerTest(new TestClass(TIMEOUT, LABELS));

/** Registers a GUI test with the labels provided.*/
#define REGISTER_TEST_L(TestClass, LABELS) REGISTER_TEST_TL(TestClass, DEFAULT_GUI_TEST_TIMEOUT, LABELS);

/** Registers test only for the specified platforms only. */
#define REGISTER_TEST_LINUX(TestClass) REGISTER_TEST_L(TestClass, labels({Precommit, Linux}))
#define REGISTER_TEST_MAC(TestClass) REGISTER_TEST_L(TestClass, labels({Precommit, MacOS}))
#define REGISTER_TEST_WINDOWS(TestClass) REGISTER_TEST_L(TestClass, labels({Precommit, Windows}))
#define REGISTER_TEST_LINUX_AND_MAC(TestClass) REGISTER_TEST_L(TestClass, labels({Precommit, Linux, MacOS}))
#define REGISTER_TEST_LINUX_AND_WINDOWS(TestClass) REGISTER_TEST_L(TestClass, labels({Precommit, Linux, Windows}))

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    CHECK(AppContext::getMainWindow() != nullptr, nullptr);
    if (GUITestService::isGuiTestServiceNeeded()) {
        new GUITestService();
    }
    return new GUITestBasePlugin();
}

GUITestBasePlugin::GUITestBasePlugin()
    : Plugin(tr("GUITestBase"), tr("GUI Test Base")) {
    UGUITestBase* guiTestBase = UGUITestBase::getInstance();

    registerTests(guiTestBase);
    registerAdditionalActions(guiTestBase);

    openGUITestRunnerAction = new QAction(tr("GUI Test runner"), this);
    openGUITestRunnerAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_G));
    openGUITestRunnerAction->setObjectName("GUI_TEST_RUNNER");
    openGUITestRunnerAction->setIcon(QIcon(":gui_test/images/open_gui_test_runner.png"));
    connect(openGUITestRunnerAction, SIGNAL(triggered()), SLOT(sl_showWindow()));
    ToolsMenu::addAction(ToolsMenu::TOOLS, openGUITestRunnerAction);
}

void GUITestBasePlugin::sl_showWindow() {
    if (view == nullptr) {
        view = new GUITestRunner(UGUITestBase::getInstance());
        view->show();
    } else {
        view->raise();
    }
}

void GUITestBasePlugin::registerTests(UGUITestBase* guiTestBase) {
    SAFE_POINT(guiTestBase != nullptr, "guiTestBase is null!", );
    REGISTER_TEST(GUITest_regression_scenarios::test_5998);
}

void GUITestBasePlugin::registerAdditionalActions(UGUITestBase* guiTestBase) {
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0000, UGUITestBase::PreCheck);
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0001, UGUITestBase::PreCheck);
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0002, UGUITestBase::PreCheck);
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0003, UGUITestBase::PreCheck);
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0004, UGUITestBase::PreCheck);
    guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0005, UGUITestBase::PreCheck);

    guiTestBase->registerTest(new GUITest_posterior_checks::post_check_0000, UGUITestBase::PostCheck);

    guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0000, UGUITestBase::PostAction);
    guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0001, UGUITestBase::PostAction);
    guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0002, UGUITestBase::PostAction);
    guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0003, UGUITestBase::PostAction);
    guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0004, UGUITestBase::PostAction);
}

}  // namespace U2
