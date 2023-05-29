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
#include <api/GTUtils.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include <system/GTFile.h>
#include <utils/GTThread.h>

#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTTestsFromProjectView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/ExportChromatogramFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportMSA2SequencesDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequences2MSADialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"
#include "utils/GTKeyboardUtils.h"

namespace U2 {

namespace GUITest_common_scenarios_project_sequence_exporting_from_project_view {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_0001) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);

    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsDocument::checkDocument(os, firstAnnFileName, GTUtilsDocument::DocumentUnloaded);
    GTUtilsDocument::checkDocument(os, secondAnnFileName, GTUtilsDocument::DocumentUnloaded);

    // 2. Double-click on [a] Annotations sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    // 3. Right click on [s] NC_001363 sequence object, in project view tree. Use context menu item {Export->Export sequences}
    // Expected state: Export sequences dialog open
    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}, GTGlobals::UseMouse));

    // 4. Fill the next field in dialog:
    //     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    //     {File format to use:} FASTA
    //     {Add created document to project} set checked
    //
    // 5. Click Export button.
    GTUtilsDialog::add(os,
                       new ExportSequenceOfSelectedAnnotationsFiller(
                           os,
                           testDir + "_common_data/scenarios/sandbox/exp.fasta",
                           ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                           ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate));

    QModelIndex parent = GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);
    QModelIndex child = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, child));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence view NC_001363 sequence has been opened, with sequence same as in 1.gb document
    GTUtilsDocument::checkDocument(os, "exp.fasta", AnnotatedDNAViewFactory::ID);
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);

    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsDocument::checkDocument(os, firstAnnFileName, GTUtilsDocument::DocumentUnloaded);
    GTUtilsDocument::checkDocument(os, secondAnnFileName, GTUtilsDocument::DocumentUnloaded);

    // 2. Double-click on the first annotations file, in project view tree.
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Right click on [s] NC_001363 sequence object, in project view tree. Use context menu item {Export->Export sequence as alignment}
    // Expected state: Export sequences dialog open
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}, GTGlobals::UseMouse));

    // 4. Select file to save: _common_data/scenarios/sandbox/exp2.aln and set 'file format to use' to CLUSTALW,
    // Next to uncheck the 'add document to the project' checkbox and click Save button.
    Runnable* filler = new ExportSequenceAsAlignmentFiller(os,
                                                           testDir + "_common_data/scenarios/sandbox/",
                                                           "exp2.aln",
                                                           ExportSequenceAsAlignmentFiller::Clustalw,
                                                           false);
    GTUtilsDialog::add(os, filler);
    GTMouseDriver::click(Qt::RightButton);

    // 5. Open file _common_data/scenarios/sandbox/exp2.aln
    // Expected state: multiple alignment view with NC_001363 sequence has been opened
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/sandbox/exp2.aln");
    GTUtilsDocument::checkDocument(os, "exp2.aln");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os));
    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/project/multiple.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "multiple.fa"));
    GTMouseDriver::click();

    GTKeyboardUtils::selectAll();

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}, GTGlobals::UseMouse));

    Runnable* filler = new ExportSequenceAsAlignmentFiller(os,
                                                           testDir + "_common_data/scenarios/sandbox/",
                                                           "exp2.aln",
                                                           ExportSequenceAsAlignmentFiller::Clustalw,
                                                           GTGlobals::UseMouse);
    GTUtilsDialog::add(os, filler);
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsProject::openFile(os, testDir + "_common_data/scenarios/sandbox/exp2.aln");
    GTUtilsDocument::checkDocument(os, "exp2.aln");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/", "HIV-1.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_AS_SEQUENCES_ACTION}));
    GTUtilsDialog::add(os, new ExportToSequenceFormatFiller(os, dataDir + " _common_data/scenarios/sandbox/", "export1.fa", ExportToSequenceFormatFiller::FASTA, true, true));
    GTUtilsProjectTreeView::click(os, "HIV-1.aln", Qt::RightButton);

    GTUtilsProjectTreeView::getItemCenter(os, "export1.fa");
    GTUtilsProjectTreeView::scrollTo(os, "ru131");
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);

    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "ru131");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();

    GTUtilsMdi::checkWindowIsActive(os, "ru131");

    QString sequenceEnd = GTUtilsSequenceView::getEndOfSequenceAsString(os, 1);
    CHECK_SET_ERR(sequenceEnd.at(0) == '-', "Sequence has no '-' symbol as a suffix");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // 1. Use menu {File->Open}. Open project data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    // 3. Right click [m] COI object, in project view tree. Use context menu item {Export->Export to FASTA}
    //    Expected state: Export alignment dialog open

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_AS_SEQUENCES_ACTION}));
    GTUtilsDialog::add(os, new ExportToSequenceFormatFiller(os, dataDir + " _common_data/scenarios/sandbox/", "export1.fa", ExportToSequenceFormatFiller::FASTA, true, false));
    GTUtilsProjectTreeView::click(os, "COI.aln", Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"openInMenu", "action_open_view"}, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "Zychia_baranovi", Qt::RightButton);
    GTThread::waitForMainThread();

    QWidget* activeWindow = GTUtilsMdi::activeWindow(os);
    CHECK_SET_ERR(activeWindow->windowTitle().contains("Zychia_baranovi"), "fasta file with sequences has been not opened");

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 586, "Sequence length is " + QString::number(sequenceLength) + ", expected 586");
}

GUI_TEST_CLASS_DEFINITION(test_0005_1) {
    // 1. Use menu {File->Open}. Open project data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    // 3. Right click [m] COI object, in project view tree. Use context menu item {Export->Export to FASTA}
    //    Expected state: Export alignment dialog open

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_AS_SEQUENCES_ACTION}));
    GTUtilsDialog::add(os,
                       new ExportToSequenceFormatFiller(os,
                                                        dataDir + " _common_data/scenarios/sandbox/",
                                                        "export1.fa",
                                                        ExportToSequenceFormatFiller::FASTA,
                                                        true,
                                                        true));

    GTUtilsProjectTreeView::click(os, "COI.aln", Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"openInMenu", "action_open_view"}, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "Zychia_baranovi", Qt::RightButton);

    QWidget* activeWindow = GTUtilsMdi::activeWindow(os);
    if (!activeWindow->windowTitle().contains("Zychia_baranovi") && !os.hasError()) {
        os.setError("fasta file with sequences has been not opened");
    }

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 604, "Sequence length is " + QString::number(sequenceLength) + ", expected 586");
}

GUI_TEST_CLASS_DEFINITION(test_0005_2) {
    // 1. Use menu {File->Open}. Open project data/samples/CLUSTALW/COI.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtils::checkExportServiceIsEnabled(os);

    // 3. Right click [m] COI object, in project view tree. Use context menu item {Export->Export to FASTA}
    //    Expected state: Export alignment dialog open

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_PROJECT__EXPORT_AS_SEQUENCES_ACTION}));
    GTUtilsDialog::add(os, new ExportToSequenceFormatFiller(os, dataDir + " _common_data/scenarios/sandbox/", "export1.fa", ExportToSequenceFormatFiller::FASTA, true, false));

    GTUtilsProjectTreeView::click(os, "COI.aln", Qt::RightButton);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"openInMenu", "action_open_view"}, GTGlobals::UseMouse));
    GTUtilsProjectTreeView::click(os, "Zychia_baranovi", Qt::RightButton);

    QWidget* activeWindow = GTUtilsMdi::activeWindow(os);
    if (!activeWindow->windowTitle().contains("Zychia_baranovi") && !os.hasError()) {
        os.setError("fasta file with sequences has been not opened");
    }

    int sequenceLength = GTUtilsSequenceView::getLengthOfSequence(os);
    CHECK_SET_ERR(sequenceLength == 586, "Sequence length is " + QString::number(sequenceLength) + ", expected 586");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state: project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsProjectTreeView::click(os, firstAnnFileName);
    GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(os, secondAnnFileName);
    CHECK_SET_ERR(!GTUtilsDocument::isDocumentLoaded(os, firstAnnFileName), "Documents is not unloaded: " + firstAnnFileName);
    CHECK_SET_ERR(!GTUtilsDocument::isDocumentLoaded(os, secondAnnFileName), "Documents is not unloaded: " + secondAnnFileName);

    // Double-click on [a] Annotations sequence object, in project view tree
    GTUtilsProjectTreeView::click(os, "Annotations");
    GTMouseDriver::doubleClick();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsMdi::checkWindowIsActive(os, "NC_001363");

    // Right click on [s] NC_001363 sequence object, in project view tree.
    // Use context menu item {Export->Export sequence as alignment}
    // Expected state: Export sequences dialog open
    // Select file to save: _common_data/scenarios/sandbox/exp2.msf and set 'file format to use' to MSF,
    // Next to uncheck the 'add document to the project' checkbox and click Save button.
    GTUtils::checkExportServiceIsEnabled(os);
    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));

    GTUtilsDialog::add(os, new ExportSequenceAsAlignmentFiller(os, sandBoxDir, "exp2.msf", ExportSequenceAsAlignmentFiller::Msf));
    QModelIndex parent = GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);
    QModelIndex child = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, child));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open file _common_data/scenarios/sandbox/exp2.msf
    GTFileDialog::openFile(os, sandBoxDir, "exp2.msf");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: multiple alignment view with NC_001363 sequence has been opened.
    CHECK_SET_ERR(GTUtilsProjectTreeView::getSelectedItem(os) == "[s] NC_001363 sequence", "Multiple alignment view with NC_001363 sequence has been not opened");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);

    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(os, secondAnnFileName);
    if (GTUtilsDocument::isDocumentLoaded(os, firstAnnFileName) || GTUtilsDocument::isDocumentLoaded(os, secondAnnFileName)) {
        os.setError("Documents is not unload");
        return;
    }

    // 2. Double-click on [a] Annotations sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();
    GTGlobals::sleep();

    // Expected result: NC_001363 sequence has been opened in sequence view
    auto activeWindow = qobject_cast<GObjectViewWindow*>(GTUtilsMdi::activeWindow(os));
    if (!activeWindow->getViewName().contains("NC_001363")) {
        os.setError("NC_001363 sequence has been not opened in sequence view");
        return;
    }

    // 3. Right click on [s] NC_001363 sequence object, in project view tree. Use context menu item {Export->Export sequence as alignment}
    // Expected state: Export sequences dialog open
    // 4. Select file to save: _common_data/scenarios/sandbox/exp2.msf and set 'file format to use' to MSF,
    // Next to uncheck the 'add document to the project' checkbox and click Save button.
    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(os, new ExportSequenceAsAlignmentFiller(os, sandBoxDir, "exp2.sto", ExportSequenceAsAlignmentFiller::Stockholm));

    QModelIndex parent = GTUtilsProjectTreeView::findIndex(os, "1.gb");
    QModelIndex child = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, child));
    GTMouseDriver::click(Qt::RightButton);

    // 5. Open file _common_data/scenarios/sandbox/exp2.msf
    GTGlobals::sleep();
    GTFileDialog::openFile(os, sandBoxDir, "exp2.sto");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(1000);

    // Expected state: multiple alignment view with NC_001363 sequence has been opened
    if (GTUtilsProjectTreeView::getSelectedItem(os) != "[s] NC_001363 sequence") {
        os.setError("multiple alignment view with NC_001363 sequence has been not opened");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0007_1) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(os, secondAnnFileName);
    if (GTUtilsDocument::isDocumentLoaded(os, firstAnnFileName) || GTUtilsDocument::isDocumentLoaded(os, secondAnnFileName)) {
        os.setError("Documents is not unload");
        return;
    }

    // 2. Double-click on [a] Annotations sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected result: NC_001363 sequence has been opened in sequence view
    auto activeWindow = qobject_cast<GObjectViewWindow*>(GTUtilsMdi::activeWindow(os));
    if (!activeWindow->getViewName().contains("NC_001363")) {
        os.setError("NC_001363 sequence has been not opened in sequence view");
        return;
    }

    // 3. Right click on [s] NC_001363 sequence object, in project view tree. Use context menu item {Export->Export sequence as alignment}
    // Expected state: Export sequences dialog open
    // 4. Select file to save: _common_data/scenarios/sandbox/exp2.msf and set 'file format to use' to MSF,
    // Next to uncheck the 'add document to the project' check box and click Save button.
    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));
    GTUtilsDialog::add(os, new ExportSequenceAsAlignmentFiller(os, sandBoxDir, "exp2.fa", ExportSequenceAsAlignmentFiller::Fasta));

    QModelIndex parent = GTUtilsProjectTreeView::findIndex(os, "1.gb");
    QModelIndex child = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, child));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 5. Open file _common_data/scenarios/sandbox/exp2.msf
    GTFileDialog::openFile(os, sandBoxDir, "exp2.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: multiple alignment view with NC_001363 sequence has been opened
    if (GTUtilsProjectTreeView::getSelectedItem(os) != "[s] NC_001363 sequence") {
        os.setError("multiple alignment view with NC_001363 sequence has been not opened");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0007_2) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileNAme = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(os, projectFilePath, sandBoxDir + "/" + projectFileNAme);
    GTFile::copy(os, firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(os, secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);

    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, sandBoxDir, projectFileNAme);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(os, secondAnnFileName);
    if (GTUtilsDocument::isDocumentLoaded(os, firstAnnFileName) || GTUtilsDocument::isDocumentLoaded(os, secondAnnFileName)) {
        os.setError("Documents is not unload");
        return;
    }

    // 2. Double-click on [a] Annotations sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected result: NC_001363 sequence has been opened in sequence view
    auto activeWindow = qobject_cast<GObjectViewWindow*>(GTUtilsMdi::activeWindow(os));
    if (!activeWindow->getViewName().contains("NC_001363")) {
        os.setError("NC_001363 sequence has been not opened in sequence view");
        return;
    }

    // 3. Right click on [s] NC_001363 sequence object, in project view tree. Use context menu item {Export->Export sequence as alignment}
    // Expected state: Export sequences dialog open
    // 4. Select file to save: _common_data/scenarios/sandbox/exp2.msf and set 'file format to use' to MSF,
    // Next to uncheck the 'add document to the project' check box and click Save button.
    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE_AS_ALIGNMENT}));

    GTUtilsDialog::add(os, new ExportSequenceAsAlignmentFiller(os, sandBoxDir, "exp2.meg", ExportSequenceAsAlignmentFiller::Mega));

    QModelIndex parent = GTUtilsProjectTreeView::findIndex(os, "1.gb");
    QModelIndex child = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", parent);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, child));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 5. Open file _common_data/scenarios/sandbox/exp2.msf
    GTFileDialog::openFile(os, sandBoxDir, "exp2.meg");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: multiple alignment view with NC_001363 sequence has been opened
    if (GTUtilsProjectTreeView::getSelectedItem(os) != "[s] NC_001363 sequence") {
        os.setError("multiple alignment view with NC_001363 sequence has been not opened");
    }
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CHROMATOGRAM}));
    GTUtilsDialog::add(os, new ExportChromatogramFiller(os, sandBoxDir, "pagefile.sys", ExportChromatogramFiller::SCF, false, true, true));
    GTUtilsProjectTreeView::click(os, "A01.abi", Qt::RightButton);
}
GUI_TEST_CLASS_DEFINITION(test_0008_1) {
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CHROMATOGRAM}));
    GTUtilsDialog::add(os, new ExportChromatogramFiller(os, sandBoxDir, "pagefile.sys", ExportChromatogramFiller::SCF, true, true, true));
    GTUtilsProjectTreeView::click(os, "A01.abi", Qt::RightButton);
}
GUI_TEST_CLASS_DEFINITION(test_0008_2) {
    GTFileDialog::openFile(os, dataDir + "samples/ABIF/", "A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CHROMATOGRAM}));
    GTUtilsDialog::add(os, new ExportChromatogramFiller(os, sandBoxDir, "pagefile.sys", ExportChromatogramFiller::SCF, false, true, false));
    GTUtilsProjectTreeView::click(os, "A01.abi", Qt::RightButton);
}

}  // namespace GUITest_common_scenarios_project_sequence_exporting_from_project_view
}  // namespace U2
