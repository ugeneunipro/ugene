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

#include <U2Core/AppContext.h>

#include <U2Gui/ToolsMenu.h>

#include <U2Test/UGUITestBase.h>

#include "GUITestBasePlugin.h"
#include "tests/PosteriorActions.h"
#include "tests/PosteriorChecks.h"
#include "tests/PreliminaryActions.h"
#include "tests/common_scenarios/Assembling/Assembly_browser/GTTestsAssemblyBrowser.h"
#include "tests/common_scenarios/Assembling/bowtie2/GTTestsBowtie2.h"
#include "tests/common_scenarios/Assembling/dna_assembly/GTTestsDnaAssembly.h"
#include "tests/common_scenarios/Assembling/dna_assembly/GTTestsIndexReuse.h"
#include "tests/common_scenarios/Assembling/dna_assembly/conversions/GTTestsDnaAssemblyConversions.h"
#include "tests/common_scenarios/Assembling/sam/GTTestsSAM.h"
#include "tests/common_scenarios/NIAID_pipelines/GTTestsNiaidPipelines.h"
#include "tests/common_scenarios/Query_Designer/GTTestsQuerryDesigner.h"
#include "tests/common_scenarios/annotations/GTTestsAnnotations.h"
#include "tests/common_scenarios/annotations/GTTestsCreateAnnotationWidget.h"
#include "tests/common_scenarios/annotations/edit/GTTestsAnnotationsEdit.h"
#include "tests/common_scenarios/annotations/qualifiers/GTTestsAnnotationsQualifiers.h"
#include "tests/common_scenarios/annotations_import/GTTestsAnnotationsImport.h"
#include "tests/common_scenarios/circular_view/GTTestsCvGeneral.h"
#include "tests/common_scenarios/cloning/GTTestsCloning.h"
#include "tests/common_scenarios/document_from_text/GTTestsDocumentFromText.h"
#include "tests/common_scenarios/dp_view/GTTestsDpView.h"
#include "tests/common_scenarios/mca_editor/GTTestsMcaEditor.h"
#include "tests/common_scenarios/msa_editor/GTTestsMsaEditor.h"
#include "tests/common_scenarios/msa_editor/align/GTTestsAlignSequenceToMsa.h"
#include "tests/common_scenarios/msa_editor/colors/GTTestsMSAEditorColors.h"
#include "tests/common_scenarios/msa_editor/consensus/GTTestsMSAEditorConsensus.h"
#include "tests/common_scenarios/msa_editor/edit/GTTestsMSAEditorEdit.h"
#include "tests/common_scenarios/msa_editor/overview/GTTestsMSAEditorOverview.h"
#include "tests/common_scenarios/msa_editor/replace_character/GTTestsMSAEditorReplaceCharacter.h"
#include "tests/common_scenarios/ngs_classification/metaphlan2/GTTestsMetaPhlAn2.h"
#include "tests/common_scenarios/options_panel/GTTestsOptionPanel.h"
#include "tests/common_scenarios/options_panel/msa/GTTestsOptionPanelMSA.h"
#include "tests/common_scenarios/options_panel/sequence_view/GTTestsOptionPanelSequenceView.h"
#include "tests/common_scenarios/pcr/GTTestsInSilicoPcr.h"
#include "tests/common_scenarios/pcr/GTTestsPrimerLibrary.h"
#include "tests/common_scenarios/phyml/GTTestsCommonScenariosPhyml.h"
#include "tests/common_scenarios/project/GTTestsProject.h"
#include "tests/common_scenarios/project/anonymous_project/GTTestsProjectAnonymousProject.h"
#include "tests/common_scenarios/project/bookmarks/GTTestsBookmarks.h"
#include "tests/common_scenarios/project/document_modifying/GTTestsProjectDocumentModifying.h"
#include "tests/common_scenarios/project/multiple_docs/GTTestsProjectMultipleDocs.h"
#include "tests/common_scenarios/project/project_filtering/GTTestsProjectFiltering.h"
#include "tests/common_scenarios/project/relations/GTTestsProjectRelations.h"
#include "tests/common_scenarios/project/remote_request/GTTestsProjectRemoteRequest.h"
#include "tests/common_scenarios/project/sequence_exporting/GTTestsProjectSequenceExporting.h"
#include "tests/common_scenarios/project/sequence_exporting/GTTestsProjectSequenceExporting.h"
#include "tests/common_scenarios/project/sequence_exporting/from_project_view/GTTestsFromProjectView.h"
#include "tests/common_scenarios/project/user_locking/GTTestsProjectUserLocking.h"
#include "tests/common_scenarios/repeat_finder/GTTestsRepeatFinder.h"
#include "tests/common_scenarios/sanger/GTTestsSanger.h"
#include "tests/common_scenarios/sequence_edit/GTTestsSequenceEdit.h"
#include "tests/common_scenarios/sequence_edit/GTTestsSequenceEditMode.h"
#include "tests/common_scenarios/sequence_selection/GTTestsSequenceSelection.h"
#include "tests/common_scenarios/sequence_view/GTTestsSequenceView.h"
#include "tests/common_scenarios/shared_database/GTTestsSharedDatabase.h"
#include "tests/common_scenarios/smith_waterman_dialog/GTTestsSWDialog.h"
#include "tests/common_scenarios/start_page/GTTestsStartPage.h"
#include "tests/common_scenarios/toggle_view/GTTestsToggleView.h"
#include "tests/common_scenarios/tree_viewer/GTTestsCommonScenariousTreeviewer.h"
#include "tests/common_scenarios/undo_redo/GTTestsUndoRedo.h"
#include "tests/common_scenarios/workflow_designer/GTTestsWorkflowDesigner.h"
#include "tests/common_scenarios/workflow_designer/dashboard/GTTestsWorkflowDashboard.h"
#include "tests/common_scenarios/workflow_designer/estimating/GTTestsWorkflowEstimating.h"
#include "tests/common_scenarios/workflow_designer/name_filter/GTTestsWorkflowNameFilter.h"
#include "tests/common_scenarios/workflow_designer/parameters_validation/GTTestsWorkflowParameterValidation.h"
#include "tests/common_scenarios/workflow_designer/scripting/GTTestsWorkflowScripting.h"
#include "tests/common_scenarios/workflow_designer/shared_db/GTTestsSharedDbWd.h"
#include "tests/crazy_user/GUICrazyUserTest.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_1001_2000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_1_1000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_2001_3000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_3001_4000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_4001_5000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_5001_6000.h"
#include "tests/regression_scenarios/GTTestsRegressionScenarios_6001_7000.h"

#define REGISTER_TEST(X) if (guiTestBase) guiTestBase->registerTest(new X())
#define REGISTER_TEST_LABEL(X, LABEL) \
    if (guiTestBase) { \
        HI::GUITest *test = new X(); \
        test->setLabel(LABEL); \
        guiTestBase->registerTest(test); \
        }
#define REGISTER_TEST_WITH_TIMEOUT(X, TIMEOUT) \
    if (guiTestBase) { \
        HI::GUITest *test = new X(); \
        test->setTimeout(TIMEOUT); \
        guiTestBase->registerTest(test); \
        }
#define REGISTER_TEST_IGNORED_BY(X, BY, MESSAGE, reason) \
    if (guiTestBase) { \
        HI::GUITest *test = new X(); \
        test->setIgnored(BY, MESSAGE); \
        test->setReason(reason); \
        guiTestBase->registerTest(test); \
        }

#define REGISTER_TEST_IGNORED(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::Ignored, MESSAGE, HI::GUITest::Bug)
#define REGISTER_TEST_IGNORED_LINUX(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredLinux, MESSAGE, HI::GUITest::Bug)
#define REGISTER_TEST_IGNORED_WINDOWS(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows, MESSAGE, HI::GUITest::Bug)
#define REGISTER_TEST_IGNORED_MAC(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredMac, MESSAGE, HI::GUITest::Bug)

#define REGISTER_TEST_NOT_FOR_LINUX(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredLinux, "not for Linux", HI::GUITest::System)
#define REGISTER_TEST_NOT_FOR_WINDOWS(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows, "not for Windows", HI::GUITest::System)
#define REGISTER_TEST_NOT_FOR_MAC(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredMac, "not for Mac", HI::GUITest::System)

#define REGISTER_TEST_LINUX(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows|HI::GUITest::IgnoredMac, MESSAGE, HI::GUITest::Bug)
#define REGISTER_TEST_WINDOWS(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredMac|HI::GUITest::IgnoredLinux, MESSAGE, HI::GUITest::Bug)
#define REGISTER_TEST_MAC(X, MESSAGE) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows|HI::GUITest::IgnoredLinux, MESSAGE, HI::GUITest::Bug)

#define REGISTER_TEST_ONLY_LINUX(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows|HI::GUITest::IgnoredMac, "only for Linux", HI::GUITest::System)
#define REGISTER_TEST_ONLY_WINDOWS(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredMac|HI::GUITest::IgnoredLinux, "only for Windows", HI::GUITest::System)
#define REGISTER_TEST_ONLY_MAC(X) REGISTER_TEST_IGNORED_BY(X, HI::GUITest::IgnoredWindows|HI::GUITest::IgnoredLinux, "only for Mac", HI::GUITest::System)


namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        GUITestBasePlugin* plug = new GUITestBasePlugin();
        return plug;
    }
    return NULL;
}

GUITestBasePlugin::GUITestBasePlugin() : Plugin(tr("GUITestBase"), tr("GUI Test Base")) {

    UGUITestBase *guiTestBase = AppContext::getGUITestBase();

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
    if (view == NULL) {
        view = new GUITestRunner(AppContext::getGUITestBase());
        view->show();
    } else {
        view->raise();
    }
}

void GUITestBasePlugin::registerTests(UGUITestBase *guiTestBase) {

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

    REGISTER_TEST_MAC(GUITest_regression_scenarios::test_0339, "Mac specific test");
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
    REGISTER_TEST(GUITest_regression_scenarios::test_0829);
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
    REGISTER_TEST_NOT_FOR_MAC(GUITest_regression_scenarios::test_0889);//"Spidey tool is not avaluable on mac"
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
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1003, "too long test");
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
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1029, "UGENE-4076");
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
    REGISTER_TEST_LABEL(GUITest_regression_scenarios::test_1166, "mem");
    REGISTER_TEST(GUITest_regression_scenarios::test_1172);
    REGISTER_TEST(GUITest_regression_scenarios::test_1175);
    REGISTER_TEST(GUITest_regression_scenarios::test_1180);
    REGISTER_TEST(GUITest_regression_scenarios::test_1182);
    REGISTER_TEST(GUITest_regression_scenarios::test_1184);
    REGISTER_TEST(GUITest_regression_scenarios::test_1186_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1186_2);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1189, "UGENE-3830");
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1189_1, "UGENE-3830");
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
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1249, "TODO: add the 3, 4 steps");
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
    REGISTER_TEST_ONLY_MAC(GUITest_regression_scenarios::test_1551);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_1594);
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
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1640);
    REGISTER_TEST(GUITest_regression_scenarios::test_1643);
    REGISTER_TEST(GUITest_regression_scenarios::test_1645);
    REGISTER_TEST(GUITest_regression_scenarios::test_1651);
    REGISTER_TEST(GUITest_regression_scenarios::test_1653);
    REGISTER_TEST(GUITest_regression_scenarios::test_1654);
    REGISTER_TEST(GUITest_regression_scenarios::test_1658);
    REGISTER_TEST(GUITest_regression_scenarios::test_1660);
    REGISTER_TEST(GUITest_regression_scenarios::test_1661);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1662);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1664);
    REGISTER_TEST(GUITest_regression_scenarios::test_1668);
    REGISTER_TEST(GUITest_regression_scenarios::test_1672);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_1673_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_1677);
    REGISTER_TEST_ONLY_MAC(GUITest_regression_scenarios::test_1680);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1681);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1681_1);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1681_2);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1681_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_1686);
    REGISTER_TEST(GUITest_regression_scenarios::test_1687);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1688, "big data");
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1693);//no tuxedo for windows

    REGISTER_TEST(GUITest_regression_scenarios::test_1700);
    REGISTER_TEST_NOT_FOR_LINUX(GUITest_regression_scenarios::test_1701);//virtual display can not show 3d view
    REGISTER_TEST(GUITest_regression_scenarios::test_1703);//, "UGENE-3693"
    REGISTER_TEST(GUITest_regression_scenarios::test_1704);
    REGISTER_TEST(GUITest_regression_scenarios::test_1708);
    REGISTER_TEST(GUITest_regression_scenarios::test_1710_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1710_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1714);
    REGISTER_TEST(GUITest_regression_scenarios::test_1720);
    REGISTER_TEST(GUITest_regression_scenarios::test_1731);
    REGISTER_TEST(GUITest_regression_scenarios::test_1733);
    REGISTER_TEST(GUITest_regression_scenarios::test_1734);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1735);
    REGISTER_TEST(GUITest_regression_scenarios::test_1738);
    REGISTER_TEST(GUITest_regression_scenarios::test_1747);
    REGISTER_TEST(GUITest_regression_scenarios::test_1751);
    REGISTER_TEST(GUITest_regression_scenarios::test_1756);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_1884);//, "UGENE-3693"
    REGISTER_TEST(GUITest_regression_scenarios::test_1886_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_1886_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_1897);

    REGISTER_TEST(GUITest_regression_scenarios::test_1908);
    REGISTER_TEST(GUITest_regression_scenarios::test_1918);
    REGISTER_TEST(GUITest_regression_scenarios::test_1919);
    REGISTER_TEST(GUITest_regression_scenarios::test_1921);
    REGISTER_TEST(GUITest_regression_scenarios::test_1924);
    //REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_1946);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_1946, "Not finished. Requires large data.");
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
    REGISTER_TEST(GUITest_regression_scenarios::test_2021_9);
    REGISTER_TEST(GUITest_regression_scenarios::test_2026);
    REGISTER_TEST(GUITest_regression_scenarios::test_2030);
    REGISTER_TEST(GUITest_regression_scenarios::test_2032);
    REGISTER_TEST(GUITest_regression_scenarios::test_2049);
    REGISTER_TEST(GUITest_regression_scenarios::test_2053);
    REGISTER_TEST(GUITest_regression_scenarios::test_2076);
    REGISTER_TEST(GUITest_regression_scenarios::test_2077);
    REGISTER_TEST(GUITest_regression_scenarios::test_2078);
    REGISTER_TEST_ONLY_WINDOWS(GUITest_regression_scenarios::test_2089);//, "no forbidden folder characters on linux and mac");
    REGISTER_TEST(GUITest_regression_scenarios::test_2093_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2091);
    REGISTER_TEST(GUITest_regression_scenarios::test_2093_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2100_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2100_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_2124);
    REGISTER_TEST(GUITest_regression_scenarios::test_2128);
    REGISTER_TEST(GUITest_regression_scenarios::test_2128_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2138);
    REGISTER_TEST_NOT_FOR_MAC(GUITest_regression_scenarios::test_2140);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_2204);
    REGISTER_TEST(GUITest_regression_scenarios::test_2225);
    REGISTER_TEST(GUITest_regression_scenarios::test_2259);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2266_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2267_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2267_2);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_2268, "UGENE-4948");
    REGISTER_TEST(GUITest_regression_scenarios::test_2269);
    REGISTER_TEST(GUITest_regression_scenarios::test_2270);
    REGISTER_TEST(GUITest_regression_scenarios::test_2281);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2282);
    REGISTER_TEST(GUITest_regression_scenarios::test_2284);
    REGISTER_TEST(GUITest_regression_scenarios::test_2285);
    REGISTER_TEST(GUITest_regression_scenarios::test_2270);
    REGISTER_TEST(GUITest_regression_scenarios::test_2292);
    REGISTER_TEST(GUITest_regression_scenarios::test_2295);
    REGISTER_TEST(GUITest_regression_scenarios::test_2298);
    REGISTER_TEST(GUITest_regression_scenarios::test_2293);
    REGISTER_TEST(GUITest_regression_scenarios::test_2306);
    REGISTER_TEST(GUITest_regression_scenarios::test_2309);
    REGISTER_TEST(GUITest_regression_scenarios::test_2314);
    REGISTER_TEST(GUITest_regression_scenarios::test_2316);
    REGISTER_TEST(GUITest_regression_scenarios::test_2318);
    REGISTER_TEST(GUITest_regression_scenarios::test_2342);
    REGISTER_TEST(GUITest_regression_scenarios::test_2343);
    REGISTER_TEST(GUITest_regression_scenarios::test_2344);
    REGISTER_TEST(GUITest_regression_scenarios::test_2351);
    REGISTER_TEST(GUITest_regression_scenarios::test_2352);
    REGISTER_TEST(GUITest_regression_scenarios::test_2360);
    REGISTER_TEST(GUITest_regression_scenarios::test_2364);
    REGISTER_TEST(GUITest_regression_scenarios::test_2373);
    REGISTER_TEST(GUITest_regression_scenarios::test_2374);
    REGISTER_TEST(GUITest_regression_scenarios::test_2375);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_2470_1);
    //REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2475);
    REGISTER_TEST(GUITest_regression_scenarios::test_2482);
    REGISTER_TEST(GUITest_regression_scenarios::test_2487);
    REGISTER_TEST(GUITest_regression_scenarios::test_2496);
    REGISTER_TEST(GUITest_regression_scenarios::test_2498);
    REGISTER_TEST(GUITest_regression_scenarios::test_2506);
    REGISTER_TEST(GUITest_regression_scenarios::test_2506_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2513);
    REGISTER_TEST(GUITest_regression_scenarios::test_2519);
    REGISTER_TEST(GUITest_regression_scenarios::test_2538);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_2540, "UGENE-4948");
    REGISTER_TEST(GUITest_regression_scenarios::test_2542);
    REGISTER_TEST(GUITest_regression_scenarios::test_2543);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_2544, "UGENE-4948");
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
    REGISTER_TEST(GUITest_regression_scenarios::test_2581_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_2583);
    REGISTER_TEST(GUITest_regression_scenarios::test_2605);
    REGISTER_TEST(GUITest_regression_scenarios::test_2612);
    REGISTER_TEST(GUITest_regression_scenarios::test_2619);
    REGISTER_TEST(GUITest_regression_scenarios::test_2622);
    REGISTER_TEST(GUITest_regression_scenarios::test_2622_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_2632);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2638);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2640);
    REGISTER_TEST_IGNORED_MAC(GUITest_regression_scenarios::test_2651, "this test seems to break mac testing");
    REGISTER_TEST(GUITest_regression_scenarios::test_2656);
    REGISTER_TEST(GUITest_regression_scenarios::test_2662);
    REGISTER_TEST(GUITest_regression_scenarios::test_2667);
    REGISTER_TEST(GUITest_regression_scenarios::test_2683);
    REGISTER_TEST(GUITest_regression_scenarios::test_2690);
    REGISTER_TEST(GUITest_regression_scenarios::test_2701);
    REGISTER_TEST(GUITest_regression_scenarios::test_2709);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2711);
    REGISTER_TEST(GUITest_regression_scenarios::test_2713);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2721);// no cistrome data on windows servers
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
    REGISTER_TEST_WINDOWS(GUITest_regression_scenarios::test_2801, "Specific API should be written for Linux and Mac");
    REGISTER_TEST_WINDOWS(GUITest_regression_scenarios::test_2801_1, "Specific API should be written for Linux and Mac");
    REGISTER_TEST(GUITest_regression_scenarios::test_2808);
    REGISTER_TEST(GUITest_regression_scenarios::test_2809);
    REGISTER_TEST(GUITest_regression_scenarios::test_2811);
    REGISTER_TEST_IGNORED_MAC(GUITest_regression_scenarios::test_2829, "hotkeys on mac");
    REGISTER_TEST(GUITest_regression_scenarios::test_2853);
    REGISTER_TEST(GUITest_regression_scenarios::test_2863);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_2866);
    REGISTER_TEST(GUITest_regression_scenarios::test_2884);
    REGISTER_TEST(GUITest_regression_scenarios::test_2887);
    REGISTER_TEST(GUITest_regression_scenarios::test_2891);
    REGISTER_TEST(GUITest_regression_scenarios::test_2891_1);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_2894, "proper data needed");
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
    REGISTER_TEST_LABEL(GUITest_regression_scenarios::test_2971, "mem");
    REGISTER_TEST(GUITest_regression_scenarios::test_2972);
    REGISTER_TEST(GUITest_regression_scenarios::test_2975);
    REGISTER_TEST(GUITest_regression_scenarios::test_2981);
    REGISTER_TEST(GUITest_regression_scenarios::test_2987);
    REGISTER_TEST(GUITest_regression_scenarios::test_2991);
    REGISTER_TEST(GUITest_regression_scenarios::test_2998);

    REGISTER_TEST(GUITest_regression_scenarios::test_3006);
    REGISTER_TEST(GUITest_regression_scenarios::test_3014);
    REGISTER_TEST(GUITest_regression_scenarios::test_3017);
    REGISTER_TEST(GUITest_regression_scenarios::test_3031);
    REGISTER_TEST(GUITest_regression_scenarios::test_3034);
    REGISTER_TEST(GUITest_regression_scenarios::test_3035);
    REGISTER_TEST(GUITest_regression_scenarios::test_3052);
    REGISTER_TEST(GUITest_regression_scenarios::test_3052_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3072);
    REGISTER_TEST(GUITest_regression_scenarios::test_3073);
    REGISTER_TEST(GUITest_regression_scenarios::test_3074);
    REGISTER_TEST(GUITest_regression_scenarios::test_3079);
    REGISTER_TEST(GUITest_regression_scenarios::test_3085_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3085_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3086);
    REGISTER_TEST(GUITest_regression_scenarios::test_3092);
    REGISTER_TEST(GUITest_regression_scenarios::test_3101);
    REGISTER_TEST(GUITest_regression_scenarios::test_3102);
    REGISTER_TEST(GUITest_regression_scenarios::test_3103);
    REGISTER_TEST(GUITest_regression_scenarios::test_3112);
    REGISTER_TEST(GUITest_regression_scenarios::test_3124);
    REGISTER_TEST(GUITest_regression_scenarios::test_3125);
    REGISTER_TEST(GUITest_regression_scenarios::test_3126);
    REGISTER_TEST(GUITest_regression_scenarios::test_3128);
    REGISTER_TEST(GUITest_regression_scenarios::test_3128_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3130);
    REGISTER_TEST(GUITest_regression_scenarios::test_3133);
    REGISTER_TEST(GUITest_regression_scenarios::test_3137);
    REGISTER_TEST(GUITest_regression_scenarios::test_3138);
    REGISTER_TEST(GUITest_regression_scenarios::test_3139);
    REGISTER_TEST(GUITest_regression_scenarios::test_3140);
    REGISTER_TEST(GUITest_regression_scenarios::test_3142);
    REGISTER_TEST(GUITest_regression_scenarios::test_3143);
    REGISTER_TEST(GUITest_regression_scenarios::test_3144);
    REGISTER_TEST(GUITest_regression_scenarios::test_3155);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_3156, "drag and drop");
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_3165, "drag and drop");
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
    REGISTER_TEST(GUITest_regression_scenarios::test_3250);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3253_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3255);
    REGISTER_TEST(GUITest_regression_scenarios::test_3263);
    REGISTER_TEST(GUITest_regression_scenarios::test_3266);
    REGISTER_TEST(GUITest_regression_scenarios::test_3270);
    REGISTER_TEST(GUITest_regression_scenarios::test_3274);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_3276, "Obsolete, should be replaced by UGENE-6104");
    REGISTER_TEST(GUITest_regression_scenarios::test_3277);
    REGISTER_TEST(GUITest_regression_scenarios::test_3279);
    REGISTER_TEST(GUITest_regression_scenarios::test_3283);
    REGISTER_TEST(GUITest_regression_scenarios::test_3287);
    REGISTER_TEST_WITH_TIMEOUT(GUITest_regression_scenarios::test_3288, 420000);
    REGISTER_TEST(GUITest_regression_scenarios::test_3305);
    REGISTER_TEST(GUITest_regression_scenarios::test_3306);
    REGISTER_TEST(GUITest_regression_scenarios::test_3307);
    REGISTER_TEST(GUITest_regression_scenarios::test_3308);
    REGISTER_TEST(GUITest_regression_scenarios::test_3312);
    REGISTER_TEST(GUITest_regression_scenarios::test_3313);
    REGISTER_TEST(GUITest_regression_scenarios::test_3318);
    REGISTER_TEST(GUITest_regression_scenarios::test_3319);
    REGISTER_TEST(GUITest_regression_scenarios::test_3321);
    REGISTER_TEST(GUITest_regression_scenarios::test_3328);
    REGISTER_TEST(GUITest_regression_scenarios::test_3332);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_3333, "drag and drop");
    REGISTER_TEST(GUITest_regression_scenarios::test_3335);
    REGISTER_TEST(GUITest_regression_scenarios::test_3342);
    REGISTER_TEST(GUITest_regression_scenarios::test_3344);
    REGISTER_TEST(GUITest_regression_scenarios::test_3346);
    REGISTER_TEST(GUITest_regression_scenarios::test_3348);
    REGISTER_TEST(GUITest_regression_scenarios::test_3357);
    REGISTER_TEST(GUITest_regression_scenarios::test_3373);
    REGISTER_TEST(GUITest_regression_scenarios::test_3379);
    REGISTER_TEST(GUITest_regression_scenarios::test_3384);
    REGISTER_TEST(GUITest_regression_scenarios::test_3396);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_3398_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_3402);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_3414);
    REGISTER_TEST(GUITest_regression_scenarios::test_3428);
    REGISTER_TEST(GUITest_regression_scenarios::test_3430);
    REGISTER_TEST(GUITest_regression_scenarios::test_3437);
    REGISTER_TEST(GUITest_regression_scenarios::test_3439);
    REGISTER_TEST(GUITest_regression_scenarios::test_3441);
    REGISTER_TEST(GUITest_regression_scenarios::test_3443);
    REGISTER_TEST(GUITest_regression_scenarios::test_3450);
    REGISTER_TEST(GUITest_regression_scenarios::test_3451);
    REGISTER_TEST_LINUX(GUITest_regression_scenarios::test_3452, "temporary ignored because of mac failing");
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
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_3545, "big data");
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

    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_3603, "UGENE-3830");
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_3609_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_3610);
    REGISTER_TEST(GUITest_regression_scenarios::test_3612);
    REGISTER_TEST(GUITest_regression_scenarios::test_3613);
    REGISTER_TEST(GUITest_regression_scenarios::test_3619);
    REGISTER_TEST(GUITest_regression_scenarios::test_3622);
    REGISTER_TEST(GUITest_regression_scenarios::test_3623);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_3625, "UGENE-3999");
    REGISTER_TEST(GUITest_regression_scenarios::test_3629);
    REGISTER_TEST(GUITest_regression_scenarios::test_3634);
    REGISTER_TEST(GUITest_regression_scenarios::test_3639);
    REGISTER_TEST(GUITest_regression_scenarios::test_3640);
    REGISTER_TEST(GUITest_regression_scenarios::test_3645);
    REGISTER_TEST(GUITest_regression_scenarios::test_3649);
    REGISTER_TEST(GUITest_regression_scenarios::test_3656);
    REGISTER_TEST(GUITest_regression_scenarios::test_3658);
    REGISTER_TEST(GUITest_regression_scenarios::test_3675);
    REGISTER_TEST(GUITest_regression_scenarios::test_3676);
    REGISTER_TEST_WITH_TIMEOUT(GUITest_regression_scenarios::test_3687_1, 350000);
    REGISTER_TEST_WITH_TIMEOUT(GUITest_regression_scenarios::test_3687_2, 350000);
    REGISTER_TEST_IGNORED_MAC(GUITest_regression_scenarios::test_3690, "hotkeys on mac");
    REGISTER_TEST(GUITest_regression_scenarios::test_3697);

    REGISTER_TEST(GUITest_regression_scenarios::test_3702);
    REGISTER_TEST(GUITest_regression_scenarios::test_3710);
    REGISTER_TEST(GUITest_regression_scenarios::test_3715);
    REGISTER_TEST(GUITest_regression_scenarios::test_3717);
    REGISTER_TEST(GUITest_regression_scenarios::test_3723);
    REGISTER_TEST(GUITest_regression_scenarios::test_3724);
    REGISTER_TEST(GUITest_regression_scenarios::test_3728);
    REGISTER_TEST(GUITest_regression_scenarios::test_3730);
    REGISTER_TEST(GUITest_regression_scenarios::test_3731);
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_3732, "big data");
    REGISTER_TEST(GUITest_regression_scenarios::test_3736);
    REGISTER_TEST(GUITest_regression_scenarios::test_3738);
    REGISTER_TEST(GUITest_regression_scenarios::test_3744);
    REGISTER_TEST(GUITest_regression_scenarios::test_3749);
    REGISTER_TEST(GUITest_regression_scenarios::test_3753);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_3813);
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
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_3950);//too long for windows test server
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
    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_4022, "big data");
    REGISTER_TEST(GUITest_regression_scenarios::test_4026);
    REGISTER_TEST(GUITest_regression_scenarios::test_4030);
    REGISTER_TEST(GUITest_regression_scenarios::test_4033);
    REGISTER_TEST(GUITest_regression_scenarios::test_4034);
    REGISTER_TEST_WITH_TIMEOUT(GUITest_regression_scenarios::test_4035, 300000);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_4087);
    REGISTER_TEST(GUITest_regression_scenarios::test_4091);
    REGISTER_TEST(GUITest_regression_scenarios::test_4093);
    REGISTER_TEST(GUITest_regression_scenarios::test_4095);
    REGISTER_TEST(GUITest_regression_scenarios::test_4096);
    REGISTER_TEST(GUITest_regression_scenarios::test_4097);
    REGISTER_TEST(GUITest_regression_scenarios::test_4099);

    REGISTER_TEST(GUITest_regression_scenarios::test_4100);
    REGISTER_TEST(GUITest_regression_scenarios::test_4104);
    REGISTER_TEST(GUITest_regression_scenarios::test_4106);
    REGISTER_TEST(GUITest_regression_scenarios::test_4110);
    REGISTER_TEST(GUITest_regression_scenarios::test_4111);
    REGISTER_TEST(GUITest_regression_scenarios::test_4113);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_4150);
    REGISTER_TEST(GUITest_regression_scenarios::test_4151);
    REGISTER_TEST(GUITest_regression_scenarios::test_4153);
    REGISTER_TEST(GUITest_regression_scenarios::test_4156);
    REGISTER_TEST(GUITest_regression_scenarios::test_4160);
    REGISTER_TEST_LABEL(GUITest_regression_scenarios::test_4164, "mem");
    REGISTER_TEST(GUITest_regression_scenarios::test_4170);
    REGISTER_TEST(GUITest_regression_scenarios::test_4179);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4188_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4194);

    REGISTER_TEST_IGNORED(GUITest_regression_scenarios::test_4209, "test consumes too much memory");
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

    REGISTER_TEST(GUITest_regression_scenarios::test_4505);
    REGISTER_TEST(GUITest_regression_scenarios::test_4508);
    REGISTER_TEST(GUITest_regression_scenarios::test_4515);
    REGISTER_TEST(GUITest_regression_scenarios::test_4523);
    REGISTER_TEST(GUITest_regression_scenarios::test_4524);
    REGISTER_TEST(GUITest_regression_scenarios::test_4536);
    REGISTER_TEST(GUITest_regression_scenarios::test_4537);
    REGISTER_TEST(GUITest_regression_scenarios::test_4552);
    REGISTER_TEST(GUITest_regression_scenarios::test_4557);
    if (QSysInfo::WordSize == 32) {
        REGISTER_TEST(GUITest_regression_scenarios::test_4563);
    }
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
    REGISTER_TEST(GUITest_regression_scenarios::test_4687);
    REGISTER_TEST(GUITest_regression_scenarios::test_4689_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4689_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4694);
    REGISTER_TEST(GUITest_regression_scenarios::test_4699);

    REGISTER_TEST(GUITest_regression_scenarios::test_4700);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_4702_5);
    REGISTER_TEST(GUITest_regression_scenarios::test_4710);
    REGISTER_TEST(GUITest_regression_scenarios::test_4710_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4714_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4714_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4712);
    REGISTER_TEST(GUITest_regression_scenarios::test_4718);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4719_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4728);
    REGISTER_TEST(GUITest_regression_scenarios::test_4732);
    REGISTER_TEST(GUITest_regression_scenarios::test_4734);
    REGISTER_TEST(GUITest_regression_scenarios::test_4735);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4764_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_4782);
    REGISTER_TEST(GUITest_regression_scenarios::test_4784_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4785_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_4785_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_4795);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_4852);
    REGISTER_TEST(GUITest_regression_scenarios::test_4860);
    REGISTER_TEST(GUITest_regression_scenarios::test_4871);
    //REGISTER_TEST(GUITest_regression_scenarios::test_4881);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_4983);
    REGISTER_TEST(GUITest_regression_scenarios::test_4986);
    REGISTER_TEST(GUITest_regression_scenarios::test_4996);

    REGISTER_TEST(GUITest_regression_scenarios::test_5004);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5012_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5018);
    REGISTER_TEST(GUITest_regression_scenarios::test_5026);

    if (QSysInfo::WordSize == 64) {
        REGISTER_TEST(GUITest_regression_scenarios::test_5027_1);
    }
    REGISTER_TEST(GUITest_regression_scenarios::test_5027_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5029);
    REGISTER_TEST(GUITest_regression_scenarios::test_5039);
    REGISTER_TEST(GUITest_regression_scenarios::test_5052);
    REGISTER_TEST(GUITest_regression_scenarios::test_5069);
    REGISTER_TEST(GUITest_regression_scenarios::test_5082);
    REGISTER_TEST(GUITest_regression_scenarios::test_5090);

    REGISTER_TEST(GUITest_regression_scenarios::test_5110);
    REGISTER_TEST(GUITest_regression_scenarios::test_5128);
    REGISTER_TEST_ONLY_LINUX(GUITest_regression_scenarios::test_5130);
    REGISTER_TEST(GUITest_regression_scenarios::test_5137);
    if (QSysInfo::WordSize == 32) {
        REGISTER_TEST_WITH_TIMEOUT(GUITest_regression_scenarios::test_5138_1, 420000);
        REGISTER_TEST(GUITest_regression_scenarios::test_5138_2);
    }
    REGISTER_TEST(GUITest_regression_scenarios::test_5199);

    REGISTER_TEST(GUITest_regression_scenarios::test_5208);
    REGISTER_TEST(GUITest_regression_scenarios::test_5211);
    REGISTER_TEST(GUITest_regression_scenarios::test_5216);
    REGISTER_TEST(GUITest_regression_scenarios::test_5220);
    REGISTER_TEST(GUITest_regression_scenarios::test_5227);
    REGISTER_TEST(GUITest_regression_scenarios::test_5231);
    REGISTER_TEST(GUITest_regression_scenarios::test_5246);
    REGISTER_TEST(GUITest_regression_scenarios::test_5249);
    REGISTER_TEST(GUITest_regression_scenarios::test_5252);
    REGISTER_TEST(GUITest_regression_scenarios::test_5268);
    REGISTER_TEST(GUITest_regression_scenarios::test_5278);
    REGISTER_TEST(GUITest_regression_scenarios::test_5295);

    REGISTER_TEST(GUITest_regression_scenarios::test_5314);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_5659);
    REGISTER_TEST(GUITest_regression_scenarios::test_5663);
    REGISTER_TEST(GUITest_regression_scenarios::test_5665);
    REGISTER_TEST(GUITest_regression_scenarios::test_5681);
    REGISTER_TEST(GUITest_regression_scenarios::test_5696);

    REGISTER_TEST(GUITest_regression_scenarios::test_5714_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5714_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_5714_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_5716);
    REGISTER_TEST(GUITest_regression_scenarios::test_5718);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_5762);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_5847);
    REGISTER_TEST(GUITest_regression_scenarios::test_5849);
    REGISTER_TEST(GUITest_regression_scenarios::test_5851);
    REGISTER_TEST(GUITest_regression_scenarios::test_5853);
    REGISTER_TEST(GUITest_regression_scenarios::test_5854);
    REGISTER_TEST(GUITest_regression_scenarios::test_5855);
    REGISTER_TEST(GUITest_regression_scenarios::test_5872);
    REGISTER_TEST(GUITest_regression_scenarios::test_5898);
    REGISTER_TEST(GUITest_regression_scenarios::test_5903);
    REGISTER_TEST(GUITest_regression_scenarios::test_5905);
    REGISTER_TEST(GUITest_regression_scenarios::test_5947);
    REGISTER_TEST(GUITest_regression_scenarios::test_5948);
    REGISTER_TEST(GUITest_regression_scenarios::test_5950);
    REGISTER_TEST(GUITest_regression_scenarios::test_5970);
    REGISTER_TEST(GUITest_regression_scenarios::test_5972_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_5972_2);

    REGISTER_TEST(GUITest_regression_scenarios::test_6031);
    REGISTER_TEST(GUITest_regression_scenarios::test_6033);
    REGISTER_TEST(GUITest_regression_scenarios::test_6038_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6038_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6038_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6038_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6043);
    REGISTER_TEST(GUITest_regression_scenarios::test_6047);
    REGISTER_TEST(GUITest_regression_scenarios::test_6058_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6058_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6062);
    REGISTER_TEST(GUITest_regression_scenarios::test_6066);
    REGISTER_TEST(GUITest_regression_scenarios::test_6071);
    REGISTER_TEST(GUITest_regression_scenarios::test_6078);
    REGISTER_TEST(GUITest_regression_scenarios::test_6083);
    REGISTER_TEST(GUITest_regression_scenarios::test_6087);

    REGISTER_TEST(GUITest_regression_scenarios::test_6102);
    REGISTER_TEST(GUITest_regression_scenarios::test_6118);
    REGISTER_TEST(GUITest_regression_scenarios::test_6135);
    REGISTER_TEST(GUITest_regression_scenarios::test_6136);
    REGISTER_TEST(GUITest_regression_scenarios::test_6167);

    REGISTER_TEST(GUITest_regression_scenarios::test_6204);
    REGISTER_TEST(GUITest_regression_scenarios::test_6207);
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
    REGISTER_TEST_ONLY_MAC(GUITest_regression_scenarios::test_6238);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_6283);
    REGISTER_TEST(GUITest_regression_scenarios::test_6291);
    REGISTER_TEST(GUITest_regression_scenarios::test_6298);

    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_6301);
    REGISTER_TEST(GUITest_regression_scenarios::test_6309);
    REGISTER_TEST(GUITest_regression_scenarios::test_6314);
    REGISTER_TEST(GUITest_regression_scenarios::test_6334);
    REGISTER_TEST(GUITest_regression_scenarios::test_6350);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_6378);
    REGISTER_TEST(GUITest_regression_scenarios::test_6397);
    REGISTER_TEST(GUITest_regression_scenarios::test_6398);

    REGISTER_TEST(GUITest_regression_scenarios::test_6459);
    REGISTER_TEST(GUITest_regression_scenarios::test_6475_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6475_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_2);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_3);
    REGISTER_TEST(GUITest_regression_scenarios::test_6481_4);
    REGISTER_TEST(GUITest_regression_scenarios::test_6474_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6474_2);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_6564);
    REGISTER_TEST(GUITest_regression_scenarios::test_6569);
    REGISTER_TEST(GUITest_regression_scenarios::test_6580);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_regression_scenarios::test_6581); //old MAFFT version  v7.212 for Windows32
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
    REGISTER_TEST(GUITest_regression_scenarios::test_6659);
    REGISTER_TEST(GUITest_regression_scenarios::test_6667_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6676_1);
    REGISTER_TEST(GUITest_regression_scenarios::test_6676_2);
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
    REGISTER_TEST(GUITest_regression_scenarios::test_6705);
    REGISTER_TEST(GUITest_regression_scenarios::test_6706);
    REGISTER_TEST(GUITest_regression_scenarios::test_6707);
    REGISTER_TEST(GUITest_regression_scenarios::test_6710);
    REGISTER_TEST(GUITest_regression_scenarios::test_6714);
    REGISTER_TEST(GUITest_regression_scenarios::test_6718);

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
    REGISTER_TEST(GUITest_common_scenarios_project::test_0032);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0033);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0034);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0035);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0036);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0037);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0038);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0038_1);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0039);
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0040, "UGENE-5042");
    REGISTER_TEST_NOT_FOR_MAC(GUITest_common_scenarios_project::test_0041);     // There is no "Shift + Insert" hotkey on mac
    REGISTER_TEST(GUITest_common_scenarios_project::test_0042);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0043);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0045);
    REGISTER_TEST(GUITest_common_scenarios_project::test_0046);
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0047, "UGENE-5042");
    REGISTER_TEST(GUITest_common_scenarios_project::test_0048);
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0049, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0050, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0051, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0052, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0053, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0054, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0055, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0056, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0057, "UGENE-5042");
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_project::test_0058, "UGENE-5042");

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
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_project_filtering::test_0013);
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
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0023);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0024);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0025);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0028);
    REGISTER_TEST(GUITest_common_scenarios_sequence_view::test_0029);
    REGISTER_TEST_LABEL(GUITest_common_scenarios_sequence_view::test_0030, "mem");
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
    // Common scenarios/ngs_classification/metaphlan2
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0001);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0002);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0003);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0004);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0005);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0006);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0007);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_external_tool::test_0008);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_workflow_designer_element::test_0001);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_workflow_designer_element::test_0002);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_workflow_designer_element::test_0003);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_mg_metaphlan2_workflow_designer_element::test_0004);

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
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0003_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0003_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0003_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0003_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0004_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0005_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0007_4);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0008_3);

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
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0016_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0016_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0017_2);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0018);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0019);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0019_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0019_2);

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

    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_msa_editor::test_0025);
    REGISTER_TEST_ONLY_WINDOWS(GUITest_common_scenarios_msa_editor::test_0025_1);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0026);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0026_1);

    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_msa_editor::test_0026_2_linux);
    REGISTER_TEST_ONLY_WINDOWS(GUITest_common_scenarios_msa_editor::test_0026_2_windows);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0027);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0027_1);

    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_msa_editor::test_0028_linux);
    REGISTER_TEST_ONLY_WINDOWS(GUITest_common_scenarios_msa_editor::test_0028_windows);

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
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0051);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0052);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_2);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_3);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_4);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0053_5);

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0054);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0054_1);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0055);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0056);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0057);
    //REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0058); not completed yet
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0059);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0060);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0061);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0062);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0063);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0064);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0065);
    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0066);
    //REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_0068); removed as obsolete
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

    REGISTER_TEST(GUITest_common_scenarios_msa_editor::test_fake);

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
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0004_1);
    REGISTER_TEST(GUITest_common_scenarios_annotations_qualifiers::test_0004_2);

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
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_options_panel_MSA::tree_settings_test_0007, "UGENE-5188");
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
    // Common scenarios/Assembling/Assembly browser
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_Assembly_browser::test_0001);
    REGISTER_TEST(GUITest_Assembly_browser::test_0002);
    REGISTER_TEST(GUITest_Assembly_browser::test_0010);
    REGISTER_TEST(GUITest_Assembly_browser::test_0011);
    REGISTER_TEST_IGNORED_WINDOWS(GUITest_Assembly_browser::test_0012, "permission setter issue");
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

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/bowtie2
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0001);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0002);//"Restore when this tool becomes available");
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0003);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0004);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0005);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_Bowtie2::test_0006);

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
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_dna_assembly_conversions::test_0001);
    REGISTER_TEST(GUITest_dna_assembly_conversions::test_0002);
    REGISTER_TEST(GUITest_dna_assembly_conversions::test_0003);
    REGISTER_TEST(GUITest_dna_assembly_conversions::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Assembling/sam
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_SAM::test_0002);
    REGISTER_TEST(GUITest_SAM::test_0003);
    REGISTER_TEST(GUITest_SAM::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Query designer
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST_IGNORED_MAC(GUITest_common_scenarios_querry_designer::test_0001, "breaks testing on mac");
    REGISTER_TEST_IGNORED_MAC(GUITest_common_scenarios_querry_designer::test_0001_1, "breaks testing on mac");
    REGISTER_TEST_IGNORED_MAC(GUITest_common_scenarios_querry_designer::test_0001_2, "breaks testing on mac");

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_workflow_designer::test_0002_1);
    REGISTER_TEST(GUITest_common_scenarios_workflow_designer::test_0003);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_workflow_designer::test_0005)
    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_workflow_designer::test_0006);
    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_workflow_designer::test_0006_1);
    REGISTER_TEST_ONLY_LINUX(GUITest_common_scenarios_workflow_designer::test_0007);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_workflow_designer::test_0009)
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_workflow_designer::test_0010)
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
    REGISTER_TEST_NOT_FOR_MAC(GUITest_common_scenarios_workflow_parameters_validation::test_0002);//, "qt dialog can't be shown");
    REGISTER_TEST(GUITest_common_scenarios_workflow_parameters_validation::test_0003);
    REGISTER_TEST_NOT_FOR_WINDOWS(GUITest_common_scenarios_workflow_parameters_validation::test_0005);//, "Test should run not under admin user on WIN");
    REGISTER_TEST(GUITest_common_scenarios_workflow_parameters_validation::test_0006);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Workflow designer/Dashboard misc
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::misc_test_0005);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tree_nodes_creation_test_0004);

    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0011);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0012);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0013);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::tool_launch_nodes_test_0014);
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
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_workflow_dashboard::output_dir_scanning_test_0008);


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
    // Common scenarios/Workflow designer/Scripting
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_workflow_scripting::test_0004);

    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/NIAID_pipelines
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_NIAID_pipelines::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_NIAID_pipelines::test_0002);
    /////////////////////////////////////////////////////////////////////////
    // Common scenarios/Tree viewer
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_tree_viewer::test_0001);
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
    // Common scenarios/shared_database
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0013);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::cm_test_0014);

    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::proj_test_0009);

    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0002);
    REGISTER_TEST_IGNORED(GUITest_common_scenarios_shared_database::import_test_0003,"drag&drop");
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0011);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0012);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0013);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0014);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0015);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0016);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0017);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0018);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0019);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::import_test_0020);

    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::view_test_0006);

    REGISTER_TEST(GUITest_common_scenarios_shared_database::del_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::del_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::del_test_0003);

    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_shared_database::export_test_0009);

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

    /////////////////////////////////////////////////////////////////////////
    // common_scenarios/workflow_designer/shared_db
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0005);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0006);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_neg_test_0007);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0008);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0009);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0010);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_test_0011);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::read_gui_neg_test_0012);

    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::write_gui_test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::write_gui_test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::write_gui_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::write_gui_test_0003);

    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::open_uwl_gui_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::open_uwl_gui_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::open_uwl_gui_test_0003);

    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::save_uwl_gui_test_0001);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::save_uwl_gui_test_0002);

    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0001_1);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0001_2);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0002);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0003);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0004);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0005_1);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0005_2);
    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::run_workflow_gui_test_0006);

    REGISTER_TEST(GUITest_common_scenarios_shared_db_wd::test_3726);

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
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0008);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0009);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0010);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0011);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0012);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0013);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0014);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0015);
    REGISTER_TEST(GUITest_common_scenarios_primer_library::test_0016);

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
    // common_scenarios/start_page
    /////////////////////////////////////////////////////////////////////////
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0001);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0002);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0003);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0005);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0006);
    REGISTER_TEST(GUITest_common_scenarios_start_page::test_0008);
}

void GUITestBasePlugin::registerAdditionalActions(UGUITestBase *guiTestBase) {
    if (guiTestBase) {
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0000, UGUITestBase::PreAdditional);
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0001, UGUITestBase::PreAdditional);
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0002, UGUITestBase::PreAdditional);
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0003, UGUITestBase::PreAdditional);
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0004, UGUITestBase::PreAdditional);
        guiTestBase->registerTest(new GUITest_preliminary_actions::pre_action_0005, UGUITestBase::PreAdditional);

        guiTestBase->registerTest(new GUITest_posterior_checks::post_check_0000, UGUITestBase::PostAdditionalChecks);
        guiTestBase->registerTest(new GUITest_posterior_checks::post_check_0001, UGUITestBase::PostAdditionalChecks);

        guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0000, UGUITestBase::PostAdditionalActions);
        guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0001, UGUITestBase::PostAdditionalActions);
        guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0002, UGUITestBase::PostAdditionalActions);
        guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0003, UGUITestBase::PostAdditionalActions);
        guiTestBase->registerTest(new GUITest_posterior_actions::post_action_0004, UGUITestBase::PostAdditionalActions);
    }
}

} //namespace
