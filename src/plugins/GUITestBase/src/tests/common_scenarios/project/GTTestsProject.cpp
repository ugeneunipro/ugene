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

#include "GTTestsProject.h"
#include <GTUtilsAssemblyBrowser.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTreeWidget.h>
#include <utils/GTKeyboardUtils.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDebug>
#include <QMdiSubWindow>
#include <QTextStream>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/AssemblyBrowserFactory.h>
#include <U2View/MaEditorFactory.h>

#include "GTGlobals.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTSequenceReadingModeDialogUtils.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/ExportProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"
#include "system/GTFile.h"
#include "utils/GTUtilsToolTip.h"

namespace U2 {

namespace GUITest_common_scenarios_project {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0004) {
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj1.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj1.uprj");
    // Expected state:
    //     1) Project view with document "1CF7.pdb" is opened
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTUtilsDocument::checkDocument("1CF7.pdb");
    //     2) UGENE window titled with text "proj1 UGENE"
    GTMainWindow::checkTitle("proj1 UGENE");

    // 2. Use menu {File->Export Project}
    // Expected state: "Export Project" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Destination folder} _common_data/scenarios/sandbox
    // 4. Click OK button
    GTUtilsDialog::waitForDialog(new ExportProjectDialogFiller(testDir + "_common_data/scenarios/sandbox/proj1.uprj"));
    GTMenu::clickMainMenuItem({"File", "Export project..."});
    GTUtilsTaskTreeView::waitTaskFinished();

    // 5. Use menu {File->Close project}
    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsProjectTreeView::checkProjectViewIsClosed();
    // Expected state: project is unloaded and project view is closed
    GTUtilsProject::checkProject(GTUtilsProject::NotExists);

    // 6. Use menu {File->Open}. Open project _common_data/sandbox/proj1.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/", "proj1.uprj");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    // Expected state:
    //     1) project view with document "1CF7.pdb" has been opened,
    GTUtilsDocument::checkDocument("1CF7.pdb");
    //     2) UGENE window titled with text "proj1 UGENE"
    GTMainWindow::checkTitle("proj1 UGENE");

    //     3) File path at tooltip for "1CF7.PDB" must be "_common_data/scenarios/sandbox/1CF7.pdb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.pdb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTUtilsToolTip::checkExistingToolTip("_common_data/scenarios/sandbox/1CF7.pdb");

    // 7. Select "1CF7.PDB" in project tree and press Enter
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.pdb"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state:
    //     1) Document is loaded,
    GTUtilsDocument::checkDocument("1CF7.pdb", AnnotatedDNAViewFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/proj1.uprj");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    GTMainWindow::checkTitle("proj1 UGENE");
    GTUtilsDocument::checkDocument("1CF7.pdb");

    GTUtilsDialog::waitForDialog(new SaveProjectAsDialogFiller("proj2", testDir + "_common_data/scenarios/sandbox/proj2"));
    GTMenu::clickMainMenuItem({"File", "Save project as..."});
    GTUtilsDialog::checkNoActiveWaiters();

    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsProjectTreeView::checkProjectViewIsClosed();

    GTFileDialog::openFile(testDir + "_common_data/scenarios/sandbox/proj2.uprj");
    GTUtilsProjectTreeView::checkProjectViewIsOpened();
    GTMainWindow::checkTitle("proj2 UGENE");
    GTUtilsDocument::checkDocument("1CF7.pdb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1CF7.pdb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTGlobals::sleep();  // todo: make checkExistingToolTip wait until tooltip or fail.
    GTUtilsToolTip::checkExistingToolTip("_common_data/pdb/1CF7.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    QString expectedTitle = "UGENE";
    GTMainWindow::checkTitle(expectedTitle);

    QMenu* m = GTMenu::showMainMenu(MWMENU_FILE);
    QAction* result = GTMenu::getMenuItem(m, ACTION_PROJECTSUPPORT__EXPORT_PROJECT, false);
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    GTUtilsProject::checkProject(GTUtilsProject::NotExists);
    CHECK_SET_ERR(result == nullptr, "Export menu item present in menu without any project created");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("1CF7.pdb");

    GTUtilsDocument::removeDocument("1CF7.pdb", GTGlobals::UseMouse);
    GTUtilsProject::checkProject(GTUtilsProject::Empty);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTFileDialog::openFile(dataDir, "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("ty3.aln.gz", MsaEditorFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTFileDialog::openFile(dataDir, "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134)", "qqq");
    GTUtilsProjectTreeView::findIndex("qqq");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    GTFileDialog::openFile(testDir, "_common_data/scenarios/project/1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new ExportProjectDialogChecker("project.uprj"));
    GTMenu::clickMainMenuItem({"File", "Export project..."});
    GTUtilsDialog::checkNoActiveWaiters();
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"Export document"}, GTGlobals::UseMouse));

    Runnable* filler = new ExportDocumentDialogFiller(testDir + "_common_data/scenarios/sandbox/", "1.gb", ExportDocumentDialogFiller::Genbank, true, true, GTGlobals::UseMouse);
    GTUtilsDialog::add(filler);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1.gb"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDocument::checkDocument("1.gb.gz");
    QString fileNames[] = {"_common_data/scenarios/project/test_0012.gb", "_common_data/scenarios/project/1.gb"};
    QString fileContent[2];

    for (int i = 0; i < 2; i++) {
        QFile file(testDir + fileNames[i]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            GT_FAIL("Can't open file \"" + testDir + fileNames[i], );
        }
        QTextStream in(&file);
        QString temp;
        temp = in.readLine();
        while (!in.atEnd()) {
            temp = in.readLine();
            fileContent[i] += temp;
        }
        file.close();
    }

    qDebug() << "file 1 = " << fileContent[0] << "file 2 = " << fileContent[1];
    if (fileContent[0] != fileContent[1]) {
        GT_FAIL("File content does not match", );
    }
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Add "Open project in new window" (0001629)

    // 1. Open project _common_data\scenario\project\proj1.uprj
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProject::checkProject();

    GTUtilsDialog::waitForDialog(new MessageBoxOpenAnotherProject());

    // 2. Do menu {File->Open}. Open project _common_data\scenario\project\proj2.uprj
    // Expected state: dialog with text "Open project in new window" has appeared

    GTFileDialog::openFileWithDialog(testDir + "_common_data/scenarios/project/", "proj2.uprj");

    /*
    this test just checking appearing of dialog not its behavior
    */
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("1HTQ", 3));
    GTMenu::clickMainMenuItem({"File", "Access remote database..."}, GTGlobals::UseKey);

    GTUtilsTaskTreeView::openView();
    GTUtilsTaskTreeView::cancelTask("Download remote documents");
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    GTFileDialog::openFile(testDir + "_common_data/genbank/.dir/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"Export document"}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("murine.gb"));
    GTUtilsDialog::add(new ExportDocumentDialogFiller(testDir + "_common_data/genbank/.dir/", "murine_copy1.gb", ExportDocumentDialogFiller::Genbank, false, true, GTGlobals::UseMouse));

    GTMouseDriver::click(Qt::RightButton);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("murine_copy1.gb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));

    GTUtilsToolTip::checkExistingToolTip(".dir");
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    GTUtilsDialog::waitForDialog(new GTSequenceReadingModeDialogUtils());
    GTUtilsProject::openFiles(QList<QUrl>() << dataDir + "samples/Genbank/murine.gb" << dataDir + "samples/Genbank/sars.gb" << dataDir + "samples/Genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("murine.gb");
    GTUtilsDocument::checkDocument("sars.gb");
    GTUtilsDocument::checkDocument("CVU55762.gb");
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::rename("human_T1 (UCSC April 2002 chr7:115977709-117855134)", "qqq");
    GTUtilsProjectTreeView::rename("qqq", "eee");
    GTUtilsDocument::removeDocument("human_T1.fa");

    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex("human_T1.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTFileDialog::openFileWithDialog(testDir + "_common_data/scenarios/project/", "multiple.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    QModelIndex se1 = GTUtilsProjectTreeView::findIndex("se1");
    QModelIndex se2 = GTUtilsProjectTreeView::findIndex("se2");
    QFont fse1 = GTUtilsProjectTreeView::getFont(se1);
    QFont fse2 = GTUtilsProjectTreeView::getFont(se2);

    CHECK_SET_ERR(fse1.bold(), "se1 are not marked with bold text");
    CHECK_SET_ERR(fse2.bold(), "se2 are not marked with bold text");

    auto w = GTWidget::findWidget("render_area_se1");

    GTUtilsDialog::waitForDialog(new PopupChooser({"ADV_MENU_REMOVE", ACTION_EDIT_SELECT_SEQUENCE_FROM_VIEW}));
    GTMenu::showContextMenu(w);

    QFont fse1_2 = GTUtilsProjectTreeView::getFont(se1);
    CHECK_SET_ERR(!fse1_2.bold(), "se1 is not marked with regular text");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTFileDialog::openFileWithDialog(testDir + "_common_data/scenarios/project/", "multiple.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    QModelIndex se1 = GTUtilsProjectTreeView::findIndex("se1");
    GTUtilsProjectTreeView::itemActiveCheck(se1);

    QModelIndex se2 = GTUtilsProjectTreeView::findIndex("se2");
    GTUtilsProjectTreeView::itemActiveCheck(se2);

    GTUtilsMdi::closeActiveWindow();
    GTUtilsProjectTreeView::itemActiveCheck(se1, false);
    GTUtilsProjectTreeView::itemActiveCheck(se2, false);

    GTUtilsSequenceView::openSequenceView("se1");
    GTUtilsProjectTreeView::itemActiveCheck(se1);

    GTUtilsSequenceView::addSequenceView("se2");
    GTUtilsProjectTreeView::itemActiveCheck(se2);
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller());
    GTFileDialog::openFileWithDialog(testDir + "_common_data/scenarios/project/", "multiple.fa");
    GTUtilsDialog::checkNoActiveWaiters();

    QModelIndex item = GTUtilsProjectTreeView::findIndex("se1");
    QFont font = GTUtilsProjectTreeView::getFont(item);
    CHECK_SET_ERR(font.bold(), "se1 item font is not a bold_1");
    item = GTUtilsProjectTreeView::findIndex("se2");
    font = GTUtilsProjectTreeView::getFont(item);
    CHECK_SET_ERR(font.bold(), "se2 item font is not a bold_1");

    GTUtilsMdi::closeActiveWindow();
    item = GTUtilsProjectTreeView::findIndex("se1");
    font = GTUtilsProjectTreeView::getFont(item);
    CHECK_SET_ERR(!font.bold(), "se1 item font is not a bold_2");

    GTUtilsSequenceView::openSequenceView("se1");
    item = GTUtilsProjectTreeView::findIndex("se1");
    font = GTUtilsProjectTreeView::getFont(item);
    CHECK_SET_ERR(font.bold(), "se1 item font is not a bold_3");

    GTUtilsSequenceView::openSequenceView("se2");
    item = GTUtilsProjectTreeView::findIndex("se2");
    font = GTUtilsProjectTreeView::getFont(item);
    CHECK_SET_ERR(font.bold(), "se2 item font is not a bold_2");
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    GTFileDialog::openFile(testDir + "_common_data/scenarios/project/1m.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* w = GTUtilsMdi::findWindow("Multiple alignment [1m.fa]");
    CHECK_SET_ERR(w != nullptr, "Sequence view window title is not 1m [m] Multiple alignment");
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    const QString filePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    const QString fileName = "proj4.uprj";
    const QString firstAnn = testDir + "_common_data/scenarios/project/1.gb";
    const QString firstAnnFileName = "1.gb";
    const QString secondAnn = testDir + "_common_data/scenarios/project/2.gb";
    const QString secondAnnFileName = "2.gb";

    GTFile::copy(filePath, sandBoxDir + "/" + fileName);
    GTFile::copy(firstAnn, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnn, sandBoxDir + "/" + secondAnnFileName);
    GTFileDialog::openFile(sandBoxDir, fileName);
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    GTUtilsDialog::waitForDialog(new PopupChooser({"action_load_selected_documents"}, GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("1.gb"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "<auto>", "misc_feature", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));

    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("sars.gb", AnnotatedDNAViewFactory::ID);
    GTUtilsDocument::removeDocument("sars.gb");
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    QMdiSubWindow* fasta = (QMdiSubWindow*)GTUtilsMdi::findWindow("human_T1 (UCSC April 2002 chr7:115977709-117855134) [human_T1.fa]");

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    QWidget* coi = GTUtilsMdi::findWindow("COI [COI.aln]");
    CHECK_SET_ERR(fasta->windowIcon().cacheKey() != coi->windowIcon().cacheKey(), "Icons must not be equals");
    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    GTLogTracer lt;
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::Cancel));
    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsDialog::checkNoActiveWaiters();

    CHECK_SET_ERR(!lt.hasErrors(), "Found errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument("human_T1.fa");

    GTUtilsProjectTreeView::openView();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto nameFilterEdit = GTWidget::findLineEdit("nameFilterEdit");
    GTLineEdit::setText(nameFilterEdit, "BBBB");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMdi::click(GTGlobals::Close);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFileWithDialog(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    //    ACE format can be opened both as assembly and as alignment

    //    1. Open "_common_data/ACE/ace_test_1.ace".
    //    Expected state: a dialog appears, it offers to select a view to open the file with.

    //    2. Select "Open as multiple sequence alignment" item, accept the dialog.
    //    Expected state: file opens, document contains two malignment objects, the MSA Editor is shown.
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTFileDialog::openFileWithDialog(testDir + "_common_data/ace/", "ace_test_1.ace");
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDocument::checkDocument("ace_test_1.ace", MsaEditorFactory::ID);
    GTUtilsProjectTreeView::checkObjectTypes(
        QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT,
        GTUtilsProjectTreeView::findIndex("ace_test_1.ace"));

    //    3. Open "_common_data/ACE/ace_test_2.ace".
    //    Expected state: a dialog appears, it offers to select a view to open the file with.

    //    4. Select "Open as assembly" item, accept the dialog.
    //    Expected state: file opens, document contains two assembly objects and two sequence objects, the Assembly Browser is shown.
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "project_test_0033.ugenedb"));
    GTFileDialog::openFileWithDialog(testDir + "_common_data/ace/", "ace_test_2.ace");
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsDocument::checkDocument("project_test_0033.ugenedb", AssemblyBrowserFactory::ID);
    GTUtilsProjectTreeView::checkObjectTypes(
        QSet<GObjectType>() << GObjectTypes::SEQUENCE << GObjectTypes::ASSEMBLY,
        GTUtilsProjectTreeView::findIndex("project_test_0033.ugenedb"));
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // select sequence object
    GTUtilsProjectTreeView::click("murine.gb");
    GTUtilsDialog::waitForDialog(new PopupChecker({"openInMenu", "openContainingFolderAction"}, PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click("murine.gb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // select 2 objects
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click("NC_001363");
    GTUtilsProjectTreeView::click("NC_004718");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsDialog::waitForDialog(new PopupChecker({"openInMenu", "openContainingFolderAction"}, PopupChecker::Exists, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click("NC_001363", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // select 2 files
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click("sars.gb");
    GTUtilsProjectTreeView::click("murine.gb");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsDialog::waitForDialog(new PopupChecker({"openInMenu", "openContainingFolderAction"}, PopupChecker::Exists, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click("sars.gb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // select 1 file
    GTUtilsProjectTreeView::click("sars.gb");
    GTUtilsDialog::waitForDialog(new PopupChecker({"openInMenu", "openContainingFolderAction"}, PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click("sars.gb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    // test for several alignments in one document
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(true));
    GTFileDialog::openFileWithDialog(dataDir + "samples/ACE/", "BL060C3.ace");
    GTUtilsDialog::checkNoActiveWaiters();

    // check for first document
    GTUtilsProjectTreeView::doubleClickItem("Contig1");
    QString title1 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title1 == "Contig1 [BL060C3.ace]", "unexpected title for doc1: " + title1);

    // check for second document
    GTUtilsProjectTreeView::doubleClickItem("Contig2");
    QString title2 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title2 == "Contig2 [BL060C3.ace]", "unexpected title for doc2: " + title2);

    // reopening windows z
    while (GTUtilsMdi::activeWindow({false}) != nullptr) {
        GTUtilsMdi::closeActiveWindow();
    }
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("BL060C3.ace", Qt::RightButton);

    // check for first document
    GTUtilsProjectTreeView::doubleClickItem("Contig1");
    GTThread::waitForMainThread();
    title1 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title1 == "Contig1 [BL060C3.ace]", "unexpected title for doc1: " + title1);

    // check for second document
    GTUtilsProjectTreeView::doubleClickItem("Contig2");
    GTThread::waitForMainThread();
    title2 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title2 == "Contig2 [BL060C3.ace]", "unexpected title for doc2: " + title2);
}

GUI_TEST_CLASS_DEFINITION(test_0038_1) {
    // test for several assembly documents in one document
    GTUtilsDialog::waitForDialog(new ImportACEFileFiller(false, sandBoxDir + "test_3637_1.ugenedb"));
    GTFileDialog::openFileWithDialog(dataDir + "samples/ACE/", "BL060C3.ace");
    GTUtilsDialog::checkNoActiveWaiters();

    // check for first document
    GTUtilsProjectTreeView::doubleClickItem("Contig1");
    QString title1 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title1 == "Contig1 [test_3637_1.ugenedb]", "unexpected title for doc1: " + title1);

    // check for first document
    GTUtilsProjectTreeView::doubleClickItem("Contig2");
    QString title2 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title2 == "Contig2 [test_3637_1.ugenedb]", "unexpected title for doc2: " + title2);

    // reopening windows
    while (GTUtilsMdi::activeWindow({false}) != nullptr) {
        GTUtilsMdi::closeActiveWindow();
    }
    GTUtilsDialog::waitForDialog(new PopupChooser({"openInMenu", "action_open_view"}));
    GTUtilsProjectTreeView::click("test_3637_1.ugenedb", Qt::RightButton);

    // check for first document
    GTUtilsProjectTreeView::doubleClickItem("Contig1");
    title1 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title1 == "Contig1 [test_3637_1.ugenedb]", "unexpected title for doc1: " + title1);

    // check for second document
    GTUtilsProjectTreeView::doubleClickItem("Contig2");
    title2 = GTUtilsMdi::activeWindowTitle();
    CHECK_SET_ERR(title2 == "Contig2 [test_3637_1.ugenedb]", "unexpected title for doc2: " + title2);
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1 (UCS\r\nACGT\r\nACG");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex("human_T1 (UCS");
}

QString readFileToStr(const QString& path) {
    GUrl url(path);
    QFile f(url.getURLString());
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }
    QTextStream in(&f);
    return in.readAll();
}

GUI_TEST_CLASS_DEFINITION(test_0040) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check adding document with 2 sequences in separate mode
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsDialog::checkNoActiveWaiters();

    GTUtilsProjectTreeView::findIndex("human_T1");
    GTUtilsProjectTreeView::findIndex("human_T2");
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check shift+insert instead Ctrl+V
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1 (UCS\nACGT\nACG");

    GTKeyboardDriver::keyClick(Qt::Key_Insert, Qt::ShiftModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCS");
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    // check adding schemes (WD QD) in project, it should not appear in project view
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "workflow_samples/Alignment/basic_align.uwl");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new StartupDialogFiller());

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(GTUtilsMdi::activeWindowTitle().contains("Workflow Designer"), "Mdi window is not a WD window");
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check newick format because there was crash
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Newick/COI.nwk");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("Tree");
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check document which format cant be saved by UGENE
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Stockholm/CBS.sto");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("CBS");
}

GUI_TEST_CLASS_DEFINITION(test_0046) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check document which format can't be saved by UGENE has no locked state
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/PDB/1CF7.PDB");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("1CF7");

    GTUtilsProjectTreeView::itemModificationCheck(GTUtilsProjectTreeView::findIndex("clipboard.pdb"), false);
    GTUtilsStartPage::openStartPage();
    GTWidget::findLabelByText("clipboard.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0047) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check document which format can't be saved by UGENE has no locked state
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

GUI_TEST_CLASS_DEFINITION(test_0048) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // pasting same data 10 times
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    for (int i = 0; i < 10; i++) {
        GTUtilsProjectTreeView::click("COI.aln");
        GTClipboard::setText(QString(">human_T%1\r\nACGT\r\nACG").arg(QString::number(i)));
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
        uiLog.trace(QString("item number %1 inserted").arg(i));
    }

    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndexList list = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait("");
    uiLog.trace("All items in project tree view:");
    foreach (QModelIndex index, list) {
        uiLog.trace(index.data().toString());
    }

    for (int i = 0; i < 10; i++) {
        GTUtilsProjectTreeView::findIndex(QString("human_T%1").arg(QString::number(i)));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0049) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check no crash after closing project without saving
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsProject::checkProject(GTUtilsProject::NotExists);
}

GUI_TEST_CLASS_DEFINITION(test_0050) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //'usual' scenario
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTUtilsTaskTreeView::waitTaskFinished();
    QFile savedFile(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/clipboard.fa");
    CHECK_SET_ERR(savedFile.exists(), "Saved file is not found");
}

GUI_TEST_CLASS_DEFINITION(test_0051) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check adding document with 2 sequences in align mode
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Join));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex("Multiple alignment");
}

GUI_TEST_CLASS_DEFINITION(test_0052) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check adding document with 2 sequences in merge mode
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Merge));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::findIndex("Sequence");
    GTUtilsProjectTreeView::findIndex("Contigs");
}

GUI_TEST_CLASS_DEFINITION(test_0053) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    // check adding document with 2 sequences in separate mode.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/multy_fa.fa");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_0054) {
    // check adding document with 2 sequences in separate mode, with file which cannot be written by UGENE
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/genbank/multi.gb");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate, 10, true));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0055) {
    // check document format dialog cancelling
    class CustomScenarioCancel : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFileWithDialog(dataDir + "samples/CLUSTALW/", "COI.aln");

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/broken/broken_doc.fa");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller(new CustomScenarioCancel()));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0056) {
    // check opening broken fasta document as text

    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/broken/broken_doc.fa");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("Plain text"));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0057) {
    // check adding document with 2 sequences in short reads mode
    class CheckPathScenario : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto treeWidget = GTWidget::findTreeWidget("shortReadsTable", dialog);
            QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
            QTreeWidgetItem* firstItem = treeItems.first();
            QString path = firstItem->text(0);
            CHECK_SET_ERR(!path.isEmpty(), "Reads filepath should not be empty");
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFileWithDialog(dataDir + "samples/CLUSTALW/", "COI.aln");

    GTUtilsProjectTreeView::click("COI.aln");
    GTClipboard::setText(">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(new AlignShortReadsFiller(new CheckPathScenario()));
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Align));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    // 1. Open a project.
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Paste the content of the file "samples/Assembly/chrM.sam".
    GTUtilsProjectTreeView::click("COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Assembly/chrM.sam");
    GTClipboard::setText(fileContent);

    GTUtilsDialog::waitForDialog(new ImportBAMFileFiller(sandBoxDir + "project_test_0058/project_test_0058.ugenedb", "", "", true));
    GTUtilsDialog::waitForDialog(new DocumentFormatSelectorDialogFiller("BAM/SAM file import"));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsDialog::checkNoActiveWaiters();
    GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive();
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Move focus to the project view from MSA
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    GTClipboard::setUrls({dataDir + "samples/FASTA/human_T1.fa"});
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Move focus to the project view from MSA Editor.
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    GTClipboard::setText("ACGT");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("clipboard", GTGlobals::FindOptions(true, Qt::MatchContains));
}

GUI_TEST_CLASS_DEFINITION(test_0061) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Move focus to the project view from Sequence view.
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    GTClipboard::setUrls({dataDir + "samples/FASTA/human_T1.fa"});
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Move focus to the project view from object view.
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    GTClipboard::setUrls({dataDir + "samples/FASTA/human_T1.fa", dataDir + "samples/HMM/aligment15900.hmm"});
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsProjectTreeView::findIndex("aligment15900");
}

GUI_TEST_CLASS_DEFINITION(test_0063) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::waitForDialog(new SaveProjectDialogFiller(QDialogButtonBox::No));
    GTMenu::clickMainMenuItem({"File", "Close project"});

    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[8] == "human_T1 (UCSC April 2002 chr7:115977709-117855134)", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0065) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTACG\r\n");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[8] == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0066) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsMSAEditorSequenceArea::selectArea(QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(">human_T1\r\nACGTACG\r\n");

    GTUtilsDialog::waitForDialog(new PopupChooser({"MSAE_MENU_COPY", "paste"}));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList();
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList[8] == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0067) {
    GTFileDialog::openFile(dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Move focus to the project view from object view.
    GTWidget::click(GTUtilsProjectTreeView::getTreeView());

    GTClipboard::setUrls({dataDir + "samples/FASTA/human_T1.fa"});
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

// seq
GUI_TEST_CLASS_DEFINITION(test_0068) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 2);
    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    QAction* editMode = GTAction::findActionByText("Switch on the editing mode");
    CHECK_SET_ERR(editMode != nullptr, "Cannot find Edit mode action");
    GTWidget::click(GTAction::button(editMode));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    int len = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 2);
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    QAction* editMode = GTAction::findActionByText("Switch on the editing mode");
    CHECK_SET_ERR(editMode != nullptr, "Cannot find Edit mode action");
    GTWidget::click(GTAction::button(editMode));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    int len = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0070) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsSequenceView::selectSequenceRegion(1, 2);
    GTClipboard::setText(">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    GTUtilsSequenceView::enableEditingMode(true);
    GTKeyboardUtils::paste();
    GTUtilsTaskTreeView::waitTaskFinished();
    int len = GTUtilsSequenceView::getLengthOfSequence();
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0071) {
    GTFileDialog::openFile(dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/Genbank/sars.gb");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("NC_004718");
}

// ann
GUI_TEST_CLASS_DEFINITION(test_0072) {
    // TODO: fix the test.
    // Ctrl+Shift+V в GUI-test?
    // UGENE-4907
    /*
    GTUtilsProject::openFiles(dataDir + "samples/Genbank/murine.gb");
    //select annotations
    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/Genbank/sars.gb");
    //Ctrl+Shift+V в GUI-test?
    //GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier + Qt::Key_Shift);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    */
}

GUI_TEST_CLASS_DEFINITION(test_0073) {
    // TODO: fix the test!
    // Ctrl+Shift+V в GUI-test?
    /*
    GTUtilsProject::openFiles(dataDir + "samples/Genbank/murine.gb");

    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    */
}

GUI_TEST_CLASS_DEFINITION(test_0074) {
    GTFileDialog::openFile(dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTClipboard::setUrls(QList<QString>() << dataDir + "samples/Genbank/sars.gb");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("NC_004718");
}

}  // namespace GUITest_common_scenarios_project

}  // namespace U2
