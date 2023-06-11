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

#include "GTTestsWorkflowSamples.h"

#include <QDir>

#include "GTUtilsDashboard.h"
#include "GTUtilsLog.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "base_dialogs/GTFileDialog.h"
#include "primitives/GTMenu.h"
#include "primitives/GTWidget.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_workflow_samples {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // 1. Click Tools -> NGS data analysis -> Filter short scaffolds...
    // 2. Add directory "data\samples\Genbank\"
    // 3. Click "Next", "Next" and "Run"
    // 4. Wait for workflow finished
    // Expected state: No errors in the log, no notifications in the dashboard
    // 5. Open output file
    // Expected state: There are two sequences in the project tree view: "NC_004718" and "NC_014267"

    class FilterShortScaffoldsWizard : public CustomScenario {
    public:
        void run() override {
            QWidget* const wizard = GTWidget::getActiveModalWidget();

            const QString dirPath = QDir(dataDir + "samples/Genbank").absolutePath();
            const auto chooseDirDialog = new GTFileDialogUtils(dirPath, "", HI::GTFileDialogUtils::Choose);
            GTUtilsDialog::waitForDialog(chooseDirDialog);
            GTWidget::click(GTWidget::findWidget("addDirButton", wizard));

            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);
            GTUtilsWizard::clickButton(GTUtilsWizard::Run);
        }
    };

    const GTLogTracer lt;
    GTUtilsWorkflowDesigner::openWorkflowDesigner();
    GTUtilsDialog::waitForDialog(new WizardFiller("Filter short sequences", new FilterShortScaffoldsWizard()));
    GTMenu::clickMainMenuItem({"Tools", "NGS data analysis", "Filter short scaffolds..."});

    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
    CHECK_SET_ERR(!GTUtilsDashboard::hasNotifications(),
                  "Notifications in dashboard: " /*+ GTUtilsDashboard::getJoinedNotificationsString()*/);

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsDashboard::clickOutputFile("Dataset 1.fa");
    GTUtilsProjectTreeView::getItemCenter("NC_004718");
    GTUtilsProjectTreeView::getItemCenter("NC_014267");
    GTUtilsTaskTreeView::waitTaskFinished();
}

}  // namespace GUITest_common_scenarios_workflow_samples

}  // namespace U2
