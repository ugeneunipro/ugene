/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include "tests/common_scenarios/external_tools/mfold/GTTestsMfold.h"
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
#include "tests/common_scenarios/workspace/GTTestsWorkspace.h"
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

    //////////////////////////////////////////////////////////////////////////
    // Tests for investigation
    //////////////////////////////////////////////////////////////////////////

    // REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0058); // not completed yet

    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_common_scenarios_project::test_0041);  // There is no "Shift + Insert" hotkey on Mac
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_common_scenarios_workflow_parameters_validation::test_0002);  //, "qt dialog can't be shown");
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_regression_scenarios::test_0889);  // Spidey tool is not available on Mac.
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_regression_scenarios::test_2140);
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_common_scenarios_create_shortcut::test_0001);
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_regression_scenarios::test_3690);  // Passes only with native menu bar. With non-native menu bar (like Mac uses for GUI tests) the checked shortcut does not work.
    REGISTER_TEST_LINUX_AND_WINDOWS(GUITest_regression_scenarios::test_7584);  // Spidey tool is not available on Mac.

    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0001);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0002);  //"Restore when this tool becomes available");
    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0003);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0004);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0005);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_Bowtie2::test_0006);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_common_scenarios_workflow_designer::test_0002_1);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_common_scenarios_workflow_designer::test_0005);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_common_scenarios_workflow_designer::test_0009);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_common_scenarios_workflow_designer::test_0010);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_common_scenarios_workflow_parameters_validation::test_0005);  //, "Test should run not under admin user on WIN");
    REGISTER_TEST_LINUX_AND_MAC(GUITest_dna_assembly_conversions::test_0001);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1640);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1662);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1664);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1681);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1681_1);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1681_2);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1681_3);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1693);  // no tuxedo for windows
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1735);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2266_1);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2282);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2475);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2638);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2640);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_2866);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_3950);  // too long for windows test server
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_6301);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_5425);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_5425_1);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_5425_2);

    REGISTER_TEST_LINUX(GUITest_common_scenarios_msa_editor::test_0025);
    REGISTER_TEST_LINUX(GUITest_common_scenarios_msa_editor::test_0028_linux);
    REGISTER_TEST_LINUX(GUITest_common_scenarios_workflow_designer::test_0006);
    REGISTER_TEST_LINUX(GUITest_common_scenarios_workflow_designer::test_0006_1);
    REGISTER_TEST_LINUX(GUITest_common_scenarios_workflow_designer::test_0007);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_5130);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_5295);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_7043);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_7611);

    REGISTER_TEST_MAC(GUITest_regression_scenarios::test_0339);
    REGISTER_TEST_MAC(GUITest_regression_scenarios::test_1551);
    REGISTER_TEST_MAC(GUITest_regression_scenarios::test_1680);

    REGISTER_TEST_WINDOWS(GUITest_common_scenarios_msa_editor::test_0025_1);
    REGISTER_TEST_WINDOWS(GUITest_common_scenarios_msa_editor::test_0028_windows);
    REGISTER_TEST_WINDOWS(GUITest_regression_scenarios::test_2089);  // "no forbidden folder characters on linux and mac";

    //////////////////////////////////////////////////////////////////////////
    // Regression scenarios/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_0057_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_0073_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0073_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_0394);

    REGISTER_TEST(GUITest_regression_scenarios::test_0407);
    REGISTER_TEST(GUITest_regression_scenarios::test_0490);

    REGISTER_TEST(GUITest_regression_scenarios::test_0567);
    REGISTER_TEST(GUITest_regression_scenarios::test_0574);
    REGISTER_TEST(GUITest_regression_scenarios::test_0587);
    REGISTER_TEST(GUITest_regression_scenarios::test_0597);
    REGISTER_TEST(GUITest_regression_scenarios::test_0598);

    REGISTER_TEST(GUITest_regression_scenarios::test_0605);
    REGISTER_TEST(GUITest_regression_scenarios::test_0610);
    REGISTER_TEST(GUITest_regression_scenarios::test_0627);
    REGISTER_TEST(GUITest_regression_scenarios::test_0652);
    REGISTER_TEST(GUITest_regression_scenarios::test_0659);
    REGISTER_TEST(GUITest_regression_scenarios::test_0666);
    REGISTER_TEST(GUITest_regression_scenarios::test_0677);
    REGISTER_TEST(GUITest_regression_scenarios::test_0678);
    REGISTER_TEST(GUITest_regression_scenarios::test_0680);
    REGISTER_TEST(GUITest_regression_scenarios::test_0681);
    REGISTER_TEST(GUITest_regression_scenarios::test_0684);
    REGISTER_TEST(GUITest_regression_scenarios::test_0685);
    REGISTER_TEST(GUITest_regression_scenarios::test_0688);

    REGISTER_TEST(GUITest_regression_scenarios::test_0700);
    REGISTER_TEST(GUITest_regression_scenarios::test_0702);
    REGISTER_TEST(GUITest_regression_scenarios::test_0703);
    REGISTER_TEST(GUITest_regression_scenarios::test_0733);
    REGISTER_TEST(GUITest_regression_scenarios::test_0734);
    REGISTER_TEST(GUITest_regression_scenarios::test_0746);
    REGISTER_TEST(GUITest_regression_scenarios::test_0750);
    REGISTER_TEST(GUITest_regression_scenarios::test_0762);
    REGISTER_TEST(GUITest_regression_scenarios::test_0768);
    REGISTER_TEST(GUITest_regression_scenarios::test_0774);
    REGISTER_TEST(GUITest_regression_scenarios::test_0775);
    REGISTER_TEST(GUITest_regression_scenarios::test_0776);
    REGISTER_TEST(GUITest_regression_scenarios::test_0778);
    REGISTER_TEST(GUITest_regression_scenarios::test_0779);
    REGISTER_TEST(GUITest_regression_scenarios::test_0782);
    REGISTER_TEST(GUITest_regression_scenarios::test_0786);
    REGISTER_TEST(GUITest_regression_scenarios::test_0792);
    REGISTER_TEST(GUITest_regression_scenarios::test_0798);

    REGISTER_TEST(GUITest_regression_scenarios::test_0801);
    REGISTER_TEST(GUITest_regression_scenarios::test_0807);
    REGISTER_TEST(GUITest_regression_scenarios::test_0808);
    REGISTER_TEST(GUITest_regression_scenarios::test_0812);
    REGISTER_TEST(GUITest_regression_scenarios::test_0814);
    REGISTER_TEST(GUITest_regression_scenarios::test_0821);
    REGISTER_TEST(GUITest_regression_scenarios::test_0828);
    REGISTER_TEST(GUITest_regression_scenarios::test_0830);
    REGISTER_TEST(GUITest_regression_scenarios::test_0834);
    REGISTER_TEST(GUITest_regression_scenarios::test_0835);
    REGISTER_TEST(GUITest_regression_scenarios::test_0839);
    REGISTER_TEST(GUITest_regression_scenarios::test_0840);
    REGISTER_TEST(GUITest_regression_scenarios::test_0842);
    REGISTER_TEST(GUITest_regression_scenarios::test_0844);
    REGISTER_TEST(GUITest_regression_scenarios::test_0846);
    REGISTER_TEST(GUITest_regression_scenarios::test_0854);
    REGISTER_TEST(GUITest_regression_scenarios::test_0858);
    REGISTER_TEST(GUITest_regression_scenarios::test_0861_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0861_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_0861_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_0861_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_0861_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_0866);
    REGISTER_TEST(GUITest_regression_scenarios::test_0868);
    REGISTER_TEST(GUITest_regression_scenarios::test_0871);
    REGISTER_TEST(GUITest_regression_scenarios::test_0873);
    REGISTER_TEST(GUITest_regression_scenarios::test_0878);
    REGISTER_TEST(GUITest_regression_scenarios::test_0882);
    REGISTER_TEST(GUITest_regression_scenarios::test_0886);
    REGISTER_TEST(GUITest_regression_scenarios::test_0888);
    REGISTER_TEST(GUITest_regression_scenarios::test_0896);
    REGISTER_TEST(GUITest_regression_scenarios::test_0898);
    REGISTER_TEST(GUITest_regression_scenarios::test_0899);

    REGISTER_TEST(GUITest_regression_scenarios::test_0908);
    REGISTER_TEST(GUITest_regression_scenarios::test_0910);
    REGISTER_TEST(GUITest_regression_scenarios::test_0928);
    REGISTER_TEST(GUITest_regression_scenarios::test_0930);
    REGISTER_TEST(GUITest_regression_scenarios::test_0934);
    REGISTER_TEST(GUITest_regression_scenarios::test_0935);
    REGISTER_TEST(GUITest_regression_scenarios::test_0938);
    REGISTER_TEST(GUITest_regression_scenarios::test_0940);
    REGISTER_TEST(GUITest_regression_scenarios::test_0941);
    REGISTER_TEST(GUITest_regression_scenarios::test_0947);
    REGISTER_TEST(GUITest_regression_scenarios::test_0948);
    REGISTER_TEST(GUITest_regression_scenarios::test_0952);
    REGISTER_TEST(GUITest_regression_scenarios::test_0958);
    REGISTER_TEST(GUITest_regression_scenarios::test_0965);
    REGISTER_TEST(GUITest_regression_scenarios::test_0967_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0967_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_0967_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_0969);
    REGISTER_TEST(GUITest_regression_scenarios::test_0970);
    REGISTER_TEST(GUITest_regression_scenarios::test_0981_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0981_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_0986);
    REGISTER_TEST(GUITest_regression_scenarios::test_0986_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0986_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_0986_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_0986_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_0994);
    REGISTER_TEST(GUITest_regression_scenarios::test_0999_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_0999_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_1000);
    REGISTER_TEST(GUITest_regression_scenarios::test_1001);
    REGISTER_TEST(GUITest_regression_scenarios::test_1001_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1001_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1001_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1001_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1003);
    REGISTER_TEST(GUITest_regression_scenarios::test_1011);
    REGISTER_TEST(GUITest_regression_scenarios::test_1013);
    REGISTER_TEST(GUITest_regression_scenarios::test_1015);
    REGISTER_TEST(GUITest_regression_scenarios::test_1015_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1015_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1015_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1015_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1016);
    REGISTER_TEST(GUITest_regression_scenarios::test_1020);
    REGISTER_TEST(GUITest_regression_scenarios::test_1021);
    REGISTER_TEST(GUITest_regression_scenarios::test_1021_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1021_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1021_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1021_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1022);
    REGISTER_TEST(GUITest_regression_scenarios::test_1029);
    REGISTER_TEST(GUITest_regression_scenarios::test_1037);
    REGISTER_TEST(GUITest_regression_scenarios::test_1038);
    REGISTER_TEST(GUITest_regression_scenarios::test_1044);
    REGISTER_TEST(GUITest_regression_scenarios::test_1047);
    REGISTER_TEST(GUITest_regression_scenarios::test_1048);
    REGISTER_TEST(GUITest_regression_scenarios::test_1049);
    REGISTER_TEST(GUITest_regression_scenarios::test_1052);
    REGISTER_TEST(GUITest_regression_scenarios::test_1058);
    REGISTER_TEST(GUITest_regression_scenarios::test_1059);
    REGISTER_TEST(GUITest_regression_scenarios::test_1061);
    REGISTER_TEST(GUITest_regression_scenarios::test_1063);
    REGISTER_TEST(GUITest_regression_scenarios::test_1064);
    REGISTER_TEST(GUITest_regression_scenarios::test_1065_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1065_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1065_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1068);
    REGISTER_TEST(GUITest_regression_scenarios::test_1069);
    REGISTER_TEST(GUITest_regression_scenarios::test_1071);
    REGISTER_TEST(GUITest_regression_scenarios::test_1078);
    REGISTER_TEST(GUITest_regression_scenarios::test_1080);
    REGISTER_TEST(GUITest_regression_scenarios::test_1083);
    REGISTER_TEST(GUITest_regression_scenarios::test_1093);

    REGISTER_TEST(GUITest_regression_scenarios::test_1107);
    REGISTER_TEST(GUITest_regression_scenarios::test_1113);
    REGISTER_TEST(GUITest_regression_scenarios::test_1113_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1115);
    REGISTER_TEST(GUITest_regression_scenarios::test_1121);
    REGISTER_TEST(GUITest_regression_scenarios::test_1122);
    REGISTER_TEST(GUITest_regression_scenarios::test_1123);
    REGISTER_TEST(GUITest_regression_scenarios::test_1123_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1124);
    REGISTER_TEST(GUITest_regression_scenarios::test_1133);
    REGISTER_TEST(GUITest_regression_scenarios::test_1152);
    REGISTER_TEST(GUITest_regression_scenarios::test_1152_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1154);
    REGISTER_TEST(GUITest_regression_scenarios::test_1155);
    REGISTER_TEST(GUITest_regression_scenarios::test_1156);
    REGISTER_TEST(GUITest_regression_scenarios::test_1157);
    REGISTER_TEST(GUITest_regression_scenarios::test_1163);
    REGISTER_TEST(GUITest_regression_scenarios::test_1165);
    REGISTER_TEST(GUITest_regression_scenarios::test_1172);
    REGISTER_TEST(GUITest_regression_scenarios::test_1175);
    REGISTER_TEST(GUITest_regression_scenarios::test_1180);
    REGISTER_TEST(GUITest_regression_scenarios::test_1182);
    REGISTER_TEST(GUITest_regression_scenarios::test_1184);
    REGISTER_TEST(GUITest_regression_scenarios::test_1186_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1186_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1189);
    REGISTER_TEST(GUITest_regression_scenarios::test_1189_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1190);
    REGISTER_TEST(GUITest_regression_scenarios::test_1199);

    REGISTER_TEST(GUITest_regression_scenarios::test_1203_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1203_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1204);
    REGISTER_TEST(GUITest_regression_scenarios::test_1209);
    REGISTER_TEST(GUITest_regression_scenarios::test_1210);
    REGISTER_TEST(GUITest_regression_scenarios::test_1212);
    REGISTER_TEST(GUITest_regression_scenarios::test_1212_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1219);
    REGISTER_TEST(GUITest_regression_scenarios::test_1220);
    REGISTER_TEST(GUITest_regression_scenarios::test_1229);
    REGISTER_TEST(GUITest_regression_scenarios::test_1234);
    REGISTER_TEST(GUITest_regression_scenarios::test_1241);
    REGISTER_TEST(GUITest_regression_scenarios::test_1245);
    REGISTER_TEST(GUITest_regression_scenarios::test_1246);
    REGISTER_TEST(GUITest_regression_scenarios::test_1249);
    REGISTER_TEST(GUITest_regression_scenarios::test_1252);
    REGISTER_TEST(GUITest_regression_scenarios::test_1252_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1252_real);
    REGISTER_TEST(GUITest_regression_scenarios::test_1253);
    REGISTER_TEST(GUITest_regression_scenarios::test_1259);
    REGISTER_TEST(GUITest_regression_scenarios::test_1257);
    REGISTER_TEST(GUITest_regression_scenarios::test_1260);
    REGISTER_TEST(GUITest_regression_scenarios::test_1262);
    REGISTER_TEST(GUITest_regression_scenarios::test_1263);
    REGISTER_TEST(GUITest_regression_scenarios::test_1266);
    REGISTER_TEST(GUITest_regression_scenarios::test_1274);
    REGISTER_TEST(GUITest_regression_scenarios::test_1273);
    REGISTER_TEST(GUITest_regression_scenarios::test_1285);
    REGISTER_TEST(GUITest_regression_scenarios::test_1288);
    REGISTER_TEST(GUITest_regression_scenarios::test_1289);
    REGISTER_TEST(GUITest_regression_scenarios::test_1295);
    REGISTER_TEST(GUITest_regression_scenarios::test_1299);

    REGISTER_TEST(GUITest_regression_scenarios::test_1300_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1300_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1310);
    REGISTER_TEST(GUITest_regression_scenarios::test_1315_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1315_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1319);
    REGISTER_TEST(GUITest_regression_scenarios::test_1319_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1319_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1325);
    REGISTER_TEST(GUITest_regression_scenarios::test_1323);
    REGISTER_TEST(GUITest_regression_scenarios::test_1324);
    REGISTER_TEST(GUITest_regression_scenarios::test_1321_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1321_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1326);
    REGISTER_TEST(GUITest_regression_scenarios::test_1337);
    REGISTER_TEST(GUITest_regression_scenarios::test_1338);
    REGISTER_TEST(GUITest_regression_scenarios::test_1342);
    REGISTER_TEST(GUITest_regression_scenarios::test_1347);
    REGISTER_TEST(GUITest_regression_scenarios::test_1348);
    REGISTER_TEST(GUITest_regression_scenarios::test_1358);
    REGISTER_TEST(GUITest_regression_scenarios::test_1360);
    REGISTER_TEST(GUITest_regression_scenarios::test_1362);
    REGISTER_TEST(GUITest_regression_scenarios::test_1364);
    REGISTER_TEST(GUITest_regression_scenarios::test_1365);
    REGISTER_TEST(GUITest_regression_scenarios::test_1368);
    REGISTER_TEST(GUITest_regression_scenarios::test_1371);
    REGISTER_TEST(GUITest_regression_scenarios::test_1376);
    REGISTER_TEST(GUITest_regression_scenarios::test_1386);
    REGISTER_TEST(GUITest_regression_scenarios::test_1387);
    REGISTER_TEST(GUITest_regression_scenarios::test_1390);
    REGISTER_TEST(GUITest_regression_scenarios::test_1393);
    REGISTER_TEST(GUITest_regression_scenarios::test_1396);

    REGISTER_TEST(GUITest_regression_scenarios::test_1405);
    REGISTER_TEST(GUITest_regression_scenarios::test_1408);
    REGISTER_TEST(GUITest_regression_scenarios::test_1409);
    REGISTER_TEST(GUITest_regression_scenarios::test_1419);
    REGISTER_TEST(GUITest_regression_scenarios::test_1420);
    REGISTER_TEST(GUITest_regression_scenarios::test_1424);
    REGISTER_TEST(GUITest_regression_scenarios::test_1426);
    REGISTER_TEST(GUITest_regression_scenarios::test_1427);
    REGISTER_TEST(GUITest_regression_scenarios::test_1428);
    REGISTER_TEST(GUITest_regression_scenarios::test_1429);
    REGISTER_TEST(GUITest_regression_scenarios::test_1432);
    REGISTER_TEST(GUITest_regression_scenarios::test_1434_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1434_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1446);
    REGISTER_TEST(GUITest_regression_scenarios::test_1455);
    REGISTER_TEST(GUITest_regression_scenarios::test_1457);
    REGISTER_TEST(GUITest_regression_scenarios::test_1458);
    REGISTER_TEST(GUITest_regression_scenarios::test_1435);
    REGISTER_TEST(GUITest_regression_scenarios::test_1439);
    REGISTER_TEST(GUITest_regression_scenarios::test_1442_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1442_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1442_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1443);
    REGISTER_TEST(GUITest_regression_scenarios::test_1445);
    REGISTER_TEST(GUITest_regression_scenarios::test_1461_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1461_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1463);
    REGISTER_TEST(GUITest_regression_scenarios::test_1475);
    REGISTER_TEST(GUITest_regression_scenarios::test_1483);
    REGISTER_TEST(GUITest_regression_scenarios::test_1491);
    REGISTER_TEST(GUITest_regression_scenarios::test_1497);
    REGISTER_TEST(GUITest_regression_scenarios::test_1499);

    REGISTER_TEST(GUITest_regression_scenarios::test_1506);
    REGISTER_TEST(GUITest_regression_scenarios::test_1508);
    REGISTER_TEST(GUITest_regression_scenarios::test_1510);
    REGISTER_TEST(GUITest_regression_scenarios::test_1511);
    REGISTER_TEST(GUITest_regression_scenarios::test_1514);
    REGISTER_TEST(GUITest_regression_scenarios::test_1515);
    REGISTER_TEST(GUITest_regression_scenarios::test_1527);
    REGISTER_TEST(GUITest_regression_scenarios::test_1527_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1528);
    REGISTER_TEST(GUITest_regression_scenarios::test_1529);
    REGISTER_TEST(GUITest_regression_scenarios::test_1531);
    REGISTER_TEST(GUITest_regression_scenarios::test_1537);
    REGISTER_TEST(GUITest_regression_scenarios::test_1548);
    REGISTER_TEST(GUITest_regression_scenarios::test_1554);
    REGISTER_TEST(GUITest_regression_scenarios::test_1560);
    REGISTER_TEST(GUITest_regression_scenarios::test_1567);
    REGISTER_TEST(GUITest_regression_scenarios::test_1568);
    REGISTER_TEST(GUITest_regression_scenarios::test_1573);
    REGISTER_TEST(GUITest_regression_scenarios::test_1574);
    REGISTER_TEST(GUITest_regression_scenarios::test_1575);
    REGISTER_TEST(GUITest_regression_scenarios::test_1576);
    REGISTER_TEST(GUITest_regression_scenarios::test_1576_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1584);
    REGISTER_TEST(GUITest_regression_scenarios::test_1585);
    REGISTER_TEST(GUITest_regression_scenarios::test_1586);
    REGISTER_TEST(GUITest_regression_scenarios::test_1587);
    REGISTER_TEST(GUITest_regression_scenarios::test_1595);
    REGISTER_TEST(GUITest_regression_scenarios::test_1597);

    REGISTER_TEST(GUITest_regression_scenarios::test_1600_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_1600_8);
    REGISTER_TEST(GUITest_regression_scenarios::test_1603);
    REGISTER_TEST(GUITest_regression_scenarios::test_1606);
    REGISTER_TEST(GUITest_regression_scenarios::test_1607);
    REGISTER_TEST(GUITest_regression_scenarios::test_1609);
    REGISTER_TEST(GUITest_regression_scenarios::test_1616);
    REGISTER_TEST(GUITest_regression_scenarios::test_1622);
    REGISTER_TEST(GUITest_regression_scenarios::test_1626);
    REGISTER_TEST(GUITest_regression_scenarios::test_1627);
    REGISTER_TEST(GUITest_regression_scenarios::test_1629);
    REGISTER_TEST(GUITest_regression_scenarios::test_1631);
    REGISTER_TEST(GUITest_regression_scenarios::test_1643);
    REGISTER_TEST(GUITest_regression_scenarios::test_1645);
    REGISTER_TEST(GUITest_regression_scenarios::test_1651);
    REGISTER_TEST(GUITest_regression_scenarios::test_1653);
    REGISTER_TEST(GUITest_regression_scenarios::test_1654);
    REGISTER_TEST(GUITest_regression_scenarios::test_1658);
    REGISTER_TEST(GUITest_regression_scenarios::test_1660);
    REGISTER_TEST(GUITest_regression_scenarios::test_1661);
    REGISTER_TEST(GUITest_regression_scenarios::test_1668);
    REGISTER_TEST(GUITest_regression_scenarios::test_1672);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_1677);
    REGISTER_TEST(GUITest_regression_scenarios::test_1686);
    REGISTER_TEST(GUITest_regression_scenarios::test_1687);
    REGISTER_TEST(GUITest_regression_scenarios::test_1688);

    REGISTER_TEST(GUITest_regression_scenarios::test_1700);
    REGISTER_TEST(GUITest_regression_scenarios::test_1701);
    REGISTER_TEST(GUITest_regression_scenarios::test_1703);  //, "UGENE-3693"
    REGISTER_TEST(GUITest_regression_scenarios::test_1704);
    REGISTER_TEST(GUITest_regression_scenarios::test_1708);
    REGISTER_TEST(GUITest_regression_scenarios::test_1710_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1710_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1720);
    REGISTER_TEST(GUITest_regression_scenarios::test_1731);
    REGISTER_TEST(GUITest_regression_scenarios::test_1733);
    REGISTER_TEST(GUITest_regression_scenarios::test_1734);
    REGISTER_TEST(GUITest_regression_scenarios::test_1738);
    REGISTER_TEST(GUITest_regression_scenarios::test_1747);
    REGISTER_TEST(GUITest_regression_scenarios::test_1751);
    REGISTER_TEST(GUITest_regression_scenarios::test_1759);
    REGISTER_TEST(GUITest_regression_scenarios::test_1763_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1764);
    REGISTER_TEST(GUITest_regression_scenarios::test_1771);
    REGISTER_TEST(GUITest_regression_scenarios::test_1784);
    REGISTER_TEST(GUITest_regression_scenarios::test_1797);
    REGISTER_TEST(GUITest_regression_scenarios::test_1798);

    REGISTER_TEST(GUITest_regression_scenarios::test_1808);
    REGISTER_TEST(GUITest_regression_scenarios::test_1811_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1821);
    REGISTER_TEST(GUITest_regression_scenarios::test_1831);
    REGISTER_TEST(GUITest_regression_scenarios::test_1834);
    REGISTER_TEST(GUITest_regression_scenarios::test_1859);
    REGISTER_TEST(GUITest_regression_scenarios::test_1860);
    REGISTER_TEST(GUITest_regression_scenarios::test_1865);
    REGISTER_TEST(GUITest_regression_scenarios::test_1883);
    REGISTER_TEST(GUITest_regression_scenarios::test_1884);
    REGISTER_TEST(GUITest_regression_scenarios::test_1886_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1886_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1897);

    REGISTER_TEST(GUITest_regression_scenarios::test_1908);
    REGISTER_TEST(GUITest_regression_scenarios::test_1918);
    REGISTER_TEST(GUITest_regression_scenarios::test_1919);
    REGISTER_TEST(GUITest_regression_scenarios::test_1921);
    REGISTER_TEST(GUITest_regression_scenarios::test_1924);
    REGISTER_TEST_LINUX_AND_MAC(GUITest_regression_scenarios::test_1946);  // TopHat
    REGISTER_TEST(GUITest_regression_scenarios::test_1984);
    REGISTER_TEST(GUITest_regression_scenarios::test_1986);

    REGISTER_TEST(GUITest_regression_scenarios::test_2006);
    REGISTER_TEST(GUITest_regression_scenarios::test_2007);
    REGISTER_TEST(GUITest_regression_scenarios::test_2009);
    REGISTER_TEST(GUITest_regression_scenarios::test_2012);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_8);
    REGISTER_TEST(GUITest_regression_scenarios::test_2026);
    REGISTER_TEST(GUITest_regression_scenarios::test_2030);
    REGISTER_TEST(GUITest_regression_scenarios::test_2032);
    REGISTER_TEST(GUITest_regression_scenarios::test_2049);
    REGISTER_TEST(GUITest_regression_scenarios::test_2053);
    REGISTER_TEST(GUITest_regression_scenarios::test_2076);
    REGISTER_TEST(GUITest_regression_scenarios::test_2077);
    REGISTER_TEST(GUITest_regression_scenarios::test_2093_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2091);
    REGISTER_TEST(GUITest_regression_scenarios::test_2093_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_2100_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2100_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2124);
    REGISTER_TEST(GUITest_regression_scenarios::test_2128);
    REGISTER_TEST(GUITest_regression_scenarios::test_2128_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2138);
    REGISTER_TEST(GUITest_regression_scenarios::test_2144);
    REGISTER_TEST(GUITest_regression_scenarios::test_2150);
    REGISTER_TEST(GUITest_regression_scenarios::test_2152);
    REGISTER_TEST(GUITest_regression_scenarios::test_2156);
    REGISTER_TEST(GUITest_regression_scenarios::test_2157);
    REGISTER_TEST(GUITest_regression_scenarios::test_2160);
    REGISTER_TEST(GUITest_regression_scenarios::test_2165);
    REGISTER_TEST(GUITest_regression_scenarios::test_2187);
    REGISTER_TEST(GUITest_regression_scenarios::test_2188);
    REGISTER_TEST(GUITest_regression_scenarios::test_2192);

    REGISTER_TEST(GUITest_regression_scenarios::test_2202);
    REGISTER_TEST(GUITest_regression_scenarios::test_2225);
    REGISTER_TEST(GUITest_regression_scenarios::test_2259);
    REGISTER_TEST(GUITest_regression_scenarios::test_2267_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2267_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2269);
    REGISTER_TEST(GUITest_regression_scenarios::test_2270);
    REGISTER_TEST(GUITest_regression_scenarios::test_2281);
    REGISTER_TEST(GUITest_regression_scenarios::test_2284);
    REGISTER_TEST(GUITest_regression_scenarios::test_2285);
    REGISTER_TEST(GUITest_regression_scenarios::test_2270);
    REGISTER_TEST(GUITest_regression_scenarios::test_2292);
    REGISTER_TEST(GUITest_regression_scenarios::test_2293);
    REGISTER_TEST(GUITest_regression_scenarios::test_2295);
    REGISTER_TEST(GUITest_regression_scenarios::test_2298);

    REGISTER_TEST(GUITest_regression_scenarios::test_2306);
    REGISTER_TEST(GUITest_regression_scenarios::test_2309);
    REGISTER_TEST(GUITest_regression_scenarios::test_2314);
    REGISTER_TEST(GUITest_regression_scenarios::test_2316);
    REGISTER_TEST(GUITest_regression_scenarios::test_2318);
    REGISTER_TEST(GUITest_regression_scenarios::test_2343);
    REGISTER_TEST(GUITest_regression_scenarios::test_2351);
    REGISTER_TEST(GUITest_regression_scenarios::test_2352);
    REGISTER_TEST(GUITest_regression_scenarios::test_2360);
    REGISTER_TEST(GUITest_regression_scenarios::test_2364);
    REGISTER_TEST(GUITest_regression_scenarios::test_2373);
    REGISTER_TEST(GUITest_regression_scenarios::test_2374);
    REGISTER_TEST(GUITest_regression_scenarios::test_2377);
    REGISTER_TEST(GUITest_regression_scenarios::test_2378);
    REGISTER_TEST(GUITest_regression_scenarios::test_2378_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2379);
    REGISTER_TEST(GUITest_regression_scenarios::test_2382);
    REGISTER_TEST(GUITest_regression_scenarios::test_2382_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2387);
    REGISTER_TEST(GUITest_regression_scenarios::test_2392);

    REGISTER_TEST(GUITest_regression_scenarios::test_2400);
    REGISTER_TEST(GUITest_regression_scenarios::test_2401);
    REGISTER_TEST(GUITest_regression_scenarios::test_2402);
    REGISTER_TEST(GUITest_regression_scenarios::test_2403);
    REGISTER_TEST(GUITest_regression_scenarios::test_2404);
    REGISTER_TEST(GUITest_regression_scenarios::test_2406);
    REGISTER_TEST(GUITest_regression_scenarios::test_2407);
    REGISTER_TEST(GUITest_regression_scenarios::test_2410);
    REGISTER_TEST(GUITest_regression_scenarios::test_2415);
    REGISTER_TEST(GUITest_regression_scenarios::test_2424);
    REGISTER_TEST(GUITest_regression_scenarios::test_2430);
    REGISTER_TEST(GUITest_regression_scenarios::test_2431);
    REGISTER_TEST(GUITest_regression_scenarios::test_2432);
    REGISTER_TEST(GUITest_regression_scenarios::test_2449);
    REGISTER_TEST(GUITest_regression_scenarios::test_2451);
    REGISTER_TEST(GUITest_regression_scenarios::test_2459);
    REGISTER_TEST(GUITest_regression_scenarios::test_2460);
    REGISTER_TEST(GUITest_regression_scenarios::test_2470);
    REGISTER_TEST(GUITest_regression_scenarios::test_2482);
    REGISTER_TEST(GUITest_regression_scenarios::test_2487);
    REGISTER_TEST(GUITest_regression_scenarios::test_2496);
    REGISTER_TEST(GUITest_regression_scenarios::test_2498);

    REGISTER_TEST(GUITest_regression_scenarios::test_2506);
    REGISTER_TEST(GUITest_regression_scenarios::test_2506_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2513);
    REGISTER_TEST(GUITest_regression_scenarios::test_2519);
    REGISTER_TEST(GUITest_regression_scenarios::test_2538);
    REGISTER_TEST(GUITest_regression_scenarios::test_2540);
    REGISTER_TEST(GUITest_regression_scenarios::test_2542);
    REGISTER_TEST(GUITest_regression_scenarios::test_2544);
    REGISTER_TEST(GUITest_regression_scenarios::test_2545);
    REGISTER_TEST(GUITest_regression_scenarios::test_2549);
    REGISTER_TEST(GUITest_regression_scenarios::test_2565);
    REGISTER_TEST(GUITest_regression_scenarios::test_2566);
    REGISTER_TEST(GUITest_regression_scenarios::test_2567);
    REGISTER_TEST(GUITest_regression_scenarios::test_2568);
    REGISTER_TEST(GUITest_regression_scenarios::test_2569);
    REGISTER_TEST(GUITest_regression_scenarios::test_2577);
    REGISTER_TEST(GUITest_regression_scenarios::test_2578);
    REGISTER_TEST(GUITest_regression_scenarios::test_2579);
    REGISTER_TEST(GUITest_regression_scenarios::test_2581);
    REGISTER_TEST(GUITest_regression_scenarios::test_2581_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2581_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2581_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_2581_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_2583);

    REGISTER_TEST(GUITest_regression_scenarios::test_2605);
    REGISTER_TEST(GUITest_regression_scenarios::test_2612);
    REGISTER_TEST(GUITest_regression_scenarios::test_2619);
    REGISTER_TEST(GUITest_regression_scenarios::test_2622);
    REGISTER_TEST(GUITest_regression_scenarios::test_2622_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2632);
    REGISTER_TEST(GUITest_regression_scenarios::test_2651);
    REGISTER_TEST(GUITest_regression_scenarios::test_2656);
    REGISTER_TEST(GUITest_regression_scenarios::test_2662);
    REGISTER_TEST(GUITest_regression_scenarios::test_2667);
    REGISTER_TEST(GUITest_regression_scenarios::test_2683);
    REGISTER_TEST(GUITest_regression_scenarios::test_2690);

    REGISTER_TEST(GUITest_regression_scenarios::test_2701);
    REGISTER_TEST(GUITest_regression_scenarios::test_2709);
    REGISTER_TEST(GUITest_regression_scenarios::test_2713);
    REGISTER_TEST(GUITest_regression_scenarios::test_2726);
    REGISTER_TEST(GUITest_regression_scenarios::test_2729);
    REGISTER_TEST(GUITest_regression_scenarios::test_2730);
    REGISTER_TEST(GUITest_regression_scenarios::test_2737);
    REGISTER_TEST(GUITest_regression_scenarios::test_2737_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2754);
    REGISTER_TEST(GUITest_regression_scenarios::test_2761_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2761_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2762);
    REGISTER_TEST(GUITest_regression_scenarios::test_2770);
    REGISTER_TEST(GUITest_regression_scenarios::test_2773);
    REGISTER_TEST(GUITest_regression_scenarios::test_2778);
    REGISTER_TEST(GUITest_regression_scenarios::test_2784);
    REGISTER_TEST(GUITest_regression_scenarios::test_2796);

    REGISTER_TEST(GUITest_regression_scenarios::test_2801);
    REGISTER_TEST(GUITest_regression_scenarios::test_2801_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2808);
    REGISTER_TEST(GUITest_regression_scenarios::test_2809);
    REGISTER_TEST(GUITest_regression_scenarios::test_2811);
    REGISTER_TEST(GUITest_regression_scenarios::test_2829);
    REGISTER_TEST(GUITest_regression_scenarios::test_2853);
    REGISTER_TEST(GUITest_regression_scenarios::test_2863);
    REGISTER_TEST(GUITest_regression_scenarios::test_2884);
    REGISTER_TEST(GUITest_regression_scenarios::test_2887);
    REGISTER_TEST(GUITest_regression_scenarios::test_2894);
    REGISTER_TEST(GUITest_regression_scenarios::test_2895);
    REGISTER_TEST(GUITest_regression_scenarios::test_2897);
    REGISTER_TEST(GUITest_regression_scenarios::test_2899);

    REGISTER_TEST(GUITest_regression_scenarios::test_2900);
    REGISTER_TEST(GUITest_regression_scenarios::test_2903);
    REGISTER_TEST(GUITest_regression_scenarios::test_2907);
    REGISTER_TEST(GUITest_regression_scenarios::test_2910);
    REGISTER_TEST(GUITest_regression_scenarios::test_2910_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2910_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2910_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_2923);
    REGISTER_TEST(GUITest_regression_scenarios::test_2924);
    REGISTER_TEST(GUITest_regression_scenarios::test_2927);
    REGISTER_TEST(GUITest_regression_scenarios::test_2929);
    REGISTER_TEST(GUITest_regression_scenarios::test_2930);
    REGISTER_TEST(GUITest_regression_scenarios::test_2931);
    REGISTER_TEST(GUITest_regression_scenarios::test_2945);
    REGISTER_TEST(GUITest_regression_scenarios::test_2951);
    REGISTER_TEST(GUITest_regression_scenarios::test_2962_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2962_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2971);
    REGISTER_TEST(GUITest_regression_scenarios::test_2972);
    REGISTER_TEST(GUITest_regression_scenarios::test_2975);
    REGISTER_TEST(GUITest_regression_scenarios::test_2981);
    REGISTER_TEST(GUITest_regression_scenarios::test_2987);
    REGISTER_TEST(GUITest_regression_scenarios::test_2991);
    REGISTER_TEST(GUITest_regression_scenarios::test_2998);

    REGISTER_TEST(GUITest_regression_scenarios::test_3014);
    REGISTER_TEST(GUITest_regression_scenarios::test_3017);
    REGISTER_TEST(GUITest_regression_scenarios::test_3031);
    REGISTER_TEST(GUITest_regression_scenarios::test_3034);
    REGISTER_TEST(GUITest_regression_scenarios::test_3052);
    REGISTER_TEST(GUITest_regression_scenarios::test_3052_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3073);
    REGISTER_TEST(GUITest_regression_scenarios::test_3085_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3085_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3086);
    REGISTER_TEST(GUITest_regression_scenarios::test_3092);

    REGISTER_TEST(GUITest_regression_scenarios::test_3101);
    REGISTER_TEST(GUITest_regression_scenarios::test_3112);
    REGISTER_TEST(GUITest_regression_scenarios::test_3126);
    REGISTER_TEST(GUITest_regression_scenarios::test_3128);
    REGISTER_TEST(GUITest_regression_scenarios::test_3128_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3138);
    REGISTER_TEST(GUITest_regression_scenarios::test_3139);
    REGISTER_TEST(GUITest_regression_scenarios::test_3140);
    REGISTER_TEST(GUITest_regression_scenarios::test_3142);
    REGISTER_TEST(GUITest_regression_scenarios::test_3143);
    REGISTER_TEST(GUITest_regression_scenarios::test_3155);
    REGISTER_TEST(GUITest_regression_scenarios::test_3165);
    REGISTER_TEST(GUITest_regression_scenarios::test_3170);
    REGISTER_TEST(GUITest_regression_scenarios::test_3175);
    REGISTER_TEST(GUITest_regression_scenarios::test_3180);

    REGISTER_TEST(GUITest_regression_scenarios::test_3209_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3214);
    REGISTER_TEST(GUITest_regression_scenarios::test_3216_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3216_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3216_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_3218);
    REGISTER_TEST(GUITest_regression_scenarios::test_3220);
    REGISTER_TEST(GUITest_regression_scenarios::test_3221);
    REGISTER_TEST(GUITest_regression_scenarios::test_3223);
    REGISTER_TEST(GUITest_regression_scenarios::test_3226);
    REGISTER_TEST(GUITest_regression_scenarios::test_3229);
    REGISTER_TEST(GUITest_regression_scenarios::test_3245);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3255);
    REGISTER_TEST(GUITest_regression_scenarios::test_3263);
    REGISTER_TEST(GUITest_regression_scenarios::test_3270);
    REGISTER_TEST(GUITest_regression_scenarios::test_3274);
    REGISTER_TEST(GUITest_regression_scenarios::test_3276);
    REGISTER_TEST(GUITest_regression_scenarios::test_3277);
    REGISTER_TEST(GUITest_regression_scenarios::test_3279);
    REGISTER_TEST(GUITest_regression_scenarios::test_3283);
    REGISTER_TEST(GUITest_regression_scenarios::test_3287);
    REGISTER_TEST(GUITest_regression_scenarios::test_3288);

    REGISTER_TEST(GUITest_regression_scenarios::test_3305);
    REGISTER_TEST(GUITest_regression_scenarios::test_3306);
    REGISTER_TEST(GUITest_regression_scenarios::test_3308);
    REGISTER_TEST(GUITest_regression_scenarios::test_3313);
    REGISTER_TEST(GUITest_regression_scenarios::test_3318);
    REGISTER_TEST(GUITest_regression_scenarios::test_3319);
    REGISTER_TEST(GUITest_regression_scenarios::test_3321);
    REGISTER_TEST(GUITest_regression_scenarios::test_3328);
    REGISTER_TEST(GUITest_regression_scenarios::test_3332);
    REGISTER_TEST(GUITest_regression_scenarios::test_3335);
    REGISTER_TEST(GUITest_regression_scenarios::test_3342);
    REGISTER_TEST(GUITest_regression_scenarios::test_3344);
    REGISTER_TEST(GUITest_regression_scenarios::test_3346);
    REGISTER_TEST(GUITest_regression_scenarios::test_3348);
    REGISTER_TEST(GUITest_regression_scenarios::test_3357);
    REGISTER_TEST(GUITest_regression_scenarios::test_3373);
    REGISTER_TEST(GUITest_regression_scenarios::test_3379);
    REGISTER_TEST(GUITest_regression_scenarios::test_3384);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_4);

    REGISTER_TEST(GUITest_regression_scenarios::test_3402);
    REGISTER_TEST(GUITest_regression_scenarios::test_3414);
    REGISTER_TEST(GUITest_regression_scenarios::test_3428);
    REGISTER_TEST(GUITest_regression_scenarios::test_3430);
    REGISTER_TEST(GUITest_regression_scenarios::test_3437);
    REGISTER_TEST(GUITest_regression_scenarios::test_3439);
    REGISTER_TEST(GUITest_regression_scenarios::test_3441);
    REGISTER_TEST(GUITest_regression_scenarios::test_3443);
    REGISTER_TEST(GUITest_regression_scenarios::test_3450);
    REGISTER_TEST(GUITest_regression_scenarios::test_3451);
    REGISTER_TEST(GUITest_regression_scenarios::test_3452);
    REGISTER_TEST(GUITest_regression_scenarios::test_3455);
    REGISTER_TEST(GUITest_regression_scenarios::test_3471);
    REGISTER_TEST(GUITest_regression_scenarios::test_3472);
    REGISTER_TEST(GUITest_regression_scenarios::test_3473);
    REGISTER_TEST(GUITest_regression_scenarios::test_3477);
    REGISTER_TEST(GUITest_regression_scenarios::test_3480);
    REGISTER_TEST(GUITest_regression_scenarios::test_3484);
    REGISTER_TEST(GUITest_regression_scenarios::test_3484_1);

    REGISTER_TEST(GUITest_regression_scenarios::test_3504);
    REGISTER_TEST(GUITest_regression_scenarios::test_3518);
    REGISTER_TEST(GUITest_regression_scenarios::test_3519_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3519_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3545);
    REGISTER_TEST(GUITest_regression_scenarios::test_3552);
    REGISTER_TEST(GUITest_regression_scenarios::test_3553);
    REGISTER_TEST(GUITest_regression_scenarios::test_3555);
    REGISTER_TEST(GUITest_regression_scenarios::test_3556);
    REGISTER_TEST(GUITest_regression_scenarios::test_3557);
    REGISTER_TEST(GUITest_regression_scenarios::test_3563_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3563_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3571_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3571_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3589);

    REGISTER_TEST(GUITest_regression_scenarios::test_3603);
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_3610);
    REGISTER_TEST(GUITest_regression_scenarios::test_3612);
    REGISTER_TEST(GUITest_regression_scenarios::test_3613);
    REGISTER_TEST(GUITest_regression_scenarios::test_3619);
    REGISTER_TEST(GUITest_regression_scenarios::test_3622);
    REGISTER_TEST(GUITest_regression_scenarios::test_3623);
    REGISTER_TEST(GUITest_regression_scenarios::test_3625);
    REGISTER_TEST(GUITest_regression_scenarios::test_3629);
    REGISTER_TEST(GUITest_regression_scenarios::test_3634);
    REGISTER_TEST(GUITest_regression_scenarios::test_3645);
    REGISTER_TEST(GUITest_regression_scenarios::test_3649);
    REGISTER_TEST(GUITest_regression_scenarios::test_3658);
    REGISTER_TEST(GUITest_regression_scenarios::test_3675);
    REGISTER_TEST(GUITest_regression_scenarios::test_3676);
    REGISTER_TEST(GUITest_regression_scenarios::test_3687_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3687_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_3702);
    REGISTER_TEST(GUITest_regression_scenarios::test_3710);
    REGISTER_TEST(GUITest_regression_scenarios::test_3715);
    REGISTER_TEST(GUITest_regression_scenarios::test_3717);
    REGISTER_TEST(GUITest_regression_scenarios::test_3723);
    REGISTER_TEST(GUITest_regression_scenarios::test_3724);
    REGISTER_TEST(GUITest_regression_scenarios::test_3730);
    REGISTER_TEST(GUITest_regression_scenarios::test_3731);
    REGISTER_TEST(GUITest_regression_scenarios::test_3732);
    REGISTER_TEST(GUITest_regression_scenarios::test_3736);
    REGISTER_TEST(GUITest_regression_scenarios::test_3738);
    REGISTER_TEST(GUITest_regression_scenarios::test_3744);
    REGISTER_TEST(GUITest_regression_scenarios::test_3749);
    REGISTER_TEST(GUITest_regression_scenarios::test_3755);
    REGISTER_TEST(GUITest_regression_scenarios::test_3757);
    REGISTER_TEST(GUITest_regression_scenarios::test_3760);
    REGISTER_TEST(GUITest_regression_scenarios::test_3768);
    REGISTER_TEST(GUITest_regression_scenarios::test_3770);
    REGISTER_TEST(GUITest_regression_scenarios::test_3772);
    REGISTER_TEST(GUITest_regression_scenarios::test_3773);
    REGISTER_TEST(GUITest_regression_scenarios::test_3773_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3778);
    REGISTER_TEST(GUITest_regression_scenarios::test_3779);
    REGISTER_TEST(GUITest_regression_scenarios::test_3785_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3785_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3788);
    REGISTER_TEST(GUITest_regression_scenarios::test_3797);

    REGISTER_TEST(GUITest_regression_scenarios::test_3805);
    REGISTER_TEST(GUITest_regression_scenarios::test_3809);
    REGISTER_TEST(GUITest_regression_scenarios::test_3815);
    REGISTER_TEST(GUITest_regression_scenarios::test_3816);
    REGISTER_TEST(GUITest_regression_scenarios::test_3817);
    REGISTER_TEST(GUITest_regression_scenarios::test_3821);
    REGISTER_TEST(GUITest_regression_scenarios::test_3829);
    REGISTER_TEST(GUITest_regression_scenarios::test_3843);
    REGISTER_TEST(GUITest_regression_scenarios::test_3850);
    REGISTER_TEST(GUITest_regression_scenarios::test_3862);
    REGISTER_TEST(GUITest_regression_scenarios::test_3868);
    REGISTER_TEST(GUITest_regression_scenarios::test_3869);
    REGISTER_TEST(GUITest_regression_scenarios::test_3870);
    REGISTER_TEST(GUITest_regression_scenarios::test_3886);
    REGISTER_TEST(GUITest_regression_scenarios::test_3895);

    REGISTER_TEST(GUITest_regression_scenarios::test_3901);
    REGISTER_TEST(GUITest_regression_scenarios::test_3902);
    REGISTER_TEST(GUITest_regression_scenarios::test_3903);
    REGISTER_TEST(GUITest_regression_scenarios::test_3904);
    REGISTER_TEST(GUITest_regression_scenarios::test_3905);
    REGISTER_TEST(GUITest_regression_scenarios::test_3920);
    REGISTER_TEST(GUITest_regression_scenarios::test_3924);
    REGISTER_TEST(GUITest_regression_scenarios::test_3927);
    REGISTER_TEST(GUITest_regression_scenarios::test_3928);
    REGISTER_TEST(GUITest_regression_scenarios::test_3938);

    REGISTER_TEST(GUITest_regression_scenarios::test_3953);
    REGISTER_TEST(GUITest_regression_scenarios::test_3959);
    REGISTER_TEST(GUITest_regression_scenarios::test_3960);
    REGISTER_TEST(GUITest_regression_scenarios::test_3975);
    REGISTER_TEST(GUITest_regression_scenarios::test_3983);
    REGISTER_TEST(GUITest_regression_scenarios::test_3988);
    REGISTER_TEST(GUITest_regression_scenarios::test_3994);
    REGISTER_TEST(GUITest_regression_scenarios::test_3995);
    REGISTER_TEST(GUITest_regression_scenarios::test_3996);
    REGISTER_TEST(GUITest_regression_scenarios::test_3997);
    REGISTER_TEST(GUITest_regression_scenarios::test_3998);

    REGISTER_TEST(GUITest_regression_scenarios::test_4007);
    REGISTER_TEST(GUITest_regression_scenarios::test_4008);
    REGISTER_TEST(GUITest_regression_scenarios::test_4009);
    REGISTER_TEST(GUITest_regression_scenarios::test_4011);
    REGISTER_TEST(GUITest_regression_scenarios::test_4010);
    REGISTER_TEST(GUITest_regression_scenarios::test_4013);
    REGISTER_TEST(GUITest_regression_scenarios::test_4022);
    REGISTER_TEST(GUITest_regression_scenarios::test_4026);
    REGISTER_TEST(GUITest_regression_scenarios::test_4030);
    REGISTER_TEST(GUITest_regression_scenarios::test_4033);
    REGISTER_TEST(GUITest_regression_scenarios::test_4034);
    REGISTER_TEST(GUITest_regression_scenarios::test_4035);
    REGISTER_TEST(GUITest_regression_scenarios::test_4036);
    REGISTER_TEST(GUITest_regression_scenarios::test_4045);
    REGISTER_TEST(GUITest_regression_scenarios::test_4046);
    REGISTER_TEST(GUITest_regression_scenarios::test_4047);
    REGISTER_TEST(GUITest_regression_scenarios::test_4059);
    REGISTER_TEST(GUITest_regression_scenarios::test_4064);
    REGISTER_TEST(GUITest_regression_scenarios::test_4065);
    REGISTER_TEST(GUITest_regression_scenarios::test_4070);
    REGISTER_TEST(GUITest_regression_scenarios::test_4071);
    REGISTER_TEST(GUITest_regression_scenarios::test_4072);
    REGISTER_TEST(GUITest_regression_scenarios::test_4084);
    REGISTER_TEST(GUITest_regression_scenarios::test_4086);
    REGISTER_TEST(GUITest_regression_scenarios::test_4087);
    REGISTER_TEST(GUITest_regression_scenarios::test_4091);
    REGISTER_TEST(GUITest_regression_scenarios::test_4093);
    REGISTER_TEST(GUITest_regression_scenarios::test_4095);
    REGISTER_TEST(GUITest_regression_scenarios::test_4096);
    REGISTER_TEST(GUITest_regression_scenarios::test_4097);
    REGISTER_TEST(GUITest_regression_scenarios::test_4099);

    REGISTER_TEST(GUITest_regression_scenarios::test_4104);
    REGISTER_TEST(GUITest_regression_scenarios::test_4106);
    REGISTER_TEST(GUITest_regression_scenarios::test_4116);
    REGISTER_TEST(GUITest_regression_scenarios::test_4117);
    REGISTER_TEST(GUITest_regression_scenarios::test_4118);
    REGISTER_TEST(GUITest_regression_scenarios::test_4121);
    REGISTER_TEST(GUITest_regression_scenarios::test_4122);
    REGISTER_TEST(GUITest_regression_scenarios::test_4124);
    REGISTER_TEST(GUITest_regression_scenarios::test_4127);
    REGISTER_TEST(GUITest_regression_scenarios::test_4131);
    REGISTER_TEST(GUITest_regression_scenarios::test_4134);
    REGISTER_TEST(GUITest_regression_scenarios::test_4141);
    REGISTER_TEST(GUITest_regression_scenarios::test_4148);
    REGISTER_TEST(GUITest_regression_scenarios::test_4151);
    REGISTER_TEST(GUITest_regression_scenarios::test_4153);
    REGISTER_TEST(GUITest_regression_scenarios::test_4156);
    REGISTER_TEST(GUITest_regression_scenarios::test_4160);
    REGISTER_TEST(GUITest_regression_scenarios::test_4164);
    REGISTER_TEST(GUITest_regression_scenarios::test_4170);
    REGISTER_TEST(GUITest_regression_scenarios::test_4177);
    REGISTER_TEST(GUITest_regression_scenarios::test_4177_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4179);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4194);

    REGISTER_TEST(GUITest_regression_scenarios::test_4209);
    REGISTER_TEST(GUITest_regression_scenarios::test_4209_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4218);
    REGISTER_TEST(GUITest_regression_scenarios::test_4218_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4221);
    REGISTER_TEST(GUITest_regression_scenarios::test_4232);
    REGISTER_TEST(GUITest_regression_scenarios::test_4232_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4244);
    REGISTER_TEST(GUITest_regression_scenarios::test_4266);
    REGISTER_TEST(GUITest_regression_scenarios::test_4272);
    REGISTER_TEST(GUITest_regression_scenarios::test_4276);
    REGISTER_TEST(GUITest_regression_scenarios::test_4284);
    REGISTER_TEST(GUITest_regression_scenarios::test_4293);
    REGISTER_TEST(GUITest_regression_scenarios::test_4295);

    REGISTER_TEST(GUITest_regression_scenarios::test_4302_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4302_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4306_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4306_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4308);
    REGISTER_TEST(GUITest_regression_scenarios::test_4309);
    REGISTER_TEST(GUITest_regression_scenarios::test_4309_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4323_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4323_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4323_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4323_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_4325);
    REGISTER_TEST(GUITest_regression_scenarios::test_4334);
    REGISTER_TEST(GUITest_regression_scenarios::test_4345);
    REGISTER_TEST(GUITest_regression_scenarios::test_4352);
    REGISTER_TEST(GUITest_regression_scenarios::test_4356);
    REGISTER_TEST(GUITest_regression_scenarios::test_4359);
    REGISTER_TEST(GUITest_regression_scenarios::test_4368);
    REGISTER_TEST(GUITest_regression_scenarios::test_4373);
    REGISTER_TEST(GUITest_regression_scenarios::test_4377);
    REGISTER_TEST(GUITest_regression_scenarios::test_4383);
    REGISTER_TEST(GUITest_regression_scenarios::test_4386_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4386_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4391);

    REGISTER_TEST(GUITest_regression_scenarios::test_4400);
    REGISTER_TEST(GUITest_regression_scenarios::test_4434);
    REGISTER_TEST(GUITest_regression_scenarios::test_4439);
    REGISTER_TEST(GUITest_regression_scenarios::test_4440);
    REGISTER_TEST(GUITest_regression_scenarios::test_4463);
    REGISTER_TEST(GUITest_regression_scenarios::test_4483);
    REGISTER_TEST(GUITest_regression_scenarios::test_4486);
    REGISTER_TEST(GUITest_regression_scenarios::test_4488);
    REGISTER_TEST(GUITest_regression_scenarios::test_4489);

    REGISTER_TEST(GUITest_regression_scenarios::test_4500);
    REGISTER_TEST(GUITest_regression_scenarios::test_4505);
    REGISTER_TEST(GUITest_regression_scenarios::test_4508);
    REGISTER_TEST(GUITest_regression_scenarios::test_4515);
    REGISTER_TEST(GUITest_regression_scenarios::test_4522);
    REGISTER_TEST(GUITest_regression_scenarios::test_4523);
    REGISTER_TEST(GUITest_regression_scenarios::test_4524);
    REGISTER_TEST(GUITest_regression_scenarios::test_4536);
    REGISTER_TEST(GUITest_regression_scenarios::test_4537);
    REGISTER_TEST(GUITest_regression_scenarios::test_4557);
    REGISTER_TEST(GUITest_regression_scenarios::test_4587);
    REGISTER_TEST(GUITest_regression_scenarios::test_4588);
    REGISTER_TEST(GUITest_regression_scenarios::test_4588_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4588_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4589);
    REGISTER_TEST(GUITest_regression_scenarios::test_4591);
    REGISTER_TEST(GUITest_regression_scenarios::test_4591_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4591_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_4606);
    REGISTER_TEST(GUITest_regression_scenarios::test_4620);
    REGISTER_TEST(GUITest_regression_scenarios::test_4621);
    REGISTER_TEST(GUITest_regression_scenarios::test_4624);
    REGISTER_TEST(GUITest_regression_scenarios::test_4628);
    REGISTER_TEST(GUITest_regression_scenarios::test_4674);
    REGISTER_TEST(GUITest_regression_scenarios::test_4674_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4674_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4676_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4676_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4687);
    REGISTER_TEST(GUITest_regression_scenarios::test_4689_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4689_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4694);
    REGISTER_TEST(GUITest_regression_scenarios::test_4699);

    REGISTER_TEST(GUITest_regression_scenarios::test_4700);
    REGISTER_TEST(GUITest_regression_scenarios::test_4701);
    REGISTER_TEST(GUITest_regression_scenarios::test_4701_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702);
    REGISTER_TEST(GUITest_regression_scenarios::test_4710);
    REGISTER_TEST(GUITest_regression_scenarios::test_4710_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4714_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4714_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4718);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4721);
    REGISTER_TEST(GUITest_regression_scenarios::test_4724);
    REGISTER_TEST(GUITest_regression_scenarios::test_4728);
    REGISTER_TEST(GUITest_regression_scenarios::test_4732);
    REGISTER_TEST(GUITest_regression_scenarios::test_4734);
    REGISTER_TEST(GUITest_regression_scenarios::test_4735);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4779);
    REGISTER_TEST(GUITest_regression_scenarios::test_4782);
    REGISTER_TEST(GUITest_regression_scenarios::test_4783);
    REGISTER_TEST(GUITest_regression_scenarios::test_4784_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4785_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4785_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4795);
    REGISTER_TEST(GUITest_regression_scenarios::test_4799);

    REGISTER_TEST(GUITest_regression_scenarios::test_4803_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4803_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4803_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4803_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_4804_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_4833_8);
    REGISTER_TEST(GUITest_regression_scenarios::test_4839_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4839_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4841);
    REGISTER_TEST(GUITest_regression_scenarios::test_4852);
    REGISTER_TEST(GUITest_regression_scenarios::test_4860);
    REGISTER_TEST(GUITest_regression_scenarios::test_4871);
    REGISTER_TEST(GUITest_regression_scenarios::test_4885_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4885_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4885_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4886);

    REGISTER_TEST(GUITest_regression_scenarios::test_4908);
    REGISTER_TEST(GUITest_regression_scenarios::test_4913);
    REGISTER_TEST(GUITest_regression_scenarios::test_4918);
    REGISTER_TEST(GUITest_regression_scenarios::test_4918_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4934);
    REGISTER_TEST(GUITest_regression_scenarios::test_4936);
    REGISTER_TEST(GUITest_regression_scenarios::test_4938);
    REGISTER_TEST(GUITest_regression_scenarios::test_4938_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4965);
    REGISTER_TEST(GUITest_regression_scenarios::test_4966);
    REGISTER_TEST(GUITest_regression_scenarios::test_4969_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4969_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4981);
    REGISTER_TEST(GUITest_regression_scenarios::test_4983);
    REGISTER_TEST(GUITest_regression_scenarios::test_4985);
    REGISTER_TEST(GUITest_regression_scenarios::test_4986);
    REGISTER_TEST(GUITest_regression_scenarios::test_4996);
    REGISTER_TEST(GUITest_regression_scenarios::test_5000);

    REGISTER_TEST(GUITest_regression_scenarios::test_5004);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5018);
    REGISTER_TEST(GUITest_regression_scenarios::test_5026);

    REGISTER_TEST(GUITest_regression_scenarios::test_5027);
    REGISTER_TEST(GUITest_regression_scenarios::test_5029);
    REGISTER_TEST(GUITest_regression_scenarios::test_5039);
    REGISTER_TEST(GUITest_regression_scenarios::test_5052);
    REGISTER_TEST(GUITest_regression_scenarios::test_5059);
    REGISTER_TEST(GUITest_regression_scenarios::test_5069);
    REGISTER_TEST(GUITest_regression_scenarios::test_5082);
    REGISTER_TEST(GUITest_regression_scenarios::test_5090);

    REGISTER_TEST(GUITest_regression_scenarios::test_5110);
    REGISTER_TEST(GUITest_regression_scenarios::test_5128);
    REGISTER_TEST(GUITest_regression_scenarios::test_5136);
    REGISTER_TEST(GUITest_regression_scenarios::test_5137);
    REGISTER_TEST(GUITest_regression_scenarios::test_5149);
    REGISTER_TEST(GUITest_regression_scenarios::test_5199);

    REGISTER_TEST(GUITest_regression_scenarios::test_5205);
    REGISTER_TEST(GUITest_regression_scenarios::test_5208);
    REGISTER_TEST(GUITest_regression_scenarios::test_5211);
    REGISTER_TEST(GUITest_regression_scenarios::test_5220);
    REGISTER_TEST(GUITest_regression_scenarios::test_5227);
    REGISTER_TEST(GUITest_regression_scenarios::test_5231);
    REGISTER_TEST(GUITest_regression_scenarios::test_5239);
    REGISTER_TEST(GUITest_regression_scenarios::test_5246);
    REGISTER_TEST(GUITest_regression_scenarios::test_5249);
    REGISTER_TEST(GUITest_regression_scenarios::test_5252);
    REGISTER_TEST(GUITest_regression_scenarios::test_5263);
    REGISTER_TEST(GUITest_regression_scenarios::test_5268);
    REGISTER_TEST(GUITest_regression_scenarios::test_5278);
    REGISTER_TEST(GUITest_regression_scenarios::test_5281);
    REGISTER_TEST(GUITest_regression_scenarios::test_5291);

    REGISTER_TEST(GUITest_regression_scenarios::test_5314);
    REGISTER_TEST(GUITest_regression_scenarios::test_5330);
    REGISTER_TEST(GUITest_regression_scenarios::test_5334);
    REGISTER_TEST(GUITest_regression_scenarios::test_5335);
    REGISTER_TEST(GUITest_regression_scenarios::test_5346);
    REGISTER_TEST(GUITest_regression_scenarios::test_5352);
    REGISTER_TEST(GUITest_regression_scenarios::test_5356);
    REGISTER_TEST(GUITest_regression_scenarios::test_5360);
    REGISTER_TEST(GUITest_regression_scenarios::test_5363_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5367);
    REGISTER_TEST(GUITest_regression_scenarios::test_5371);
    REGISTER_TEST(GUITest_regression_scenarios::test_5377);
    REGISTER_TEST(GUITest_regression_scenarios::test_5382);

    REGISTER_TEST(GUITest_regression_scenarios::test_5412);
    REGISTER_TEST(GUITest_regression_scenarios::test_5417);
    REGISTER_TEST(GUITest_regression_scenarios::test_5421);
    REGISTER_TEST(GUITest_regression_scenarios::test_5431);
    REGISTER_TEST(GUITest_regression_scenarios::test_5447_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5447_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5447_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5469);
    REGISTER_TEST(GUITest_regression_scenarios::test_5492);
    REGISTER_TEST(GUITest_regression_scenarios::test_5495);
    REGISTER_TEST(GUITest_regression_scenarios::test_5499);

    REGISTER_TEST(GUITest_regression_scenarios::test_5517);
    REGISTER_TEST(GUITest_regression_scenarios::test_5520_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5550);
    REGISTER_TEST(GUITest_regression_scenarios::test_5562_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5562_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5562_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5588);
    REGISTER_TEST(GUITest_regression_scenarios::test_5594_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5594_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5594_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5594_4);

    REGISTER_TEST(GUITest_regression_scenarios::test_5604);
    REGISTER_TEST(GUITest_regression_scenarios::test_5622);
    REGISTER_TEST(GUITest_regression_scenarios::test_5636);
    REGISTER_TEST(GUITest_regression_scenarios::test_5637);
    REGISTER_TEST(GUITest_regression_scenarios::test_5638);
    REGISTER_TEST(GUITest_regression_scenarios::test_5640);
    REGISTER_TEST(GUITest_regression_scenarios::test_5657);
    REGISTER_TEST(GUITest_regression_scenarios::test_5659);
    REGISTER_TEST(GUITest_regression_scenarios::test_5660);
    REGISTER_TEST(GUITest_regression_scenarios::test_5663);
    REGISTER_TEST(GUITest_regression_scenarios::test_5665);
    REGISTER_TEST(GUITest_regression_scenarios::test_5681);
    REGISTER_TEST(GUITest_regression_scenarios::test_5696);

    REGISTER_TEST(GUITest_regression_scenarios::test_5714_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5714_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5714_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5716);
    REGISTER_TEST(GUITest_regression_scenarios::test_5718);
    REGISTER_TEST(GUITest_regression_scenarios::test_5728);
    REGISTER_TEST(GUITest_regression_scenarios::test_5730);
    REGISTER_TEST(GUITest_regression_scenarios::test_5739);
    REGISTER_TEST(GUITest_regression_scenarios::test_5747);
    REGISTER_TEST(GUITest_regression_scenarios::test_5750);
    REGISTER_TEST(GUITest_regression_scenarios::test_5751);
    REGISTER_TEST(GUITest_regression_scenarios::test_5752);
    REGISTER_TEST(GUITest_regression_scenarios::test_5753);
    REGISTER_TEST(GUITest_regression_scenarios::test_5755);
    REGISTER_TEST(GUITest_regression_scenarios::test_5758);
    REGISTER_TEST(GUITest_regression_scenarios::test_5759);
    REGISTER_TEST(GUITest_regression_scenarios::test_5761);
    REGISTER_TEST(GUITest_regression_scenarios::test_5769_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5769_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5770);
    REGISTER_TEST(GUITest_regression_scenarios::test_5773);
    REGISTER_TEST(GUITest_regression_scenarios::test_5775);
    REGISTER_TEST(GUITest_regression_scenarios::test_5781);
    REGISTER_TEST(GUITest_regression_scenarios::test_5783);
    REGISTER_TEST(GUITest_regression_scenarios::test_5786_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5786_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5786_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5789_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5789_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5790);
    REGISTER_TEST(GUITest_regression_scenarios::test_5798_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5798_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5798_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5798_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_5798_5);

    REGISTER_TEST(GUITest_regression_scenarios::test_5815);
    REGISTER_TEST(GUITest_regression_scenarios::test_5818_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5818_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5832);
    REGISTER_TEST(GUITest_regression_scenarios::test_5833);
    REGISTER_TEST(GUITest_regression_scenarios::test_5837);
    REGISTER_TEST(GUITest_regression_scenarios::test_5840);
    REGISTER_TEST(GUITest_regression_scenarios::test_5842);
    REGISTER_TEST(GUITest_regression_scenarios::test_5847);
    REGISTER_TEST(GUITest_regression_scenarios::test_5849);
    REGISTER_TEST(GUITest_regression_scenarios::test_5851);
    REGISTER_TEST(GUITest_regression_scenarios::test_5853);
    REGISTER_TEST(GUITest_regression_scenarios::test_5854);
    REGISTER_TEST(GUITest_regression_scenarios::test_5855);
    REGISTER_TEST(GUITest_regression_scenarios::test_5872);
    REGISTER_TEST(GUITest_regression_scenarios::test_5898);
    REGISTER_TEST(GUITest_regression_scenarios::test_5899);

    REGISTER_TEST(GUITest_regression_scenarios::test_5903);
    REGISTER_TEST(GUITest_regression_scenarios::test_5905);
    REGISTER_TEST(GUITest_regression_scenarios::test_5941);
    REGISTER_TEST(GUITest_regression_scenarios::test_5947);
    REGISTER_TEST(GUITest_regression_scenarios::test_5948);
    REGISTER_TEST(GUITest_regression_scenarios::test_5950);
    REGISTER_TEST(GUITest_regression_scenarios::test_5970);
    REGISTER_TEST(GUITest_regression_scenarios::test_5972_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5972_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5998);

    REGISTER_TEST(GUITest_regression_scenarios::test_6008);
    REGISTER_TEST(GUITest_regression_scenarios::test_6031);
    REGISTER_TEST(GUITest_regression_scenarios::test_6033);
    REGISTER_TEST(GUITest_regression_scenarios::test_6043);
    REGISTER_TEST(GUITest_regression_scenarios::test_6045);
    REGISTER_TEST(GUITest_regression_scenarios::test_6047);
    REGISTER_TEST(GUITest_regression_scenarios::test_6058);
    REGISTER_TEST(GUITest_regression_scenarios::test_6062);
    REGISTER_TEST(GUITest_regression_scenarios::test_6066);
    REGISTER_TEST(GUITest_regression_scenarios::test_6071);
    REGISTER_TEST(GUITest_regression_scenarios::test_6075);
    REGISTER_TEST(GUITest_regression_scenarios::test_6078);
    REGISTER_TEST(GUITest_regression_scenarios::test_6083);
    REGISTER_TEST(GUITest_regression_scenarios::test_6084);
    REGISTER_TEST(GUITest_regression_scenarios::test_6087);

    REGISTER_TEST(GUITest_regression_scenarios::test_6102);
    REGISTER_TEST(GUITest_regression_scenarios::test_6104);
    REGISTER_TEST(GUITest_regression_scenarios::test_6118);
    REGISTER_TEST(GUITest_regression_scenarios::test_6135);
    REGISTER_TEST(GUITest_regression_scenarios::test_6136);
    REGISTER_TEST(GUITest_regression_scenarios::test_6167);
    REGISTER_TEST(GUITest_regression_scenarios::test_6193);

    REGISTER_TEST(GUITest_regression_scenarios::test_6204);
    REGISTER_TEST(GUITest_regression_scenarios::test_6212);
    REGISTER_TEST(GUITest_regression_scenarios::test_6225);
    REGISTER_TEST(GUITest_regression_scenarios::test_6226);
    REGISTER_TEST(GUITest_regression_scenarios::test_6229);
    REGISTER_TEST(GUITest_regression_scenarios::test_6230);
    REGISTER_TEST(GUITest_regression_scenarios::test_6232_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6232_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6232_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6232_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6233);
    REGISTER_TEST(GUITest_regression_scenarios::test_6235_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6235_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6235_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6235_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6236);
    REGISTER_TEST(GUITest_regression_scenarios::test_6238);
    REGISTER_TEST(GUITest_regression_scenarios::test_6240);
    REGISTER_TEST(GUITest_regression_scenarios::test_6243);
    REGISTER_TEST(GUITest_regression_scenarios::test_6247);
    REGISTER_TEST(GUITest_regression_scenarios::test_6249_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6249_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6249_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6256);
    REGISTER_TEST(GUITest_regression_scenarios::test_6262);
    REGISTER_TEST(GUITest_regression_scenarios::test_6277);
    REGISTER_TEST(GUITest_regression_scenarios::test_6279);
    REGISTER_TEST(GUITest_regression_scenarios::test_6291);
    REGISTER_TEST(GUITest_regression_scenarios::test_6298);

    REGISTER_TEST(GUITest_regression_scenarios::test_6309);
    REGISTER_TEST(GUITest_regression_scenarios::test_6314);
    REGISTER_TEST(GUITest_regression_scenarios::test_6321);
    REGISTER_TEST(GUITest_regression_scenarios::test_6330);
    REGISTER_TEST(GUITest_regression_scenarios::test_6350);
    REGISTER_TEST(GUITest_regression_scenarios::test_6581);
    REGISTER_TEST(GUITest_regression_scenarios::test_6397);
    REGISTER_TEST(GUITest_regression_scenarios::test_6398);

    REGISTER_TEST(GUITest_regression_scenarios::test_6455);
    REGISTER_TEST(GUITest_regression_scenarios::test_6459);
    REGISTER_TEST(GUITest_regression_scenarios::test_6475_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6475_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6479);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6474_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6474_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6485);
    REGISTER_TEST(GUITest_regression_scenarios::test_6488_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6488_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6490);

    REGISTER_TEST(GUITest_regression_scenarios::test_6541_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6541_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6541_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6544);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_8);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_9);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_10);
    REGISTER_TEST(GUITest_regression_scenarios::test_6546_11);
    REGISTER_TEST(GUITest_regression_scenarios::test_6548_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6548_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6564);
    REGISTER_TEST(GUITest_regression_scenarios::test_6566);
    REGISTER_TEST(GUITest_regression_scenarios::test_6569);
    REGISTER_TEST(GUITest_regression_scenarios::test_6580);
    REGISTER_TEST(GUITest_regression_scenarios::test_6586_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6586_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_6616_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6616_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6616_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6616_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6616_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_6619);
    REGISTER_TEST(GUITest_regression_scenarios::test_6620);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_6628_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_6636);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_6640_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_6649);
    REGISTER_TEST(GUITest_regression_scenarios::test_6651);
    REGISTER_TEST(GUITest_regression_scenarios::test_6652);
    REGISTER_TEST(GUITest_regression_scenarios::test_6652_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6654);
    REGISTER_TEST(GUITest_regression_scenarios::test_6655);
    REGISTER_TEST(GUITest_regression_scenarios::test_6656);
    REGISTER_TEST(GUITest_regression_scenarios::test_6659);
    REGISTER_TEST(GUITest_regression_scenarios::test_6667_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6672);
    REGISTER_TEST(GUITest_regression_scenarios::test_6673);
    REGISTER_TEST(GUITest_regression_scenarios::test_6673_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6676_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6676_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6677);
    REGISTER_TEST(GUITest_regression_scenarios::test_6677_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6684);
    REGISTER_TEST(GUITest_regression_scenarios::test_6684_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6685_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6685_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6685_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6685_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6685_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_6689);
    REGISTER_TEST(GUITest_regression_scenarios::test_6691_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6691_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6692);
    REGISTER_TEST(GUITest_regression_scenarios::test_6692_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6692_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6692_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6693);
    REGISTER_TEST(GUITest_regression_scenarios::test_6697);

    REGISTER_TEST(GUITest_regression_scenarios::test_6703);
    REGISTER_TEST(GUITest_regression_scenarios::test_6705);
    REGISTER_TEST(GUITest_regression_scenarios::test_6706);
    REGISTER_TEST(GUITest_regression_scenarios::test_6707);
    REGISTER_TEST(GUITest_regression_scenarios::test_6709);
    REGISTER_TEST(GUITest_regression_scenarios::test_6710);
    REGISTER_TEST(GUITest_regression_scenarios::test_6711);
    REGISTER_TEST(GUITest_regression_scenarios::test_6712);
    REGISTER_TEST(GUITest_regression_scenarios::test_6714);
    REGISTER_TEST(GUITest_regression_scenarios::test_6715);
    REGISTER_TEST(GUITest_regression_scenarios::test_6718);
    REGISTER_TEST(GUITest_regression_scenarios::test_6730);
    REGISTER_TEST(GUITest_regression_scenarios::test_6734);
    REGISTER_TEST(GUITest_regression_scenarios::test_6736_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6736_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6739);
    REGISTER_TEST(GUITest_regression_scenarios::test_6740);
    REGISTER_TEST(GUITest_regression_scenarios::test_6742);
    REGISTER_TEST(GUITest_regression_scenarios::test_6746);
    REGISTER_TEST(GUITest_regression_scenarios::test_6749);
    REGISTER_TEST(GUITest_regression_scenarios::test_6749_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6749_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6749_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6750);
    REGISTER_TEST(GUITest_regression_scenarios::test_6751);
    REGISTER_TEST(GUITest_regression_scenarios::test_6752);
    REGISTER_TEST(GUITest_regression_scenarios::test_6754);
    REGISTER_TEST(GUITest_regression_scenarios::test_6758_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6758_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6759);
    REGISTER_TEST(GUITest_regression_scenarios::test_6760);
    REGISTER_TEST(GUITest_regression_scenarios::test_6797_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6797_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6798);

    REGISTER_TEST(GUITest_regression_scenarios::test_6807);
    REGISTER_TEST(GUITest_regression_scenarios::test_6808);
    REGISTER_TEST(GUITest_regression_scenarios::test_6808_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6809);
    REGISTER_TEST(GUITest_regression_scenarios::test_6816);
    REGISTER_TEST(GUITest_regression_scenarios::test_6826);
    REGISTER_TEST(GUITest_regression_scenarios::test_6847);
    REGISTER_TEST(GUITest_regression_scenarios::test_6860);
    REGISTER_TEST(GUITest_regression_scenarios::test_6862);
    REGISTER_TEST(GUITest_regression_scenarios::test_6872);
    REGISTER_TEST(GUITest_regression_scenarios::test_6875);
    REGISTER_TEST(GUITest_regression_scenarios::test_6897);
    REGISTER_TEST(GUITest_regression_scenarios::test_6897_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6897_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6898);
    REGISTER_TEST(GUITest_regression_scenarios::test_6898_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6898_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6899);
    REGISTER_TEST(GUITest_regression_scenarios::test_6899_1);

    REGISTER_TEST(GUITest_regression_scenarios::test_6901);
    REGISTER_TEST(GUITest_regression_scenarios::test_6903);
    REGISTER_TEST(GUITest_regression_scenarios::test_6903_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6903_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6916);
    REGISTER_TEST(GUITest_regression_scenarios::test_6916_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6924);
    REGISTER_TEST(GUITest_regression_scenarios::test_6926);
    REGISTER_TEST(GUITest_regression_scenarios::test_6927);
    REGISTER_TEST(GUITest_regression_scenarios::test_6935);
    REGISTER_TEST(GUITest_regression_scenarios::test_6941);
    REGISTER_TEST(GUITest_regression_scenarios::test_6952);
    REGISTER_TEST(GUITest_regression_scenarios::test_6953);
    REGISTER_TEST(GUITest_regression_scenarios::test_6954);
    REGISTER_TEST(GUITest_regression_scenarios::test_6959);
    REGISTER_TEST(GUITest_regression_scenarios::test_6960);
    REGISTER_TEST(GUITest_regression_scenarios::test_6963);
    REGISTER_TEST(GUITest_regression_scenarios::test_6966);
    REGISTER_TEST(GUITest_regression_scenarios::test_6968);
    REGISTER_TEST(GUITest_regression_scenarios::test_6971);
    REGISTER_TEST(GUITest_regression_scenarios::test_6979);
    REGISTER_TEST(GUITest_regression_scenarios::test_6981);
    REGISTER_TEST(GUITest_regression_scenarios::test_6990_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6990_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6995);
    REGISTER_TEST(GUITest_regression_scenarios::test_6999);
    REGISTER_TEST(GUITest_regression_scenarios::test_7000);

    REGISTER_TEST(GUITest_regression_scenarios::test_7003);
    REGISTER_TEST(GUITest_regression_scenarios::test_7012);
    REGISTER_TEST(GUITest_regression_scenarios::test_7014);
    REGISTER_TEST(GUITest_regression_scenarios::test_7022);
    REGISTER_TEST(GUITest_regression_scenarios::test_7044);
    REGISTER_TEST(GUITest_regression_scenarios::test_7045);
    REGISTER_TEST(GUITest_regression_scenarios::test_7091);

    REGISTER_TEST(GUITest_regression_scenarios::test_7106);
    REGISTER_TEST(GUITest_regression_scenarios::test_7110);
    REGISTER_TEST(GUITest_regression_scenarios::test_7111);
    REGISTER_TEST(GUITest_regression_scenarios::test_7121);
    REGISTER_TEST(GUITest_regression_scenarios::test_7125);
    REGISTER_TEST(GUITest_regression_scenarios::test_7126);
    REGISTER_TEST(GUITest_regression_scenarios::test_7127);
    REGISTER_TEST(GUITest_regression_scenarios::test_7128);
    REGISTER_TEST(GUITest_regression_scenarios::test_7151);
    REGISTER_TEST(GUITest_regression_scenarios::test_7152);
    REGISTER_TEST(GUITest_regression_scenarios::test_7154);
    REGISTER_TEST(GUITest_regression_scenarios::test_7161);
    REGISTER_TEST(GUITest_regression_scenarios::test_7183);
    REGISTER_TEST(GUITest_regression_scenarios::test_7191);
    REGISTER_TEST(GUITest_regression_scenarios::test_7193);

    REGISTER_TEST(GUITest_regression_scenarios::test_7212);
    REGISTER_TEST(GUITest_regression_scenarios::test_7234);
    REGISTER_TEST(GUITest_regression_scenarios::test_7242);
    REGISTER_TEST(GUITest_regression_scenarios::test_7246);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_6);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_7);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_8);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_9);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_10);
    REGISTER_TEST(GUITest_regression_scenarios::test_7267_11);
    REGISTER_TEST(GUITest_regression_scenarios::test_7247);
    REGISTER_TEST(GUITest_regression_scenarios::test_7276);
    REGISTER_TEST(GUITest_regression_scenarios::test_7279);
    REGISTER_TEST(GUITest_regression_scenarios::test_7290);
    REGISTER_TEST(GUITest_regression_scenarios::test_7293);

    REGISTER_TEST(GUITest_regression_scenarios::test_7338);
    REGISTER_TEST(GUITest_regression_scenarios::test_7367);
    REGISTER_TEST(GUITest_regression_scenarios::test_7368);
    REGISTER_TEST(GUITest_regression_scenarios::test_7371);
    REGISTER_TEST(GUITest_regression_scenarios::test_7380);
    REGISTER_TEST(GUITest_regression_scenarios::test_7384_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7384_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7388);
    REGISTER_TEST(GUITest_regression_scenarios::test_7390);

    REGISTER_TEST(GUITest_regression_scenarios::test_7401);
    REGISTER_TEST(GUITest_regression_scenarios::test_7402);
    REGISTER_TEST(GUITest_regression_scenarios::test_7403);
    REGISTER_TEST(GUITest_regression_scenarios::test_7404_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7404_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7405);
    REGISTER_TEST(GUITest_regression_scenarios::test_7407);
    REGISTER_TEST(GUITest_regression_scenarios::test_7410);
    REGISTER_TEST(GUITest_regression_scenarios::test_7413);
    REGISTER_TEST(GUITest_regression_scenarios::test_7414);
    REGISTER_TEST(GUITest_regression_scenarios::test_7415_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7415_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7415_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_7419);
    REGISTER_TEST(GUITest_regression_scenarios::test_7438);
    REGISTER_TEST(GUITest_regression_scenarios::test_7445);
    REGISTER_TEST(GUITest_regression_scenarios::test_7447);
    REGISTER_TEST(GUITest_regression_scenarios::test_7448_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7448_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7448_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_7448_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_7451);
    REGISTER_TEST(GUITest_regression_scenarios::test_7454);
    REGISTER_TEST(GUITest_regression_scenarios::test_7455);
    REGISTER_TEST(GUITest_regression_scenarios::test_7456);
    REGISTER_TEST(GUITest_regression_scenarios::test_7459);
    REGISTER_TEST(GUITest_regression_scenarios::test_7460);
    REGISTER_TEST(GUITest_regression_scenarios::test_7463);
    REGISTER_TEST(GUITest_regression_scenarios::test_7465);
    REGISTER_TEST(GUITest_regression_scenarios::test_7469);
    REGISTER_TEST(GUITest_regression_scenarios::test_7472);
    REGISTER_TEST(GUITest_regression_scenarios::test_7473_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7473_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7476);
    REGISTER_TEST(GUITest_regression_scenarios::test_7482);
    REGISTER_TEST(GUITest_regression_scenarios::test_7487_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7487_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7490);
    REGISTER_TEST(GUITest_regression_scenarios::test_7491);
    REGISTER_TEST(GUITest_regression_scenarios::test_7499);

    REGISTER_TEST(GUITest_regression_scenarios::test_7504);
    REGISTER_TEST(GUITest_regression_scenarios::test_7505);
    REGISTER_TEST(GUITest_regression_scenarios::test_7506);
    REGISTER_TEST(GUITest_regression_scenarios::test_7507);
    REGISTER_TEST(GUITest_regression_scenarios::test_7508);
    REGISTER_TEST(GUITest_regression_scenarios::test_7509);
    REGISTER_TEST(GUITest_regression_scenarios::test_7511);
    REGISTER_TEST(GUITest_regression_scenarios::test_7515);
    REGISTER_TEST(GUITest_regression_scenarios::test_7517);
    REGISTER_TEST(GUITest_regression_scenarios::test_7520);
    REGISTER_TEST(GUITest_regression_scenarios::test_7521);
    REGISTER_TEST(GUITest_regression_scenarios::test_7531);
    REGISTER_TEST(GUITest_regression_scenarios::test_7535);
    REGISTER_TEST(GUITest_regression_scenarios::test_7539);
    REGISTER_TEST(GUITest_regression_scenarios::test_7540);
    REGISTER_TEST(GUITest_regression_scenarios::test_7542);
    REGISTER_TEST(GUITest_regression_scenarios::test_7546);
    REGISTER_TEST(GUITest_regression_scenarios::test_7548);
    REGISTER_TEST(GUITest_regression_scenarios::test_7550);
    REGISTER_TEST(GUITest_regression_scenarios::test_7555);
    REGISTER_TEST(GUITest_regression_scenarios::test_7556);
    REGISTER_TEST(GUITest_regression_scenarios::test_7558);
    REGISTER_TEST(GUITest_regression_scenarios::test_7572);
    REGISTER_TEST(GUITest_regression_scenarios::test_7573);
    REGISTER_TEST(GUITest_regression_scenarios::test_7574);
    REGISTER_TEST(GUITest_regression_scenarios::test_7575);
    REGISTER_TEST(GUITest_regression_scenarios::test_7576);
    REGISTER_TEST(GUITest_regression_scenarios::test_7582);

    REGISTER_TEST(GUITest_regression_scenarios::test_7607);
    REGISTER_TEST(GUITest_regression_scenarios::test_7609);
    REGISTER_TEST(GUITest_regression_scenarios::test_7616);
    REGISTER_TEST(GUITest_regression_scenarios::test_7617);
    REGISTER_TEST(GUITest_regression_scenarios::test_7623);
    REGISTER_TEST(GUITest_regression_scenarios::test_7629);
    REGISTER_TEST(GUITest_regression_scenarios::test_7630);
    REGISTER_TEST(GUITest_regression_scenarios::test_7631);
    REGISTER_TEST(GUITest_regression_scenarios::test_7633);
    REGISTER_TEST(GUITest_regression_scenarios::test_7635);
    REGISTER_TEST(GUITest_regression_scenarios::test_7644);
    REGISTER_TEST(GUITest_regression_scenarios::test_7645);
    REGISTER_TEST(GUITest_regression_scenarios::test_7650);
    REGISTER_TEST(GUITest_regression_scenarios::test_7652);
    REGISTER_TEST(GUITest_regression_scenarios::test_7659);
    REGISTER_TEST(GUITest_regression_scenarios::test_7661);
    REGISTER_TEST(GUITest_regression_scenarios::test_7667_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7667_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7668);
    REGISTER_TEST(GUITest_regression_scenarios::test_7671);
    REGISTER_TEST(GUITest_regression_scenarios::test_7680);
    REGISTER_TEST(GUITest_regression_scenarios::test_7681);
    REGISTER_TEST(GUITest_regression_scenarios::test_7682);
    REGISTER_TEST(GUITest_regression_scenarios::test_7686);
    REGISTER_TEST(GUITest_regression_scenarios::test_7697);
    REGISTER_TEST(GUITest_regression_scenarios::test_7699);

    REGISTER_TEST(GUITest_regression_scenarios::test_7701);
    REGISTER_TEST(GUITest_regression_scenarios::test_7708);
    REGISTER_TEST(GUITest_regression_scenarios::test_7712);
    REGISTER_TEST(GUITest_regression_scenarios::test_7714);
    REGISTER_TEST(GUITest_regression_scenarios::test_7720);
    REGISTER_TEST(GUITest_regression_scenarios::test_7730);
    REGISTER_TEST(GUITest_regression_scenarios::test_7740);
    REGISTER_TEST(GUITest_regression_scenarios::test_7744);
    REGISTER_TEST(GUITest_regression_scenarios::test_7746);
    REGISTER_TEST(GUITest_regression_scenarios::test_7747);
    REGISTER_TEST(GUITest_regression_scenarios::test_7748);
    REGISTER_TEST(GUITest_regression_scenarios::test_7751);
    REGISTER_TEST(GUITest_regression_scenarios::test_7770);
    REGISTER_TEST(GUITest_regression_scenarios::test_7781);
    REGISTER_TEST(GUITest_regression_scenarios::test_7784);
    REGISTER_TEST(GUITest_regression_scenarios::test_7786);
    REGISTER_TEST(GUITest_regression_scenarios::test_7789);
    REGISTER_TEST(GUITest_regression_scenarios::test_7792);
    REGISTER_TEST(GUITest_regression_scenarios::test_7793);
    REGISTER_TEST(GUITest_regression_scenarios::test_7797);

    REGISTER_TEST(GUITest_regression_scenarios::test_7806);
    REGISTER_TEST(GUITest_regression_scenarios::test_7823);
    REGISTER_TEST(GUITest_regression_scenarios::test_7824);
    REGISTER_TEST(GUITest_regression_scenarios::test_7825);
    REGISTER_TEST(GUITest_regression_scenarios::test_7827);
    REGISTER_TEST(GUITest_regression_scenarios::test_7830);
    REGISTER_TEST(GUITest_regression_scenarios::test_7842);
    REGISTER_TEST(GUITest_regression_scenarios::test_7850);
    REGISTER_TEST(GUITest_regression_scenarios::test_7850_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7852);
    REGISTER_TEST(GUITest_regression_scenarios::test_7853_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_7853_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_7858);
    REGISTER_TEST(GUITest_regression_scenarios::test_7860);
    REGISTER_TEST(GUITest_regression_scenarios::test_7861);
    REGISTER_TEST(GUITest_regression_scenarios::test_7863);
    REGISTER_TEST(GUITest_regression_scenarios::test_7866);
    REGISTER_TEST(GUITest_regression_scenarios::test_7867);
    REGISTER_TEST(GUITest_regression_scenarios::test_7885);
    REGISTER_TEST(GUITest_regression_scenarios::test_7896);

    REGISTER_TEST(GUITest_regression_scenarios::test_7901);
    REGISTER_TEST(GUITest_regression_scenarios::test_7923);
    REGISTER_TEST(GUITest_regression_scenarios::test_7925);
    REGISTER_TEST(GUITest_regression_scenarios::test_7927);
    REGISTER_TEST(GUITest_regression_scenarios::test_7945);
    REGISTER_TEST(GUITest_regression_scenarios::test_7946);
    REGISTER_TEST(GUITest_regression_scenarios::test_7947);
    REGISTER_TEST(GUITest_regression_scenarios::test_7956);
    REGISTER_TEST(GUITest_regression_scenarios::test_7957);
    REGISTER_TEST(GUITest_regression_scenarios::test_7962);
    REGISTER_TEST(GUITest_regression_scenarios::test_7965);
    REGISTER_TEST(GUITest_regression_scenarios::test_7966);
    REGISTER_TEST(GUITest_regression_scenarios::test_7968);
    REGISTER_TEST(GUITest_regression_scenarios::test_7974);
    REGISTER_TEST(GUITest_regression_scenarios::test_7979);

    REGISTER_TEST(GUITest_regression_scenarios::test_8001);
    REGISTER_TEST(GUITest_regression_scenarios::test_8002);
    REGISTER_TEST(GUITest_regression_scenarios::test_8004);
    REGISTER_TEST(GUITest_regression_scenarios::test_8009);
    REGISTER_TEST(GUITest_regression_scenarios::test_8010);
    REGISTER_TEST(GUITest_regression_scenarios::test_8017);
    REGISTER_TEST(GUITest_regression_scenarios::test_8018);
    REGISTER_TEST(GUITest_regression_scenarios::test_8015);
    REGISTER_TEST(GUITest_regression_scenarios::test_8028);
    REGISTER_TEST(GUITest_regression_scenarios::test_8037);
    REGISTER_TEST(GUITest_regression_scenarios::test_8040);
    REGISTER_TEST(GUITest_regression_scenarios::test_8049);
    REGISTER_TEST(GUITest_regression_scenarios::test_8052);
    REGISTER_TEST(GUITest_regression_scenarios::test_8064);
    REGISTER_TEST(GUITest_regression_scenarios::test_8069);
    REGISTER_TEST(GUITest_regression_scenarios::test_8074);
    REGISTER_TEST(GUITest_regression_scenarios::test_8077_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_8077_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_8079_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_8079_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_8079_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_8083_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_8083_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_8090);
    REGISTER_TEST(GUITest_regression_scenarios::test_8092);
    REGISTER_TEST(GUITest_regression_scenarios::test_8093);
    REGISTER_TEST(GUITest_regression_scenarios::test_8097);
    REGISTER_TEST(GUITest_regression_scenarios::test_8096_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_8096_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_8096_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_8100);
    REGISTER_TEST(GUITest_regression_scenarios::test_8101);
    REGISTER_TEST(GUITest_regression_scenarios::test_8104);
    REGISTER_TEST(GUITest_regression_scenarios::test_8111);
    REGISTER_TEST(GUITest_regression_scenarios::test_8114);
    REGISTER_TEST(GUITest_regression_scenarios::test_8116);
    REGISTER_TEST(GUITest_regression_scenarios::test_8118);
    REGISTER_TEST(GUITest_regression_scenarios::test_8120_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_8120_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_8136);
    REGISTER_TEST(GUITest_regression_scenarios::test_8141);
    REGISTER_TEST(GUITest_regression_scenarios::test_8151);
    REGISTER_TEST(GUITest_regression_scenarios::test_8159);
    REGISTER_TEST(GUITest_regression_scenarios::test_8153);
    REGISTER_TEST(GUITest_regression_scenarios::test_8160);
    REGISTER_TEST(GUITest_regression_scenarios::test_8161);
    REGISTER_TEST(GUITest_regression_scenarios::test_8163);
    REGISTER_TEST(GUITest_regression_scenarios::test_8170);
    REGISTER_TEST(GUITest_regression_scenarios::test_8174);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0023);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0030);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0031);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0033);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0035);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0036);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0037);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0038_1);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0039);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0040);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0045);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0046);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0047);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0048);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0049);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0050);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0051);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0052);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0053);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0054);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0055);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0056);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0057);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0058);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0059);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0060);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0061);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0062);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0063);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0064);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0065);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0066);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0067);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0068);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0069);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0070);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0071);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0072);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0073);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0074);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/bookmarks/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_bookmarks::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_bookmarks::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_project_bookmarks::test_0002_2);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/multiple docs/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_multiple_docs::test_0001);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/anonymous project/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_anonymous_project::test_0003);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/relations/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_relations::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_relations::test_0002);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/user locking/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_user_locking::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_user_locking::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_user_locking::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_project_user_locking::test_0005);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/document modifying/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_document_modifying::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_document_modifying::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_project_document_modifying::test_0002);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/project_filtering/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0014);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/sanger/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0005_2);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0005_3);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0005_4);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_sanger::test_0008);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/Sequence view/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0002_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0003_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0006_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0018);

    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0022);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0029);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0030);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0031);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0031_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0031_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0031_3);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0032);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0035);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0036);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0037);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0037_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0040);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0041);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0044);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0044_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0044_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0045);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0046);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0047);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0048);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0050);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0051);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0052);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0052_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0053);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0054);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0055);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0056);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0057);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0058);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0059);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0059_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0060);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0061_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0061_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0061_3);

    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0062);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0063);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0064);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0065);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0066);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0067);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0068);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0069);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0070);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0071);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0075);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0076);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0077);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0078);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0079_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0079_2);

    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0080);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0081);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0082);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0083);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0084);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0085);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0086);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0087);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/sequence edit/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0013_1_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0013_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0013_2_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0014_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0014_1_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0014_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0014_2_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0015_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0015_1_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0015_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0015_2_neg);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0016_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0016_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0021_1);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0021_2);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit::test_0021_3);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/sequence edit mode/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::without_anns_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::without_anns_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::without_anns_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::without_anns_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_sequence_edit_mode::with_anns_test_0008);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/sequence selection/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::double_click_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::double_click_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::double_click_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::double_click_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::mixed_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::mixed_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::mixed_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::one_click_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::one_click_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::one_click_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::one_click_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_sequence_selection::one_click_test_0005);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/remote request/
    //////////////////////////////////////////////////////////////////////////

    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_1)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_2)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_3)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_4)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_5)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_6)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0016_7)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0017)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0018)
    REGISTER_TEST(GUITest_common_scenarios_project_remote_request::test_0019)

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/toggle view/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0001_3);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0001_4);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0002_2);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0002_3);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0002_4);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0003_2);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0003_3);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0003_4);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0004_2);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0004_3);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0004_4);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0009);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0011_2);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0012_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0012_2);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0013_2);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0014_1);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0014_2);

    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_toggle_view::test_0016);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/sequence exporting/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting::test_0013);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/project/sequence exporting/from project view/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0004);

    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0006);

    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0007_2);

    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_project_sequence_exporting_from_project_view::test_0008_2);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0001_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0001_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0002_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0002_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0002_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0003);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0005_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0009_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0009_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0010_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0010_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0011_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0012);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0013_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0014_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0014_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0016);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0019);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0020_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0021_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0021_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0022);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0022_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0022_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0023);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0026_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0026_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0027_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0027_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0029);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0029_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0029_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0031);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0031_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0031_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0031_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0031_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0032);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0032_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0033);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0033_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0034_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0035);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0035_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0036);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0036_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0036_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0036_3);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0037);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0037_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0037_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0038_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0038_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0038_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0038_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_4);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_5);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_6);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0039_7);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0040);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0041);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0042_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0042_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0044);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0045);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0045_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0046);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0047);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0048);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0049);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0050);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0052);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_4);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_5);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_6);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0054);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0054_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0055);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0056);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0057);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0059);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0060);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0061);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0062);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0063);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0064);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0065);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0066);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0069);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0070);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0071);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0072);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0073);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0074);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0075);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0076);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0077);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0078);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0079);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0080);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0081);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0082);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0083);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0090);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0091);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0092);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0093_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0093_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0094);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0095);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0096);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/exclude_list
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_exclude_list::test_0009);

    /////////////////////////////////////////////////////////////////////////
    // Common align sequences to an alignment
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0016_1);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0016_2);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0016_3);
    REGISTER_TEST(GUITest_common_scenarios_align_sequences_to_msa::test_0017);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/colors
    /////////////////////////////////////////////////////////////////////////

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_colors::test_0007);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/consensus
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0001);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0002_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0003_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_consensus::test_0006);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/tree
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_tree::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_tree::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_tree::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_tree::test_0003_2);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/edit
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0001_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0003_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0006_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0007_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0008_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0009_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0009_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0010);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0011_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0011_3);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0012);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0013_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0015);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor_edit::test_0016);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/replace_character
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_replace_character::test_0018);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/overview
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor_overview::test_0022);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/msa_editor/multiline
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::general_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::zoom_to_selection_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::vscroll_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::vscroll_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::menu_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::goto_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::overview_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::overview_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::overview_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::overview_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::keys_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::keys_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::consensus_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::similarity_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::image_export_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::drag_n_drop_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::bookmark_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::bookmark_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::bookmark_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::exclude_list_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::exclude_list_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::replace_character_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::replace_character_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::replace_character_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::replace_character_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::replace_character_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline::edit_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_options::general_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_options::general_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_options::statistic_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_options::highlighting_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_options::search_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_colors::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_colors::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_MSA_editor_multiline_colors::test_0003);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/mca_editor
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0012_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0012_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0013_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0013_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0015_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0015_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0016_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0016_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0017_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0017_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0018_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0018_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0022_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0022_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0022_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0023_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0023_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0023_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0024_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0024_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0024_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0025_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0025_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0025_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0026_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0026_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0026_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0027_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0027_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0027_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0029);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0030);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0033);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0039);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0040_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0040_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0040_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0041);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0044);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0045_1);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0045_2);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0045_3);
    REGISTER_TEST(GUITest_common_scenarios_mca_editor::test_0045_4);

    //////////////////////////////////////////////////////////////////////////
    // Common scenarios/document_from_text/
    //////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0001_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0002_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0003_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0006_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0007_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0008_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0009_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0009_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0010_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0010_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0011_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0012_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0012_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0013_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0013_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0014_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0014_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0015_1);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0015_2);

    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_document_from_text::test_0019);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Annotations import
    /////////////////////////////////////////////////////////////////////////

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0001_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0002_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0003_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0006_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0007_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0008_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0009_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_import::test_0009_2);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/annotations
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0001_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0002_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0009);

    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0010_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0010_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0010_3);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0011_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0011_3);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0012_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0012_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0012_3);
    REGISTER_TEST(GUITest_common_scenarios_annotations::test_0013);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/annotations/CreateAnnotationWidget
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0022);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0023);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0029);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0030);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0031);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0032);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0033);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0035);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0036);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0037);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0039);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0040);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0041);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0044);
    REGISTER_TEST(GUITest_common_scenarios_create_annotation_widget::test_0045);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/annotations/edit
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0002_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0003_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0004_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0005_2);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_edit::test_0006_2);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/annotations/qualifiers
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0001_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0002_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0003_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0004);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0006);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0007_2);

    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0009);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/smith_waterman_dialog
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_sw_dialog::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_sw_dialog::test_0002);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/option_panel
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_options_panel::test_0022);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/option_panel
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::general_test_0005);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0001_3);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0001_4);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_2);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_3);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_4);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_5);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_6);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_7);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_8);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0004_9);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0009_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0010_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0011);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0012);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::highlighting_test_0013);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0007_2);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0007_3);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::pairwise_alignment_test_0011);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0008);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::export_consensus_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::export_consensus_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::export_consensus_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::export_consensus_test_0004);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::statistics_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::statistics_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::statistics_test_0003);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_MSA::save_parameters_test_0006);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/dp_view
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0011);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0011_1);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0011_2);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0011_3);

    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0013);

    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0014);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0014_1);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0014_2);

    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0020);
    REGISTER_TEST(GUITest_Common_scenarios_dp_view::test_0025);
    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/genecut
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_genecut::test_0012);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/fasttree
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_fasttree::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_fasttree::test_0002);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/iqtree
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_iqtree::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_iqtree::test_0002);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/Assembly browser
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_Assembly_browser::test_0001);
    REGISTER_TEST(GUITest_Assembly_browser::test_0002);
    REGISTER_TEST(GUITest_Assembly_browser::test_0010);
    REGISTER_TEST(GUITest_Assembly_browser::test_0011);
    REGISTER_TEST(GUITest_Assembly_browser::test_0012);
    REGISTER_TEST(GUITest_Assembly_browser::test_0013);
    REGISTER_TEST(GUITest_Assembly_browser::test_0014);
    REGISTER_TEST(GUITest_Assembly_browser::test_0015);
    REGISTER_TEST(GUITest_Assembly_browser::test_0016);
    REGISTER_TEST(GUITest_Assembly_browser::test_0017);
    REGISTER_TEST(GUITest_Assembly_browser::test_0018);
    REGISTER_TEST(GUITest_Assembly_browser::test_0019);
    REGISTER_TEST(GUITest_Assembly_browser::test_0020);
    REGISTER_TEST(GUITest_Assembly_browser::test_0021);
    REGISTER_TEST(GUITest_Assembly_browser::test_0022);
    REGISTER_TEST(GUITest_Assembly_browser::test_0023);
    REGISTER_TEST(GUITest_Assembly_browser::test_0024);
    REGISTER_TEST(GUITest_Assembly_browser::test_0025);
    REGISTER_TEST(GUITest_Assembly_browser::test_0026_1);
    REGISTER_TEST(GUITest_Assembly_browser::test_0026_2);
    REGISTER_TEST(GUITest_Assembly_browser::test_0026_3);
    REGISTER_TEST(GUITest_Assembly_browser::test_0027);
    REGISTER_TEST(GUITest_Assembly_browser::test_0028);
    REGISTER_TEST(GUITest_Assembly_browser::test_0029);
    REGISTER_TEST(GUITest_Assembly_browser::test_0030);
    REGISTER_TEST(GUITest_Assembly_browser::test_0031);
    REGISTER_TEST(GUITest_Assembly_browser::test_0032);
    REGISTER_TEST(GUITest_Assembly_browser::test_0033);
    REGISTER_TEST(GUITest_Assembly_browser::test_0034);
    REGISTER_TEST(GUITest_Assembly_browser::test_0035);
    REGISTER_TEST(GUITest_Assembly_browser::test_0036);
    REGISTER_TEST(GUITest_Assembly_browser::test_0037);
    REGISTER_TEST(GUITest_Assembly_browser::test_0038);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/bowtie2
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/dna_assembly
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_dna_assembly::test_0001);
    REGISTER_TEST(GUITest_dna_assembly::test_0002);
    REGISTER_TEST(GUITest_dna_assembly::test_0003);
    REGISTER_TEST(GUITest_dna_assembly::test_0004);
    REGISTER_TEST(GUITest_dna_assembly::test_0005);
    REGISTER_TEST(GUITest_dna_assembly::test_0006);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/index_reuse
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_index_reuse::test_0001);
    REGISTER_TEST(GUITest_index_reuse::test_0002);
    REGISTER_TEST(GUITest_index_reuse::test_0003);
    REGISTER_TEST(GUITest_index_reuse::test_0004);
    REGISTER_TEST(GUITest_index_reuse::test_0005);
    REGISTER_TEST(GUITest_index_reuse::test_0006);
    REGISTER_TEST(GUITest_index_reuse::test_0007);
    REGISTER_TEST(GUITest_index_reuse::test_0008);
    REGISTER_TEST(GUITest_index_reuse::test_0009);
    REGISTER_TEST(GUITest_index_reuse::test_0010);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/dna_assembly/conversions
    /////////////////////////////////////////////////////////////////////////

    REGISTER_TEST(GUITest_dna_assembly_conversions::test_0002);
    REGISTER_TEST(GUITest_dna_assembly_conversions::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/extract_consensus
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_assembly_extract_consensus::test_0001_single_input);
    REGISTER_TEST(GUITest_assembly_extract_consensus::test_0002_multiple_input);
    REGISTER_TEST(GUITest_assembly_extract_consensus::test_0003_wrong_input);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/sam
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_SAM::test_0002);
    REGISTER_TEST(GUITest_SAM::test_0003);
    REGISTER_TEST(GUITest_SAM::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Query designer
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer
    /////////////////////////////////////////////////////////////////////////

    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0003);

    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0017);

    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0058);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0059);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0060);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0061);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0062);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Workflow parameters validation
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_parameters_validation::test_0001);

    REGISTER_TEST(GUITest_common_scenarios_workflow_parameters_validation::test_0003);

    REGISTER_TEST(GUITest_common_scenarios_workflow_parameters_validation::test_0006);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Dashboard misc
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0004);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0004);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0011);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0012);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0013);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0015);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0016);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0017);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::view_opening_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::view_opening_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::view_opening_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::view_opening_test_0004);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0008);
    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Elements
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_trimmomatic_element::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_trimmomatic_element::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_trimmomatic_element::test_0003);
    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Estimating
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_estimating::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_estimating::test_0002);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Name filter
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_name_filter::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_name_filter::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_name_filter::test_0003);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Samples
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_samples::test_0001);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Scripting
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Tree viewer
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0002_2);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0011_1);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0011_2);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0029);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0030);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0031);
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0032);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Repeat Finder
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_repeat_finder::test_0001);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Undo_Redo
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0005);

    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0006_1);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0006_2);

    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0007_2);

    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0010);

    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0011_1);

    REGISTER_TEST(GUITest_common_scenarios_undo_redo::test_0012);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/circular_view
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_1);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_2);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_3);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_4);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_5);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_6);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_7);
    REGISTER_TEST(GUITest_common_scenarios_circular_view::general_avail_8);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/cloning
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_cloning::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_cloning::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_cloning::test_0013);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/options_panel/sequence_view
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0010);

    REGISTER_TEST(GUITest_common_scenarios_options_panel_sequence_view::test_0011);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/pcr/in_silico_pcr
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0010);

    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_in_silico_pcr::test_0021);
    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/pcr/primer_library
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0017);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/phyml
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_phyml::test_0006);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/primer3
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_main_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_general_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_advanced_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_internal_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_penalty_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_tab_quality_all);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0016);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0018);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0020);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0021);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0022);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0023);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_primer3::test_0028);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/start_page
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0008);

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/external_tools/mfold
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0001_success);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0002_fail);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0003_limits);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0004_region);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0005_large);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0006_html_name);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0007_save_dialog);
    REGISTER_TEST(GUITest_common_scenarios_mfold::test_0008_save_dialog);

    // Query designer.
    REGISTER_TEST(GUITest_common_scenarios_query_designer::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_query_designer::test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_query_designer::test_0001_2);

    // Workspace.
    REGISTER_TEST(GUITest_common_scenarios_workspace::test_0001)
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
