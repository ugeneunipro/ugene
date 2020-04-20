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
#include "GTTestsDpView.h"
#include "../../../GTUtilsEscClicker.h"
#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <primitives/GTWidget.h>
#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include <primitives/GTTreeWidget.h>
#include "GTGlobals.h"
#include <drivers/GTMouseDriver.h>
#include "utils/GTUtilsApp.h"
#include "GTUtilsDocument.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsSequenceView.h"
#include "utils/GTUtilsDialog.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include "GTUtilsTaskTreeView.h"

#include <U2Gui/ToolsMenu.h>

#include <QApplication>
#include <QCheckBox>

namespace U2 {

namespace GUITest_Common_scenarios_dp_view {

GUI_TEST_CLASS_DEFINITION(test_0011){
//1. Use menu {Tools->Build Dot plot}.
//Expected state: dialog "Build dotplot from sequences" has appear.
//2. Fill the next fields in dialog:
//    {File with first sequence} _common_data/scenarios/dp_view/dp1.fa
//    {File with second sequence} _common_data/scenarios/dp_view/dp2.fa
//3. Click Next button
//Expected state: dialog "Dotplot" has appear.
//4. Fill the next fields in dialog:
//    {Minimum repeat length} 8bp
//    {repeats identity} 80%
//5. Click OK button
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 8, 80,false,false));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa", testDir + "_common_data/scenarios/dp_view/dp2.fa");
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();
//Expected state: Dot plot view has appear. There is 1 line at view.
//6. Use context menu on dot plot view {Dotplot->Remove}
//Expected state: save "Dotplot" has appear.
//7. Click No button
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Dotplot"<<"Remove"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTMenu::showContextMenu(os,GTWidget::findWidget(os,"dotplot widget"));

//Expected state: Dot plot view has closed.
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *w=GTWidget::findWidget(os,"dotplot widget",NULL, options);

    CHECK_SET_ERR(w==NULL, "Dotplot not deleted");

}
GUI_TEST_CLASS_DEFINITION(test_0011_1){
//DIFFERENCE: ONE SEQUENCE USED
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 8, 80,false,false));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Dotplot"<<"Remove"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTMenu::showContextMenu(os,GTWidget::findWidget(os,"dotplot widget"));
    GTGlobals::sleep(500);

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *w=GTWidget::findWidget(os,"dotplot widget",NULL, options);

    CHECK_SET_ERR(w==NULL, "Dotplot not deleted");
}
GUI_TEST_CLASS_DEFINITION(test_0011_2){//commit DotPlotWidget.cpp exitButton
//DIFFERENCE: EXITBUTTON IS USED
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 8, 80,false,false));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTWidget::click(os, GTWidget::findWidget(os, "exitButton"));
    GTGlobals::sleep(500);

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *w=GTWidget::findWidget(os,"dotplot widget",NULL, options);

    CHECK_SET_ERR(w==NULL, "Dotplot not deleted");
}
GUI_TEST_CLASS_DEFINITION(test_0011_3){
    //DIFFERENCE: EXITBUTTON IS USED
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/dp_view/", "dp1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 8, 80,false,false));

    GTWidget::click(os, GTWidget::findWidget(os, "build_dotplot_action_widget"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTWidget::click(os, GTWidget::findWidget(os, "exitButton"));
    GTGlobals::sleep(500);

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    QWidget *w=GTWidget::findWidget(os,"dotplot widget",NULL, options);

    CHECK_SET_ERR(w==NULL, "Dotplot not deleted");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 100));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/NC_014267.gb", "secondparametrTest",true,true,false,10);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();

}

GUI_TEST_CLASS_DEFINITION(test_0014) {
//1. Use menu {Tools->Build Dot plot}.
//Expected state: dialog "Build dotplot from sequences" has appeared.

//2. Fill the following fields in the dialog:
//    {File with first sequence} _common_data/scenarios/dp_view/dp1.fa
//    {Compare sequence againist itself} set checked
//3. Click Next button
//Expected state: dialog "Dotplot" has appeared.

//4. Fill the following fields in the dialog:
//    {Minimum repeat length} 4bp

//5. Click OK button
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 4));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep(500);
    GTUtilsProjectTreeView::openView(os);

//6. Call dotplot context menu
//7. Alt-Tab or activate another view

//8. Return to dotplot view and call context menu again

//9. Repeat operation 3-4 times
//Expected state: menu repaints correctly, UGENE not crashed
    for(int i=0;i<4;i++){
        GTUtilsDialog::waitForDialog(os, new GTUtilsEscClicker(os, "dotplot context menu"));

        GTWidget::click(os, GTWidget::findWidget(os, GTUtilsProjectTreeView::widgetName));
        QWidget* dpWidget = GTWidget::findWidget(os, "dotplot widget");
        CHECK_SET_ERR(dpWidget != NULL, "no dpWidget");

        GTMenu::showContextMenu(os, dpWidget);
    }
}

GUI_TEST_CLASS_DEFINITION(test_0014_1){
    //DIFFERENCE: ANNOTATION TREE WIDGET IS USED
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 4));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();
    GTUtilsProjectTreeView::openView(os);

    for(int i=0;i<4;i++){
        GTUtilsDialog::waitForDialog(os, new GTUtilsEscClicker(os, "dotplot context menu"));

        GTWidget::click(os, GTWidget::findWidget(os, GTUtilsAnnotationsTreeView::widgetName));
        QWidget* dpWidget = GTWidget::findWidget(os, "dotplot widget");
        CHECK_SET_ERR(dpWidget != NULL, "no dpWidget");

        GTMenu::showContextMenu(os, dpWidget);
    }
}
GUI_TEST_CLASS_DEFINITION(test_0014_2){
    //DIFFERENCE: ANNOTATION TREE WIDGET IS USED
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 4));
    Runnable *filler2 = new BuildDotPlotFiller(os, testDir + "_common_data/scenarios/dp_view/dp1.fa","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();
    GTUtilsProjectTreeView::openView(os);

    for(int i=0;i<4;i++){
        GTUtilsDialog::waitForDialog(os, new GTUtilsEscClicker(os, "dotplot context menu", true));

        GTWidget::click(os, GTWidget::findWidget(os, GTUtilsAnnotationsTreeView::widgetName));
        QWidget* dpWidget = GTWidget::findWidget(os, "dotplot widget");
        CHECK_SET_ERR(dpWidget != NULL, "no dpWidget");

        GTMenu::showContextMenu(os, dpWidget);
    }
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
//1. Use menu {Tools->Build Dot plot}.
//Expected state: dialog "Build dotplot from sequences" has appeared.
//2. Fill the following fields in the dialog:
//    {File with first sequence} trunk\data\samples\PDB\1CF7.PDB
//    {Compare sequence against itself} checked
//3. Click Next button
//Expected state: dialog "Dotplot" has appeared.
//4. Press the "1k" button
//Expected state: minimum repeat length changed to 2bp
//5. Click OK button
//Expected state: Dotplot view has appeared.
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os,100,0, false,true));
    Runnable *filler2 = new BuildDotPlotFiller(os, dataDir + "samples/PDB/1CF7.PDB","",false,true);
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep(4000);
    GTUtilsProjectTreeView::openView(os);
    GTGlobals::sleep(500);
//6. Click on the Dotplot view

    GTWidget::click(os,GTWidget::findWidget(os,"dotplot widget"));

//Expected state: Dotplot view has been selected, UGENE didn't crash
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
//    Expected state: there is an area an repeat selected on the dotplot
//    8. Repeat step 2
//    Expected state: both checkboxes are enabled

    class DotPlotExportImageFiller : public Filler {
    public:
        DotPlotExportImageFiller(int scenario, HI::GUITestOpStatus &os)
            : Filler(os, "ImageExportForm"),
              scenario(scenario) {}
        virtual void run() {
            CHECK_SET_ERR( 1<= scenario && scenario <= 4, "Wrong scenario number");
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR( dialog, "activeModalWidget is NULL");

            QCheckBox *includeAreaCheckbox = dialog->findChild<QCheckBox*>("include_area_selection");
            CHECK_SET_ERR( includeAreaCheckbox != NULL, "inlclu_area_selection is NULL");

            QCheckBox *includeRepeatCheckbox = dialog->findChild<QCheckBox*>("include_repeat_selection");
            CHECK_SET_ERR( includeRepeatCheckbox != NULL, "include_repeat_selection is NULL");

            switch (scenario) {
            case 1:
                CHECK_SET_ERR( !includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is enabled!");
                CHECK_SET_ERR( !includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is enabled!");
                break;
            case 2:
                CHECK_SET_ERR( includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is disabled!");
                CHECK_SET_ERR( !includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is enabled!");
                break;
            case 3:
                CHECK_SET_ERR( !includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is enabled!");
                CHECK_SET_ERR( includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is disabled!");
                break;
            case 4:
                CHECK_SET_ERR( includeAreaCheckbox->isEnabled(), "include_area_selection checkbox is disabled!");
                CHECK_SET_ERR( includeRepeatCheckbox->isEnabled(), "include_repeat_selection checkbox is disabled!");
                break;
            }

            QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
            CHECK_SET_ERR(box != NULL, "buttonBox is NULL");
            QPushButton* button = box->button(QDialogButtonBox::Cancel);
            CHECK_SET_ERR(button !=NULL, "Cancel button is NULL");
            GTWidget::click(os, button);
        }

        static void performScenario(HI::GUITestOpStatus &os, int scenario) {
            GTUtilsDialog::waitForDialog(os, new DotPlotExportImageFiller(scenario, os));
            GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Dotplot" << "Save/Load" << "Save as image"));
            GTWidget::click(os, GTWidget::findWidget(os, "dotplot widget"), Qt::RightButton);
        }

    private:
        // 1, 2, 3 or 4
        int scenario;
    };


    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 50, 50));
    Runnable *filler = new BuildDotPlotFiller(os,
                                              dataDir + "/samples/Genbank/murine.gb",
                                              testDir + "_common_data/genbank/pBR322.gb");
    GTUtilsDialog::waitForDialog(os, filler);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");
    GTGlobals::sleep();

    DotPlotExportImageFiller::performScenario(os, 1);
    GTGlobals::sleep();

    GTUtilsSequenceView::selectSequenceRegion(os, 1500, 2500);
    DotPlotExportImageFiller::performScenario(os, 2);
    GTGlobals::sleep();

    GTWidget::click(os, GTWidget::findWidget(os, "dotplot widget"));
    DotPlotExportImageFiller::performScenario(os, 3);
    GTGlobals::sleep();

    GTUtilsSequenceView::selectSequenceRegion(os, 1000, 2000);
    DotPlotExportImageFiller::performScenario(os, 4);
    GTGlobals::sleep();
}

} // namespace GUITest_Common_scenarios_dp_view
} // namespace U2


