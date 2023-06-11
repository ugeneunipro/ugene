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

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTWidget.h>

#include "GTTestsStartPage.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_start_page {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    //    Start UGENE
    //    Press "Open file(s)" button on start page
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dataDir + "samples/CLUSTALW/COI.aln"));
    GTWidget::click(GTWidget::findWidget("openFilesButton"));
    //    Expected state: File dialog is opened.
    bool hasWindowsWarning = lt.hasMessage("ShellExecute '#' failed");
    bool hasUnixWarning = lt.hasMessage("gvfs-open: #: error opening location");
    CHECK_SET_ERR(!hasWindowsWarning, "Windows warning");
    CHECK_SET_ERR(!hasUnixWarning, "Unix warning");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    //    Start UGENE
    //    Press "Create workflow button" button on start page
    GTUtilsDialog::waitForDialog(new StartupDialogFiller());
    GTWidget::click(GTWidget::findWidget("createWorkflowButton"));
    //    Expected state: WD opened.
    GTUtilsMdi::checkWindowIsActive("Workflow Designer - New workflow");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    //    Start UGENE
    //    Press "Create sequence" button
    GTUtilsDialog::waitForDialog(new DefaultDialogFiller("CreateDocumentFromTextDialog", QDialogButtonBox::Cancel));
    GTWidget::click(GTWidget::findWidget("createSequenceButton"));
    //    Expected: Create document from text dialog appeared
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // Open any file.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Go to Start page.
    GTUtilsStartPage::openStartPage();
    // Expected state: File is added to the "Recent files" list.
    // Remove the file from the project.
    GTUtilsProjectTreeView::click("COI.aln");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Go to Start page.
    GTUtilsStartPage::openStartPage();

    // Click the file name in the "Recent files" list.
    GTWidget::click(GTWidget::findLabelByText("COI.aln").first());
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: file is opened.
    QString name = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(name == "COI [COI.aln]", "Unexpected window title: " + name);
    GTUtilsProjectTreeView::checkItem("COI.aln");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    Start UGENE
    //    Open any project.
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    //    Go to Start page.
    //    Expected state: project is added to "Resent files" list
    //    Close project
    GTUtilsProject::closeProject(false);
    //    Go to Start page
    GTUtilsStartPage::openStartPage();
    //    Click project name in "Resent projects" list
    GTWidget::click(GTWidget::findLabelByText("proj1.uprj").first());
    //    Expected state: project is opened
    GTUtilsDocument::checkDocument("1CF7.pdb");
    GTMainWindow::checkTitle("proj1 UGENE");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    //    Start UGENE
    QString title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);

    //    Use main menu: Help->Open start page
    GTMenu::clickMainMenuItem({"Help", "Open Start Page"});

    //    Expected state: nothing happens
    title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);
    //    Close Start page
    GTUtilsMdi::click(GTGlobals::Close);
    QWidget* window = GTUtilsMdi::activeWindow({false});
    CHECK_SET_ERR(window == nullptr, "start page was not closed");
    //    Repeat step 2
    GTMenu::clickMainMenuItem({"Help", "Open Start Page"});
    //    Expected state: Start page is opened
    title = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);
}

}  // namespace GUITest_common_scenarios_start_page

}  // namespace U2
