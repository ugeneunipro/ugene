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

#include "GTTestsWorkflowScripting.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>
#include <src/GTUtilsSequenceView.h>

#include <U2Core/AppContext.h>

// TODO: cross-plugin dependency!
#include "../../workflow_designer/src/WorkflowViewItems.h"
#include "GTGlobals.h"
#include "GTUtilsMdi.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/GTAction.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/workflow_designer/CreateElementWithScriptDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WorkflowMetadialogFiller.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
namespace GUITest_common_scenarios_workflow_scripting {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    1. Open WD. Press toolbar button "Create script object".
    //    Expected state: Create element with script dialog appears.

    //    2. Fill the next field in dialog:
    //        {Name} 123

    //    3. Click OK button.

    //    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new CreateElementWithScriptDialogFiller("wd_scripting_test_0001"));
    GTMenu::clickMainMenuItem({"Actions", "Create element with script..."}, GTGlobals::UseMouse);
    //    4. Select created worker. Press toolbar button "Edit script text".
    //    Expected state: Script editor dialog appears.

    //    5. Paste "#$%not a script asdasd321 123" at the script text area. Click "Check syntax" button
    //    Expected state: messagebox "Script syntax check failed!" appears.

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("wd_scripting_test_0001"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new ScriptEditorDialogSyntaxChecker("#$%not a script asdasd321 123", "Script syntax check failed!"));
    GTMenu::clickMainMenuItem({"Actions", "Edit script of the element..."}, GTGlobals::UseMouse);
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // 	WD Scripts check syntax doesn't work (0001728)
    //
    // 	1. Open WD. Do toolbar menu "Scripting mode->Show scripting options". Place write FASTA worker on field.

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Write FASTA");

    GTUtilsDialog::waitForDialog(new PopupChooser({"Show scripting options"}));
    GTWidget::click(GTAction::button(GTAction::findActionByText("Scripting mode")));
    GTUtilsDialog::checkNoActiveWaiters();

    //  2. Select this worker, select menu item "user script" from "output file" parameter.
    //  Expected state: Script editor dialog appears.
    //
    //  3. Paste "#$%not a script asdasd321 123" at the script text area. Click "Check syntax" button
    //  Expected state: messagebox "Script syntax check failed!" appears.
    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write FASTA"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new ScriptEditorDialogFiller("", "#$%not a script asdasd321 123", true, "Script syntax check failed! Line: 1, error: Expected `end of file'"));
    GTUtilsWorkflowDesigner::setParameterScripting("output file", "user script");
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");

    GTUtilsWorkflowDesigner::addAlgorithm("Write FASTA");
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::getWorker("Write FASTA");

    GTUtilsWorkflowDesigner::connect(reader, writer);

    GTUtilsDialog::waitForDialog(new PopupChooser({"Show scripting options"}));
    GTWidget::click(GTAction::button(GTAction::findActionByText("Scripting mode")));
    GTUtilsDialog::checkNoActiveWaiters();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Write FASTA"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new ScriptEditorDialogFiller("", "url_out = url + \".result.fa\";"));
    GTUtilsWorkflowDesigner::setParameterScripting("Output file", "user script", true);
    GTUtilsDialog::checkNoActiveWaiters();

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("Read Sequence"));
    GTMouseDriver::click();

    GTUtilsWorkflowDesigner::setDatasetInputFile(dataDir + "samples/FASTA/human_T1.fa");
    GTWidget::click(GTAction::button("Run workflow"));

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa.result.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    GTUtilsDialog::waitForDialog(new CreateElementWithScriptDialogFiller("workflow_scripting_test_0004"));
    GTMenu::clickMainMenuItem({"Actions", "Create element with script..."}, GTGlobals::UseMouse);

    GTMouseDriver::moveTo(GTUtilsWorkflowDesigner::getItemCenter("workflow_scripting_test_0004"));
    GTMouseDriver::click();

    GTUtilsDialog::waitForDialog(new ScriptEditorDialogFiller("", "if(size(in_seq) >= 10000) {out_seq = in_seq;}"));
    GTMenu::clickMainMenuItem({"Actions", "Edit script of the element..."}, GTGlobals::UseMouse);

    WorkflowProcessItem* script = GTUtilsWorkflowDesigner::getWorker("workflow_scripting_test_0004");
    QString text = script->getProcess()->getScript()->getScriptText();

    GTUtilsWorkflowDesigner::addAlgorithm("Read Sequence", true);
    WorkflowProcessItem* reader = GTUtilsWorkflowDesigner::getWorker("Read Sequence");
    GTUtilsWorkflowDesigner::connect(reader, script);

    GTUtilsWorkflowDesigner::addAlgorithm("Write Sequence", true);
    WorkflowProcessItem* writer = GTUtilsWorkflowDesigner::getWorker("Write Sequence");
    GTUtilsWorkflowDesigner::connect(script, writer);

    QString workflowPath = testDir + "_common_data/scenarios/sandbox/workflow_scripting_test_0004.uwl";
    GTUtilsDialog::waitForDialog(new WorkflowMetaDialogFiller(workflowPath, "workflow_scripting_test_0004"));
    GTWidget::click(GTAction::button("Save workflow"));
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(workflowPath));
    GTWidget::click(GTAction::button("Load workflow"));
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsTaskTreeView::waitTaskFinished();

    WorkflowProcessItem* newScript = GTUtilsWorkflowDesigner::getWorker("workflow_scripting_test_0004");
    QString newText = newScript->getProcess()->getScript()->getScriptText();
    CHECK_SET_ERR(text == newText, "Different script text");
}

}  // namespace GUITest_common_scenarios_workflow_scripting
}  // namespace U2
