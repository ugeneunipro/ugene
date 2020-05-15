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

#include "GTTestsProject.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWebView.h>
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

#include <U2View/ADVConstants.h>
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
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AlignShortReadsDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportACEFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"
#include "runnables/ugene/ugeneui/ConvertAceToSqliteDialogFiller.h"
#include "runnables/ugene/ugeneui/CreateNewProjectWidgetFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentProviderSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/ExportProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "system/GTClipboard.h"
#include "system/GTFile.h"
#include "utils/GTUtilsApp.h"
#include "utils/GTUtilsToolTip.h"

namespace U2 {

namespace GUITest_common_scenarios_project {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0004) {
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj1.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1CF7.pdb" is opened
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");
    //     2) UGENE window titled with text "proj1 UGENE"
    QString expectedTitle;
#ifdef Q_OS_MAC
    expectedTitle = "proj1 UGENE";
#else
    expectedTitle = "proj1 UGENE";
#endif
    GTUtilsApp::checkUGENETitle(os, expectedTitle);

    // 2. Use menu {File->Export Project}
    // Expected state: "Export Project" dialog has appeared
    //
    // 3. Fill the next field in dialog:
    //     {Destination folder} _common_data/scenarios/sandbox
    // 4. Click OK button
    GTUtilsDialog::waitForDialog(os, new ExportProjectDialogFiller(os, testDir + "_common_data/scenarios/sandbox/proj1.uprj"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Export project...");
    GTGlobals::sleep();

    // 5. Use menu {File->Close project}
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
    GTGlobals::sleep();

    // Expected state: project is unloaded and project view is closed
    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);

    // 6. Use menu {File->Open}. Open project _common_data/sandbox/proj1.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/sandbox/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    // Expected state:
    //     1) project view with document "1CF7.pdb" has been opened,
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");
    //     2) UGENE window titled with text "proj1 UGENE"
    GTUtilsApp::checkUGENETitle(os, expectedTitle);

    //     3) File path at tooltip for "1CF7.PDB" must be "_common_data/scenarios/sandbox/1CF7.pdb"
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7.pdb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTGlobals::sleep();
    GTUtilsToolTip::checkExistingToolTip(os, "_common_data/scenarios/sandbox/1CF7.pdb");

    // 7. Select "1CF7.PDB" in project tree and press Enter
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7.pdb"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    // Expected state:
    //     1) Document is loaded,
    GTUtilsDocument::checkDocument(os, "1CF7.pdb", AnnotatedDNAViewFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/proj1.uprj");
    QString expectedTitle;
#ifdef Q_OS_MAC
    expectedTitle = "proj1 UGENE";
#else
    expectedTitle = "proj1 UGENE";
#endif
    GTUtilsApp::checkUGENETitle(os, expectedTitle);
    GTGlobals::sleep(4000);
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");

    GTUtilsDialog::waitForDialog(os, new SaveProjectAsDialogFiller(os, "proj2", testDir + "_common_data/scenarios/sandbox/proj2"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Save project as...");
    GTGlobals::sleep();

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
    GTGlobals::sleep();

    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/sandbox/proj2.uprj");
#ifdef Q_OS_MAC
    expectedTitle = "proj2 UGENE";
#else
    expectedTitle = "proj2 UGENE";
#endif
    GTUtilsApp::checkUGENETitle(os, expectedTitle);
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1CF7.pdb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTGlobals::sleep();
    GTUtilsToolTip::checkExistingToolTip(os, "_common_data/pdb/1CF7.pdb");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    QString expectedTitle;
#ifdef Q_OS_MAC
    expectedTitle = "UGENE";
#else
    expectedTitle = "UGENE";
#endif
    GTUtilsApp::checkUGENETitle(os, expectedTitle);

    QMenu *m = GTMenu::showMainMenu(os, MWMENU_FILE);
    QAction *result = GTMenu::getMenuItem(os, m, ACTION_PROJECTSUPPORT__EXPORT_PROJECT, false);
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
    GTGlobals::sleep(100);

    GTUtilsProject::checkProject(os, GTUtilsProject::NotExists);
    CHECK_SET_ERR(result == NULL, "Export menu item present in menu without any project created");
}
GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/proj1.uprj");
    GTUtilsDocument::checkDocument(os, "1CF7.pdb");

    GTUtilsDocument::removeDocument(os, "1CF7.pdb", GTGlobals::UseMouse);
    GTUtilsProject::checkProject(os, GTUtilsProject::Empty);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/ty3.aln.gz");
    GTUtilsDocument::checkDocument(os, "ty3.aln.gz", MsaEditorFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsProjectTreeView::rename(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", "qqq");
    GTUtilsProjectTreeView::findIndex(os, "qqq");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/1.gb");

    GTUtilsDialog::waitForDialog(os, new ExportProjectDialogChecker(os, "project.uprj"));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Export project...");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "1.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export document", GTGlobals::UseMouse));

    Runnable *filler = new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "1.gb", ExportDocumentDialogFiller::Genbank, true, true, GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1.gb"));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsDocument::checkDocument(os, "1.gb.gz");
    GTGlobals::sleep(100);
    QString fileNames[] = {"_common_data/scenarios/project/test_0012.gb", "_common_data/scenarios/project/1.gb"};
    QString fileContent[2];

    for (int i = 0; i < 2; i++) {
        QFile file(testDir + fileNames[i]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (!os.hasError()) {
                os.setError("Can't open file \"" + testDir + fileNames[i]);
            }
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
    if (fileContent[0] != fileContent[1] && !os.hasError()) {
        os.setError("File is not expected file");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    //Add "Open project in new window" (0001629)

    //1. Open project _common_data\scenario\project\proj1.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj1.uprj");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProject::checkProject(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxOpenAnotherProject(os));

    //2. Do menu {File->Open}. Open project _common_data\scenario\project\proj2.uprj
    //Expected state: dialog with text "Open project in new window" has appear

    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/proj2.uprj");    // TODO: ask Shutov what to do

    /*
    this test just checking appearing of dialog not its behavior
    */
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    GTUtilsDialog::waitForDialog(os, new RemoteDBDialogFillerDeprecated(os, "1HTQ", 3));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Access remote database...",
                              GTGlobals::UseKey);

    GTGlobals::sleep();

    GTUtilsTaskTreeView::openView(os);
    GTUtilsTaskTreeView::cancelTask(os, "Download remote documents");
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/genbank/.dir/murine.gb");
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export document", GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "murine.gb"));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/genbank/.dir/", "murine_copy1.gb", ExportDocumentDialogFiller::Genbank, false, true, GTGlobals::UseMouse));

    GTMouseDriver::click(Qt::RightButton);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "murine_copy1.gb"));
    GTMouseDriver::moveTo(GTMouseDriver::getMousePosition() + QPoint(5, 5));
    GTGlobals::sleep();

    GTUtilsToolTip::checkExistingToolTip(os, ".dir");
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    GTUtilsProject::openFiles(os, QList<QUrl>() << dataDir + "samples/Genbank/murine.gb" << dataDir + "samples/Genbank/sars.gb" << dataDir + "samples/Genbank/CVU55762.gb");
    GTUtilsDocument::checkDocument(os, "murine.gb");
    GTUtilsDocument::checkDocument(os, "sars.gb");
    GTUtilsDocument::checkDocument(os, "CVU55762.gb");
}
GUI_TEST_CLASS_DEFINITION(test_0018) {
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsProjectTreeView::rename(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", "qqq");
    GTUtilsProjectTreeView::rename(os, "qqq", "eee");
    GTUtilsDocument::removeDocument(os, "human_T1.fa");

    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsProjectTreeView::findIndex(os, "human_T1.fa");
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep(1000);

    QModelIndex se1 = GTUtilsProjectTreeView::findIndex(os, "se1");
    QModelIndex se2 = GTUtilsProjectTreeView::findIndex(os, "se2");
    QFont fse1 = GTUtilsProjectTreeView::getFont(os, se1);
    QFont fse2 = GTUtilsProjectTreeView::getFont(os, se2);

    CHECK_SET_ERR(fse1.bold(), "se1 are not marked with bold text");
    CHECK_SET_ERR(fse2.bold(), "se2 are not marked with bold text");

    QWidget *w = GTWidget::findWidget(os, "render_area_se1");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_REMOVE" << ACTION_EDIT_SELECT_SEQUENCE_FROM_VIEW));
    GTMenu::showContextMenu(os, w);
    GTGlobals::sleep(1000);

    QFont fse1_2 = GTUtilsProjectTreeView::getFont(os, se1);
    CHECK_SET_ERR(!fse1_2.bold(), "se1 is not marked with regular text");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep(1000);

    QModelIndex se1 = GTUtilsProjectTreeView::findIndex(os, "se1");
    GTUtilsProjectTreeView::itemActiveCheck(os, se1);

    QModelIndex se2 = GTUtilsProjectTreeView::findIndex(os, "se2");
    GTUtilsProjectTreeView::itemActiveCheck(os, se2);

    GTUtilsMdi::closeActiveWindow(os);
    GTUtilsProjectTreeView::itemActiveCheck(os, se1, false);
    GTUtilsProjectTreeView::itemActiveCheck(os, se2, false);

    GTUtilsSequenceView::openSequenceView(os, "se1");
    GTUtilsProjectTreeView::itemActiveCheck(os, se1);

    GTUtilsSequenceView::addSequenceView(os, "se2");
    GTUtilsProjectTreeView::itemActiveCheck(os, se2);
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTGlobals::sleep(1000);

    QModelIndex item = GTUtilsProjectTreeView::findIndex(os, "se1");
    QFont font = GTUtilsProjectTreeView::getFont(os, item);
    CHECK_SET_ERR(font.bold(), "se1 item font is not a bold_1");
    item = GTUtilsProjectTreeView::findIndex(os, "se2");
    font = GTUtilsProjectTreeView::getFont(os, item);
    CHECK_SET_ERR(font.bold(), "se2 item font is not a bold_1");

    GTUtilsMdi::closeActiveWindow(os);
    GTGlobals::sleep(1000);
    item = GTUtilsProjectTreeView::findIndex(os, "se1");
    font = GTUtilsProjectTreeView::getFont(os, item);
    CHECK_SET_ERR(!font.bold(), "se1 item font is not a bold_2");

    GTUtilsSequenceView::openSequenceView(os, "se1");
    item = GTUtilsProjectTreeView::findIndex(os, "se1");
    font = GTUtilsProjectTreeView::getFont(os, item);
    CHECK_SET_ERR(font.bold(), "se1 item font is not a bold_3");

    GTUtilsSequenceView::openSequenceView(os, "se2");
    item = GTUtilsProjectTreeView::findIndex(os, "se2");
    font = GTUtilsProjectTreeView::getFont(os, item);
    CHECK_SET_ERR(font.bold(), "se2 item font is not a bold_2");
}

GUI_TEST_CLASS_DEFINITION(test_0023) {
    GTUtilsProject::openFiles(os, testDir + "_common_data/scenarios/project/1m.fa");
    GTGlobals::sleep();
    // GTUtilsMdi::click(os, GTGlobals::Minimize);

    QWidget *w = GTUtilsMdi::findWindow(os, "1m [m] Multiple alignment");
    CHECK_SET_ERR(w != NULL, "Sequence view window title is not 1m [m] Multiple alignment");
}

GUI_TEST_CLASS_DEFINITION(test_0025) {
    const QString filePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    const QString fileName = "proj4.uprj";
    const QString firstAnn = testDir + "_common_data/scenarios/project/1.gb";
    const QString firstAnnFileName = "1.gb";
    const QString secondAnn = testDir + "_common_data/scenarios/project/2.gb";
    const QString secondAnnFaleName = "2.gb";

    GTFile::copy(os, filePath, sandBoxDir + "/" + fileName);
    GTFile::copy(os, firstAnn, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnn, sandBoxDir + "/" + secondAnnFaleName);
    GTFileDialog::openFile(os, sandBoxDir, fileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "action_load_selected_documents", GTGlobals::UseMouse));
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "1.gb"));
    GTGlobals::sleep(1000);
    GTMouseDriver::click(Qt::RightButton);

    GTGlobals::sleep(500);
    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "<auto>", "misc_feature", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));

    GTKeyboardDriver::keyClick('q', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0026) {
    GTUtilsProject::openFiles(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsDocument::checkDocument(os, "sars.gb", AnnotatedDNAViewFactory::ID);
    GTUtilsDocument::removeDocument(os, "sars.gb");
}

GUI_TEST_CLASS_DEFINITION(test_0028) {
    GTLogTracer logTracer;
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");
    QMdiSubWindow *fasta = (QMdiSubWindow *)GTUtilsMdi::findWindow(os, "human_T1 [s] human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");
    QWidget *coi = GTUtilsMdi::findWindow(os, "COI [m] COI");
    CHECK_SET_ERR(fasta->windowIcon().cacheKey() != coi->windowIcon().cacheKey(), "Icons must not be equals");
    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_0030) {
    GTLogTracer logTracer;
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");

    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::Cancel));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
    GTGlobals::sleep();

    GTUtilsLog::check(os, logTracer);
}

GUI_TEST_CLASS_DEFINITION(test_0031) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, "human_T1.fa");

    GTUtilsProjectTreeView::openView(os);
    GTGlobals::sleep();

    QLineEdit *nameFilterEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "nameFilterEdit"));
    GTLineEdit::setText(os, nameFilterEdit, "BBBB");
    GTGlobals::sleep();

    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsProject::openFiles(os, dataDir + "samples/FASTA/human_T1.fa");

    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0033) {
    //    ACE format can be opened both as assembly and as alignment

    //    1. Open "_common_data/ACE/ace_test_1.ace".
    //    Expected state: a dialog appears, it offers to select a view to open the file with.

    //    2. Select "Open as multiple sequence alignment" item, accept the dialog.
    //    Expected state: file opens, document contains two malignment objects, the MSA Editor is shown.
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, true));
    GTFileDialog::openFile(os, testDir + "_common_data/ace/", "ace_test_1.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "ace_test_1.ace", MsaEditorFactory::ID);
    GTUtilsProjectTreeView::checkObjectTypes(os,
                                             QSet<GObjectType>() << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT,
                                             GTUtilsProjectTreeView::findIndex(os, "ace_test_1.ace"));

    //    3. Open "_common_data/ACE/ace_test_2.ace".
    //    Expected state: a dialog appears, it offers to select a view to open the file with.

    //    4. Select "Open as assembly" item, accept the dialog.
    //    Expected state: file opens, document contains two assembly objects and two sequence objects, the Assembly Browser is shown.
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "project_test_0033.ugenedb"));
    //GTUtilsDialog::waitForDialog(os, new DocumentProviderSelectorDialogFiller(os, DocumentProviderSelectorDialogFiller::AssemblyBrowser));
    //GTUtilsDialog::waitForDialog(os, new ConvertAceToSqliteDialogFiller(os, sandBoxDir + "project_test_0033.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/ace/", "ace_test_2.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, "project_test_0033.ugenedb", AssemblyBrowserFactory::ID);
    GTUtilsProjectTreeView::checkObjectTypes(os,
                                             QSet<GObjectType>() << GObjectTypes::SEQUENCE << GObjectTypes::ASSEMBLY,
                                             GTUtilsProjectTreeView::findIndex(os, "project_test_0033.ugenedb"));
}

GUI_TEST_CLASS_DEFINITION(test_0034) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //select sequence object
    GTUtilsProjectTreeView::click(os, "murine.gb");
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Open containing folder", PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "murine.gb", Qt::RightButton);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0035) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //select 2 objects
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click(os, "NC_001363");
    GTUtilsProjectTreeView::click(os, "NC_004718");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Open containing folder", PopupChecker::NotExists, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "NC_001363", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0036) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //select 2 files
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click(os, "sars.gb");
    GTUtilsProjectTreeView::click(os, "murine.gb");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Open containing folder", PopupChecker::NotExists, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "sars.gb", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0037) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //select 1 file
    GTUtilsProjectTreeView::click(os, "sars.gb");
    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, QStringList() << "Open containing folder", PopupChecker::IsEnabled, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "sars.gb", Qt::RightButton);
    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_0038) {
    //test for several alignments in one document
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, true));
    GTFileDialog::openFile(os, dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //check for first document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig1");
    QString title1 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title1 == "BL060C3 [m] Contig1", "unexpected title for doc1: " + title1);

    //check for second document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig2");
    QString title2 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title2 == "BL060C3 [m] Contig2", "unexpected title for doc2: " + title2);

    //reopening windows z
    while (GTUtilsMdi::activeWindow(os, GTGlobals::FindOptions(false)) != NULL) {
        GTUtilsMdi::closeActiveWindow(os);
    }
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Open View"
                                                                        << "action_open_view"));
    GTUtilsProjectTreeView::click(os, "BL060C3.ace", Qt::RightButton);

    //check for first document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig1");
    GTThread::waitForMainThread();
    GTGlobals::sleep();
    title1 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title1 == "BL060C3 [m] Contig1", "unexpected title for doc1: " + title1);

    //check for second document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig2");
    GTThread::waitForMainThread();
    GTGlobals::sleep();
    title2 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title2 == "BL060C3 [m] Contig2", "unexpected title for doc2: " + title2);
}

GUI_TEST_CLASS_DEFINITION(test_0038_1) {
    //test for several assembly documents in one document
    GTUtilsDialog::waitForDialog(os, new ImportACEFileFiller(os, false, sandBoxDir + "test_3637_1.ugenedb"));
    GTFileDialog::openFile(os, dataDir + "samples/ACE/BL060C3.ace");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //check for first document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig1");
    QString title1 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title1 == "test_3637_1 [as] Contig1", "unexpected title for doc1: " + title1);

    //check for first document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig2");
    QString title2 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title2 == "test_3637_1 [as] Contig2", "unexpected title for doc2: " + title2);

    //reopening windows
    while (GTUtilsMdi::activeWindow(os, GTGlobals::FindOptions(false)) != NULL) {
        GTUtilsMdi::closeActiveWindow(os);
    }
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Open View"
                                                                        << "action_open_view"));
    GTUtilsProjectTreeView::click(os, "test_3637_1.ugenedb", Qt::RightButton);

    //check for first document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig1");
    GTGlobals::sleep();
    title1 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title1 == "test_3637_1 [as] Contig1", "unexpected title for doc1: " + title1);

    //check for second document
    GTUtilsProjectTreeView::doubleClickItem(os, "Contig2");
    GTGlobals::sleep();
    title2 = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title2 == "test_3637_1 [as] Contig2", "unexpected title for doc2: " + title2);
}

GUI_TEST_CLASS_DEFINITION(test_0039) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1 (UCS\r\nACGT\r\nACG");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCS");
}

QString readFileToStr(const QString &path) {
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
    //check adding document with 2 sequences in separate mode
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsProjectTreeView::findIndex(os, "human_T1");
    GTUtilsProjectTreeView::findIndex(os, "human_T2");
}

GUI_TEST_CLASS_DEFINITION(test_0041) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check shift+insert instead Ctrl+V
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1 (UCS\nACGT\nACG");

    GTKeyboardDriver::keyClick(Qt::Key_Insert, Qt::ShiftModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCS");
}

GUI_TEST_CLASS_DEFINITION(test_0042) {
    //check adding schemes (WD QD) in project, it should not appear in project view

    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "workflow_samples/Alignment/basic_align.uwl");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new StartupDialogFiller(os));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsMdi::activeWindowTitle(os).contains("Workflow Designer"), "Mdi window is not a WD window");
}

GUI_TEST_CLASS_DEFINITION(test_0043) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check newick format because there was crash
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Newick/COI.nwk");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "Tree");
}

GUI_TEST_CLASS_DEFINITION(test_0045) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check document which format cant be saved by UGENE
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Stockholm/CBS.sto");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "CBS");

    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.sto"), false);
}

GUI_TEST_CLASS_DEFINITION(test_0046) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check document which format can't be saved by UGENE has no locked state
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/PDB/1CF7.PDB");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "1CF7");

    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.pdb"), false);
    GTUtilsStartPage::openStartPage(os);
    HIWebElement recentDocElement = GTWebView::findElement(os, GTUtilsStartPage::getStartPage(os), "- clipboard.pdb", "A");
    QString elem = recentDocElement.toPlainText();
    CHECK_SET_ERR(elem == "- clipboard.pdb", "Recent doc not found");
}

GUI_TEST_CLASS_DEFINITION(test_0047) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check document which format can't be saved by UGENE has no locked state
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.fa"), true);
}

GUI_TEST_CLASS_DEFINITION(test_0048) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //pasting same data 10 times
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    for (int i = 0; i < 10; i++) {
        GTUtilsProjectTreeView::click(os, "COI.aln");
        GTClipboard::setText(os, QString(">human_T%1\r\nACGT\r\nACG").arg(QString::number(i)));
        GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
        GTGlobals::sleep();
        uiLog.trace(QString("item number %1 inserted").arg(i));
    }

    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndexList list = GTUtilsProjectTreeView::findIndecies(os, "");
    uiLog.trace("All items in project tree view:");
    foreach (QModelIndex index, list) {
        uiLog.trace(index.data().toString());
    }

    for (int i = 0; i < 10; i++) {
        GTUtilsProjectTreeView::findIndex(os, QString("human_T%1").arg(QString::number(i)));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0049) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check no crash after closing project without saving
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTUtilsProject::closeProject(os);
}

GUI_TEST_CLASS_DEFINITION(test_0050) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //'usual' scenario
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/FASTA/human_T1.fa");
    GTClipboard::setText(os, fileContent);

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTGlobals::sleep(500);

    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");

    GTUtilsTaskTreeView::waitTaskFinished(os);
    QFile savedFile(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/clipboard.fa");
    CHECK_SET_ERR(savedFile.exists(), "Saved file didn't exists");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0051) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check adding document with 2 sequences in align mode
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();

    GTUtilsProjectTreeView::findIndex(os, "Multiple alignment");
    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.fa"), true);
}

GUI_TEST_CLASS_DEFINITION(test_0052) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check adding document with 2 sequences in merge mode
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Merge));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::findIndex(os, "Sequence");
    GTUtilsProjectTreeView::findIndex(os, "Contigs");
    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.fa"), false);
}

GUI_TEST_CLASS_DEFINITION(test_0053) {
    GTFile::removeDir(AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath());
    //check adding document with 2 sequences in separate mode, with file which cannot be written by UGENE
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/multy_fa.fa");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsProjectTreeView::itemModificationCheck(os, GTUtilsProjectTreeView::findIndex(os, "clipboard.fa"), true);
}

GUI_TEST_CLASS_DEFINITION(test_0054) {
    //check adding document with 2 sequences in separate mode, with file which cannot be written by UGENE
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/genbank/multi.gb");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate, 10, true));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0055) {
    //check document format dialog cancelling
    class CustomScenarioCancel : public CustomScenario {
    public:
        CustomScenarioCancel() {
        }
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/broken/broken_doc.fa");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, new CustomScenarioCancel()));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0056) {
    //check opening brocken fasta document as text

    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(testDir + "_common_data/fasta/broken/broken_doc.fa");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "Plain text"));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0057) {
    //check adding document with 2 sequences in short reads mode
    class CheckPathScenario : public CustomScenario {
    public:
        CheckPathScenario() {
        }
        virtual void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QTreeWidget *treeWidget = qobject_cast<QTreeWidget *>(GTWidget::findWidget(os, "shortReadsTable", dialog));
            CHECK_SET_ERR(treeWidget != NULL, "Tree widget is NULL");
            QList<QTreeWidgetItem *> treeItems = GTTreeWidget::getItems(treeWidget->invisibleRootItem());
            QTreeWidgetItem *firstItem = treeItems.first();
            QString path = firstItem->text(0);
            CHECK_SET_ERR(!path.isEmpty(), "Reads filepath should not be empty");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsProjectTreeView::click(os, "COI.aln");
    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n>human_T2\r\nACCTGA");

    GTUtilsDialog::waitForDialog(os, new AlignShortReadsFiller(os, new CheckPathScenario()));
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Align));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_0058) {
    //1. Open a project.
    GTUtilsProject::openFiles(os, dataDir + "samples/CLUSTALW/COI.aln");

    //2. Paste the content of the file "samples/Assembly/chrM.sam".
    GTUtilsProjectTreeView::click(os, "COI.aln");
    QString fileContent = readFileToStr(dataDir + "samples/Assembly/chrM.sam");
    GTClipboard::setText(os, fileContent);

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "project_test_0058/project_test_0058.ugenedb", "", "", true));
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected: there are no dialog of file format choosing.
}

GUI_TEST_CLASS_DEFINITION(test_0059) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTGlobals::sleep();
    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

GUI_TEST_CLASS_DEFINITION(test_0060) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTGlobals::sleep();
    GTClipboard::setText(os, "ACGT");
    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "clipboard", GTGlobals::FindOptions(true, Qt::MatchContains));
}

GUI_TEST_CLASS_DEFINITION(test_0061) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0062) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa" << dataDir + "samples/HMM/aligment15900.hmm");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    GTUtilsProjectTreeView::findIndex(os, "aligment15900");
}

GUI_TEST_CLASS_DEFINITION(test_0063) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
    GTMenu::clickMainMenuItem(os, QStringList() << "File"
                                                << "Close project");
    GTGlobals::sleep();

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

GUI_TEST_CLASS_DEFINITION(test_0064) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList.last() == "human_T1 (UCSC April 2002 chr7:115977709-117855134)", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0065) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList.last() == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0066) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");
    GTUtilsMSAEditorSequenceArea::selectArea(os, QPoint(7, 3), QPoint(12, 7));

    GTClipboard::setText(os, ">human_T1\r\nACGTACG\r\n");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "MSAE_MENU_COPY"
                                                                        << "paste"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const QStringList sequencesNameList = GTUtilsMSAEditorSequenceArea::getNameList(os);
    CHECK_SET_ERR(sequencesNameList.length() > 0, "No sequences");
    CHECK_SET_ERR(sequencesNameList.last() == "human_T1", "No pasted sequences");
}

GUI_TEST_CLASS_DEFINITION(test_0067) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
}

//seq
GUI_TEST_CLASS_DEFINITION(test_0068) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 2);
    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    QAction *editMode = GTAction::findActionByText(os, "Switch on the editing mode");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    GTWidget::click(os, GTAction::button(os, editMode));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int len = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0069) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 2);
    GTClipboard::setText(os, ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    QAction *editMode = GTAction::findActionByText(os, "Switch on the editing mode");
    CHECK_SET_ERR(editMode != NULL, "Cannot find Edit mode action");
    GTWidget::click(os, GTAction::button(os, editMode));

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int len = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0070) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsSequenceView::selectSequenceRegion(os, 1, 2);
    GTClipboard::setText(os, ">human_T1\r\nACGTACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n");

    GTUtilsSequenceView::enableEditingMode(os, true);
    GTKeyboardUtils::paste(os);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    int len = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(len > 199950, "No sequences pasted");
}

GUI_TEST_CLASS_DEFINITION(test_0071) {
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/Genbank/sars.gb");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "NC_004718");
}

//ann
GUI_TEST_CLASS_DEFINITION(test_0072) {
    //Ctrl+Shift+V в GUI-test?
    //UGENE-4907
    /*
    GTUtilsProject::openFiles(os, dataDir + "samples/Genbank/murine.gb");
    //select annotations
    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/Genbank/sars.gb");
    //Ctrl+Shift+V в GUI-test?
    //GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier + Qt::Key_Shift);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    */
}

GUI_TEST_CLASS_DEFINITION(test_0073) {
    //Ctrl+Shift+V в GUI-test?
    /*
    GTUtilsProject::openFiles(os, dataDir + "samples/Genbank/murine.gb");

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/FASTA/human_T1.fa");

    GTKeyboardDriver::keyClick( 'v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)");
    */
}

GUI_TEST_CLASS_DEFINITION(test_0074) {
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");

    GTClipboard::setUrls(os, QList<QString>() << dataDir + "samples/Genbank/sars.gb");

    GTKeyboardDriver::keyClick('v', Qt::ControlModifier);
    GTGlobals::sleep();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "NC_004718");
}

}    // namespace GUITest_common_scenarios_project

}    // namespace U2
