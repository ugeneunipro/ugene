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

#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsStartPage.h"

#include <base_dialogs/GTFileDialog.h>
#include "primitives/GTMenu.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTWebView.h>
#include <primitives/GTWidget.h>

#include <base_dialogs/DefaultDialogFiller.h>
#include "primitives/PopupChooser.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"

#include "GTTestsStartPage.h"
#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

namespace GUITest_common_scenarios_start_page{
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001){
//    Start UGENE
//    Press "Open file(s)" button on start page
    GTLogTracer l;
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dataDir + "samples/CLUSTALW/COI.aln"));
    GTWidget::click(os, GTWidget::findWidget(os, "openFilesButton"));
//    Expected state: File dialog is opened.
    GTGlobals::sleep(500);
    bool hasWindowsWarning = l.checkMessage("ShellExecute '#' failed");
    bool hasUnixWarning = l.checkMessage("gvfs-open: #: error opening location");
    CHECK_SET_ERR(!hasWindowsWarning, "Windows warning");
    CHECK_SET_ERR(!hasUnixWarning, "Unix warning");
}

GUI_TEST_CLASS_DEFINITION(test_0002){
//    Start UGENE
//    Press "Create workflow button" button on start page
    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));
    GTWidget::click(os, GTWidget::findWidget(os, "createWorkflowButton"));
    GTGlobals::sleep(500);
//    Expected state: WD opened.
    GTUtilsMdi::waitWindowOpened(os, "Workflow Designer - New workflow", 20000);
}

GUI_TEST_CLASS_DEFINITION(test_0003){
//    Start UGENE
//    Press "Create sequence" button
    GTUtilsDialog::waitForDialog(os, new DefaultDialogFiller(os, "CreateDocumentFromTextDialog",QDialogButtonBox::Cancel));
    GTWidget::click(os, GTWidget::findWidget(os, "createSequenceButton"));
    //    Expected: Create document from text dialog appeared
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0005){
//    Start UGENE
//    Open any file.
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Go to Start page.
    GTUtilsStartPage::openStartPage(os);
//    Expected state: File added to "Resent files" list
//    Remove file from project
    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
//    Go to Start page
    GTUtilsStartPage::openStartPage(os);
//    Click file name in "Resent files" list
    GTGlobals::sleep();
    GTWidget::click(os, GTWidget::findLabelByText(os, "COI.aln").first());
//    Expected state: file is opened
    QString name = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(name == "COI [m] COI", "unexpected window title " + name);
    GTUtilsProjectTreeView::checkItem(os, "COI.aln");
}

GUI_TEST_CLASS_DEFINITION(test_0006){
//    Start UGENE
//    Open any project.
    GTFileDialog::openFile(os, testDir+"_common_data/scenarios/project/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
//    Go to Start page.
//    Expected state: project is added to "Resent files" list
//    Close project
    GTUtilsProject::closeProject(os);
//    Go to Start page
    GTUtilsStartPage::openStartPage(os);
//    Click project name in "Resent projects" list
    GTGlobals::sleep();
    GTWidget::click(os, GTWidget::findLabelByText(os, "proj1.uprj").first());
//    Expected state: project is opened
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");
    QString expectedTitle;
    expectedTitle = "proj1 UGENE";
    GTUtilsApp::checkUGENETitle(os, expectedTitle);
}

GUI_TEST_CLASS_DEFINITION(test_0008){
//    Start UGENE
    GTGlobals::sleep();
    QString title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);

//    Use main menu: Help->Open start page
    GTMenu::clickMainMenuItem(os, QStringList() << "Help" << "Open Start Page");
    GTGlobals::sleep(500);

//    Expected state: nothing happens
    title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);
//    Close Start page
    GTUtilsMdi::click(os, GTGlobals::Close);
    GTGlobals::sleep();
    QWidget* window = GTUtilsMdi::activeWindow(os, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(window == NULL, "start page was not closed");
//    Repeat step 2
    GTMenu::clickMainMenuItem(os, QStringList() << "Help" << "Open Start Page");
    GTGlobals::sleep(500);
//    Expected state: Start page is opened
    title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title == "Start Page", "unexpected window title: " + title);
}

}

}
