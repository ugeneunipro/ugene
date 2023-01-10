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

#include "GTTestsAssemblyExtractConsensus.h"

#include <QFileInfo>

#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "base_dialogs/DefaultDialogFiller.h"
#include "base_dialogs/GTFileDialog.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "drivers/GTKeyboardDriver.h"
#include "primitives/GTComboBox.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTMenu.h"
#include "primitives/GTToolbar.h"
#include "primitives/GTWidget.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_assembly_extract_consensus {

// Scenario for extract consensus wizard filling
class ExtractConsensusWizardScenario : public CustomScenario {
    QStringList inputPaths;
    QString outFile;

public:
    ExtractConsensusWizardScenario(const QStringList& assembliesPath = QStringList(),
                                   const QString& outputFileName = "consensus.fa")
        : inputPaths(), outFile(outputFileName) {
        inputPaths.reserve(assembliesPath.size());
        for (const QString& path : qAsConst(assembliesPath)) {
            inputPaths << QFileInfo(path).absoluteFilePath();
        }
    }

    void run(HI::GUITestOpStatus& os) override {
        QWidget* dialog = GTWidget::getActiveModalWidget(os);
        GTLineEdit::setText(os, "Assembly widget", inputPaths.join(';'), dialog);
        GTLineEdit::setText(os, "Output file widget", outFile, dialog);

        GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
    }
};

GUI_TEST_CLASS_DEFINITION(test_0001_single_input) {
    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "samples/Assembly/chrM.sorted.bam" as an input
    //  3. Click "Run"
    const auto bamScenario =
        new ExtractConsensusWizardScenario({dataDir + "samples/Assembly/chrM.sorted.bam"});
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", bamScenario));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  4. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(os, 120000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString(os));

    //  5. Return to workflow and call the Extract consensus wizard
    GTUtilsWorkflowDesigner::returnToWorkflow(os);
    //  6. Set "_common_data/ugenedb/scerevisiae.bam.ugenedb" as an input and "" as an output
    //  7. Click "Run"
    const auto ugenedbScenario =
        new ExtractConsensusWizardScenario({testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb"}, "");
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", ugenedbScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Show wizard");

    //  8. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(os, 60000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString(os));
}

GUI_TEST_CLASS_DEFINITION(test_0002_multiple_input) {
    // Scenario for extract consensus wizard filling with clicking of "Add" button
    class ExtractConsensusWizardWithAddScenario : public CustomScenario {
        QStringList inputPaths;

    public:
        ExtractConsensusWizardWithAddScenario(const QStringList& assembliesPath = QStringList())
            : inputPaths() {
            inputPaths.reserve(assembliesPath.size());
            for (const QString& path : qAsConst(assembliesPath)) {
                inputPaths << QFileInfo(path).absoluteFilePath();
            }
        }

        void run(HI::GUITestOpStatus& os) override {
            QWidget* const dialog = GTWidget::getActiveModalWidget(os);

            // Dialog filling
            GTLineEdit::setText(os, GTWidget::findLineEdit(os, "Assembly widget", dialog), inputPaths.join(';'));

            GTWidget::click(os, GTWidget::findButtonByText(os, "Add", dialog));
            GTUtilsDialog::waitForDialog(os,
                                         new GTFileDialogUtils(os, testDir + "_common_data/bam/small.bam.sorted.bam"));

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Run);
        }
    };

    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "_common_data/ugenedb/scerevisiae.bam.ugenedb" and "samples/Assembly/chrM.sorted.bam" as an input
    //  3. Add "_common_data/bam/small.bam.sorted.bam" to assemblies
    //  4. Click "Run"
    const auto multiInputScenario = new ExtractConsensusWizardScenario(QStringList()
                                                                       << testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb"
                                                                       << dataDir + "samples/Assembly/chrM.sorted.bam");

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", multiInputScenario));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  5. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(os, 180000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(os), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString(os));
}

GUI_TEST_CLASS_DEFINITION(test_0003_wrong_input) {
    GTLogTracer lt;
    QString dashboardErrMsg = "Unsupported document format: ";

    auto hasDashboardNotification = [&](const QString& errMsg) {
        auto notificationsWidget = GTWidget::findWidget(os, "NotificationsDashboardWidget", GTUtilsDashboard::getDashboard(os));
        return !GTWidget::findLabelByText(os, errMsg, notificationsWidget).isEmpty();
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "samples/Assembly/chrM.sorted.bam" and "samples/Assembly/chrM.fa" as an input
    //  3. Click "Run"
    auto wrongInputScenario = new ExtractConsensusWizardScenario({dataDir + "samples/Assembly/chrM.sorted.bam", dataDir + "samples/Assembly/chrM.fa"});

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", wrongInputScenario));
    GTMenu::clickMainMenuItem(os, {"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  4. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(os, 120000);
    //  Expected state: There should be an error about unsupported format in the log
    GTUtilsLog::checkContainsError(os, lt, dashboardErrMsg);
    //  There should be a notification about this error in the dashboard
    CHECK_SET_ERR(hasDashboardNotification(dashboardErrMsg),
                  "Expected dashboard notification \"" + dashboardErrMsg + "\"");

    //  5. Return to workflow and call the Extract consensus wizard
    GTUtilsWorkflowDesigner::returnToWorkflow(os);
    //  6. Clear input assemblies
    //  7. Click "Run"
    auto emptyInputScenario = new ExtractConsensusWizardScenario();
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "Extract Consensus Wizard", emptyInputScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Show wizard");

    //  8. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(os, 60000);
    //  Expected state: There should be dialog "Workflow cannot be executed"
    CHECK_SET_ERR(GTWidget::getActiveModalWidget(os), "activeModalWidget is nullptr");

    //  9. Click "Ok" in this dialog
    GTUtilsDialog::waitForDialog(os, new HI::MessageBoxDialogFiller(os, QMessageBox::Ok));
    //  Expected state: There should also be an error about missing required input parameter in the workflow
    GTUtilsWorkflowDesigner::checkErrorList(os,
                                            "Read Assembly: Required parameter has no input urls specified: Input file(s)");
}

}  // namespace GUITest_assembly_extract_consensus

}  // namespace U2
