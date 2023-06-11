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
#include "GTTestsDpView.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTWidget.h>

#include <QCheckBox>

#include <U2Gui/ToolsMenu.h>

#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "src/GTUtilsEscClicker.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_Common_scenarios_dp_view {

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // 1. Use menu {Tools->Build Dot plot}.
    // Expected state: dialog "Build dotplot from sequences" has appeared.
    // 2. Fill the next fields in dialog:
    //     {File with first sequence} _common_data/scenarios/dp_view/dp1.fa
    //     {File with second sequence} _common_data/scenarios/dp_view/dp2.fa
    // 3. Click Next button
    // Expected state: dialog "Dotplot" has appeared.
    // 4. Fill the next fields in dialog:
    //     {Minimum repeat length} 8bp
    //     {repeats identity} 80%
    // 5. Click OK button
    GTUtilsDialog::add(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", testDir + "_common_data/scenarios/dp_view/dp2.fa"));
    GTUtilsDialog::add(new DotPlotFiller(8, 80, false, false));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    // Expected state: Dot plot view has appeared. There is 1 line at view.
    // 6. Use context menu on dot plot view {Dotplot->Remove}
    // Expected state: save "Dotplot" has appeared.
    // 7. Click No button
    GTUtilsDialog::add(new PopupChooser({"Dotplot", "Remove"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::showContextMenu(GTWidget::findWidget("dotplot widget"));
    GTUtilsDialog::checkNoActiveWaiters();

    // Expected state: Dot plot view has closed.
    auto w = GTWidget::findWidget("dotplot widget", nullptr, {false});
    CHECK_SET_ERR(w == nullptr, "Dotplot not deleted");
}
GUI_TEST_CLASS_DEFINITION(test_0011_1) {
    // DIFFERENCE: ONE SEQUENCE USED
    GTUtilsDialog::add(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", "", false, true));
    GTUtilsDialog::add(new DotPlotFiller(8, 80, false, false));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::add(new PopupChooser({"Dotplot", "Remove"}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::showContextMenu(GTWidget::findWidget("dotplot widget"));
    GTUtilsDialog::checkNoActiveWaiters();

    auto w = GTWidget::findWidget("dotplot widget", nullptr, {false});
    CHECK_SET_ERR(w == nullptr, "Dotplot not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_0011_2) {  // commit DotPlotWidget.cpp exitButton
    // DIFFERENCE: EXIT BUTTON IS USED
    GTUtilsDialog::waitForDialog(new DotPlotFiller(8, 80, false, false));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTWidget::click(GTWidget::findWidget("exitButton"));
    GTUtilsDialog::checkNoActiveWaiters();

    auto w = GTWidget::findWidget("dotplot widget", nullptr, {false});
    CHECK_SET_ERR(w == nullptr, "Dotplot not deleted");
}
GUI_TEST_CLASS_DEFINITION(test_0011_3) {
    // DIFFERENCE: EXIT BUTTON IS USED
    GTFileDialog::openFile(testDir + "_common_data/scenarios/dp_view/", "dp1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::waitForDialog(new DotPlotFiller(8, 80, false, false));

    GTWidget::click(GTWidget::findWidget("build_dotplot_action_widget"));
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTWidget::click(GTWidget::findWidget("exitButton"));
    GTUtilsDialog::checkNoActiveWaiters();

    auto w = GTWidget::findWidget("dotplot widget", nullptr, {false});
    CHECK_SET_ERR(w == nullptr, "Dotplot not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    GTUtilsDialog::waitForDialog(new DotPlotFiller(100));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/NC_014267.gb", "secondparametrTest", true, true, false, 10));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // 1. Use menu {Tools->Build Dot plot}.
    // Expected state: dialog "Build dotplot from sequences" has appeared.

    // 2. Fill the following fields in the dialog:
    //     {File with first sequence} _common_data/scenarios/dp_view/dp1.fa
    //     {Compare sequence against itself} set checked
    // 3. Click Next button
    // Expected state: dialog "Dotplot" has appeared.

    // 4. Fill the following fields in the dialog:
    //     {Minimum repeat length} 4bp

    // 5. Click OK button
    GTUtilsDialog::waitForDialog(new DotPlotFiller(4));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsProjectTreeView::openView();

    // 6. Call dotplot context menu
    // 7. Alt-Tab or activate another view

    // 8. Return to dotplot view and call context menu again

    // 9. Repeat operation 3-4 times
    // Expected state: menu repaints correctly, UGENE not crashed
    for (int i = 0; i < 4; i++) {
        GTUtilsDialog::waitForDialog(new GTUtilsEscClicker("dotplot context menu"));
        GTWidget::click(GTWidget::findWidget(GTUtilsProjectTreeView::widgetName));
        auto dpWidget = GTWidget::findWidget("dotplot widget");
        GTMenu::showContextMenu(dpWidget);
        GTUtilsDialog::checkNoActiveWaiters();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0014_1) {
    // DIFFERENCE: ANNOTATION TREE WIDGET IS USED
    GTUtilsDialog::waitForDialog(new DotPlotFiller(4));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsProjectTreeView::openView();

    for (int i = 0; i < 4; i++) {
        GTUtilsDialog::waitForDialog(new GTUtilsEscClicker("dotplot context menu"));
        GTWidget::click(GTUtilsAnnotationsTreeView::getTreeWidget());
        auto dpWidget = GTWidget::findWidget("dotplot widget");
        GTMenu::showContextMenu(dpWidget);
        GTUtilsDialog::checkNoActiveWaiters();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0014_2) {
    // DIFFERENCE: ANNOTATION TREE WIDGET IS USED
    GTUtilsDialog::waitForDialog(new DotPlotFiller(4));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(testDir + "_common_data/scenarios/dp_view/dp1.fa", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsProjectTreeView::openView();

    for (int i = 0; i < 4; i++) {
        GTUtilsDialog::waitForDialog(new GTUtilsEscClicker("dotplot context menu", true));
        GTWidget::click(GTUtilsAnnotationsTreeView::getTreeWidget());
        auto dpWidget = GTWidget::findWidget("dotplot widget");
        GTMenu::showContextMenu(dpWidget);
        GTUtilsDialog::checkNoActiveWaiters();
    }
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    // 1. Use menu {Tools->Build Dot plot}.
    // Expected state: dialog "Build dotplot from sequences" has appeared.
    // 2. Fill the following fields in the dialog:
    //     {File with first sequence} trunk\data\samples\PDB\1CF7.PDB
    //     {Compare sequence against itself} checked
    // 3. Click Next button
    // Expected state: dialog "Dotplot" has appeared.
    // 4. Press the "1k" button
    // Expected state: minimum repeat length changed to 2bp
    // 5. Click OK button
    // Expected state: Dotplot view has appeared.
    GTUtilsDialog::waitForDialog(new DotPlotFiller(100, 0, false, true));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(dataDir + "samples/PDB/1CF7.PDB", "", false, true));

    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsProjectTreeView::openView();

    // 6. Click on the Dotplot view
    GTWidget::click(GTWidget::findWidget("dotplot widget"));

    // Expected state: Dotplot view has been selected, UGENE didn't crash
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    //     Export image dialog check
    //    1. Build DP, it should not be empty (also do not click on it)
    //    Expected state: nothing is selected on DP
    //    2. Context menu: { Dotplot --> Save/Load --> Save as image}
    //    Expected state: both checkboxes are disabled
    //    3. Select an area
    //    4. Repeat step 2
    //    Expected state: "Include area selection" is enabled, the other one is disabled
    //    5. Click on DP
    //    Expected state: the nearest repeat is selected
    //    6. Repeat step 2
    //    Expected state: "Include repeat selection" is enabled, the other one is disabled
    //    7. Select an area again
    //    Expected state: there is an area and the repeat selected on the dotplot
    //    8. Repeat step 2
    //    Expected state: both checkboxes are enabled

    class DotPlotExportImageFiller : public Filler {
    public:
        DotPlotExportImageFiller(int scenario)
            : Filler("ImageExportForm"),
              scenario(scenario) {
        }
        virtual void run() {
            CHECK_SET_ERR(1 <= scenario && scenario <= 4, "Wrong scenario number");
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto includeAreaCheckbox = GTWidget::findCheckBox("include_area_selection", dialog);

            auto includeRepeatCheckbox = GTWidget::findCheckBox("include_repeat_selection", dialog);

            switch (scenario) {
                case 1:
                    CHECK_SET_ERR(!includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is enabled!");
                    CHECK_SET_ERR(!includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is enabled!");
                    break;
                case 2:
                    CHECK_SET_ERR(includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is disabled!");
                    CHECK_SET_ERR(!includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is enabled!");
                    break;
                case 3:
                    CHECK_SET_ERR(!includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is enabled!");
                    CHECK_SET_ERR(includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is disabled!");
                    break;
                case 4:
                    CHECK_SET_ERR(includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is disabled!");
                    CHECK_SET_ERR(includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is disabled!");
                    break;
            }
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }

        static void runScenario(int scenario) {
            GTUtilsDialog::waitForDialog(new DotPlotExportImageFiller(scenario));
            GTUtilsDialog::waitForDialog(new PopupChooser({"Dotplot", "Save/Load", "Save as image"}));
            GTWidget::click(GTWidget::findWidget("dotplot widget"), Qt::RightButton);
            GTUtilsDialog::checkNoActiveWaiters();
        }

    private:
        // 1, 2, 3 or 4
        int scenario;
    };

    GTUtilsDialog::waitForDialog(new DotPlotFiller(50, 50));
    GTUtilsDialog::waitForDialog(new BuildDotPlotFiller(dataDir + "/samples/Genbank/murine.gb", testDir + "_common_data/genbank/pBR322.gb"));
    GTMenu::clickMainMenuItem({"Tools", "Build dotplot..."});
    GTUtilsDialog::checkNoActiveWaiters();

    DotPlotExportImageFiller::runScenario(1);

    GTUtilsSequenceView::selectSequenceRegion(1500, 2500);
    DotPlotExportImageFiller::runScenario(2);

    GTWidget::click(GTWidget::findWidget("dotplot widget"));
    DotPlotExportImageFiller::runScenario(3);

    GTUtilsSequenceView::selectSequenceRegion(1000, 2000);
    DotPlotExportImageFiller::runScenario(4);
}

}  // namespace GUITest_Common_scenarios_dp_view
}  // namespace U2
