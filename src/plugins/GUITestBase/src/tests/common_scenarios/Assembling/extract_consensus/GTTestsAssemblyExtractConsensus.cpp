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
#include "base_dialogs/GTFileDialog.h"
#include "base_dialogs/MessageBoxFiller.h"
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

    void run() override {
        QWidget* dialog = GTWidget::getActiveModalWidget();
        GTLineEdit::setText("Assembly widget", inputPaths.join(';'), dialog);
        GTLineEdit::setText("Output file widget", outFile, dialog);

        GTUtilsWizard::clickButton(GTUtilsWizard::Run);
    }
};

GUI_TEST_CLASS_DEFINITION(test_0001_single_input) {
    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "samples/Assembly/chrM.sorted.bam" as an input
    //  3. Click "Run"
    const auto bamScenario =
        new ExtractConsensusWizardScenario({dataDir + "samples/Assembly/chrM.sorted.bam"});
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", bamScenario));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  4. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(120000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString());

    //  5. Return to workflow and call the Extract consensus wizard
    GTUtilsWorkflowDesigner::returnToWorkflow();
    //  6. Set "_common_data/ugenedb/scerevisiae.bam.ugenedb" as an input and "" as an output
    //  7. Click "Run"
    const auto ugenedbScenario =
        new ExtractConsensusWizardScenario({testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb"}, "");
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", ugenedbScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Show wizard");

    //  8. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(60000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString());
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

        void run() override {
            QWidget* const dialog = GTWidget::getActiveModalWidget();

            // Dialog filling
            GTLineEdit::setText(GTWidget::findLineEdit("Assembly widget", dialog), inputPaths.join(';'));

            GTWidget::click(GTWidget::findButtonByText("Add", dialog));
            GTUtilsDialog::waitForDialog(
                new GTFileDialogUtils(testDir + "_common_data/bam/small.bam.sorted.bam"));

            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "_common_data/ugenedb/scerevisiae.bam.ugenedb" and "samples/Assembly/chrM.sorted.bam" as an input
    //  3. Add "_common_data/bam/small.bam.sorted.bam" to assemblies
    //  4. Click "Run"
    const auto multiInputScenario = new ExtractConsensusWizardScenario(QStringList()
                                                                       << testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb"
                                                                       << dataDir + "samples/Assembly/chrM.sorted.bam");

    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", multiInputScenario));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  5. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(180000);
    //  Expected state: There should be no errors in the log
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    //  There should be no notifications in the dashboard
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(), "Notifications in dashboard: " + GTUtilsDashboard::getJoinedNotificationsString());
}

GUI_TEST_CLASS_DEFINITION(test_0003_wrong_input) {
    GTLogTracer lt;
    QString dashboardErrMsg = "Unsupported document format: ";

    auto hasDashboardNotification = [&](const QString& errMsg) {
        auto notificationsWidget = GTWidget::findWidget("NotificationsDashboardWidget", GTUtilsDashboard::getDashboard());
        return !GTWidget::findLabelByText(errMsg, notificationsWidget).isEmpty();
    };

    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    //  1. Select "Tools->NGS data analysis->Extract consensus from assemblies..."
    //  2. Set "samples/Assembly/chrM.sorted.bam" and "samples/Assembly/chrM.fa" as an input
    //  3. Click "Run"
    auto wrongInputScenario = new ExtractConsensusWizardScenario({dataDir + "samples/Assembly/chrM.sorted.bam", dataDir + "samples/Assembly/chrM.fa"});

    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", wrongInputScenario));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Extract consensus from assemblies..."});

    //  4. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(120000);
    //  Expected state: There should be an error about unsupported format in the log
    CHECK_SET_ERR(lt.hasError(dashboardErrMsg), "Expected error is not found");
    //  There should be a notification about this error in the dashboard
    CHECK_SET_ERR(hasDashboardNotification(dashboardErrMsg), "Expected dashboard notification \"" + dashboardErrMsg + "\"");

    //  5. Return to workflow and call the Extract consensus wizard
    GTUtilsWorkflowDesigner::returnToWorkflow();
    //  6. Clear input assemblies
    //  7. Click "Run"
    auto emptyInputScenario = new ExtractConsensusWizardScenario();
    GTUtilsDialog::waitForDialog(new WizardFiller("Extract Consensus Wizard", emptyInputScenario));
    GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Show wizard");

    //  8. Wait for workflow finished
    GTUtilsTaskTreeView::waitTaskFinished(60000);
    //  Expected state: There should be dialog "Workflow cannot be executed"
    CHECK_SET_ERR(GTWidget::getActiveModalWidget(), "activeModalWidget is nullptr");

    //  9. Click "Ok" in this dialog
    GTUtilsDialog::waitForDialog(new HI::MessageBoxDialogFiller(QMessageBox::Ok));
    //  Expected state: There should also be an error about missing required input parameter in the workflow
    GTUtilsWorkflowDesigner::checkErrorList(
        "Read Assembly: Required parameter has no input urls specified: Input file(s)");
}

}  // namespace GUITest_assembly_extract_consensus

}  // namespace U2
