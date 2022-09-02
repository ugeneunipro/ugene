/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GTTestsProjectSequenceExporting.h"
#include <api/GTUtils.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTTreeWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QTreeView>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>

#include <U2View/ADVConstants.h>
#include <U2View/AnnotatedDNAViewFactory.h>

#include "GTGlobals.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDocument.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateDocumentFromTextDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "system/GTFile.h"
namespace U2 {

namespace GUITest_common_scenarios_project_sequence_exporting {
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
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDocument::checkDocument(os, firstAnnFileName);
    GTUtilsDocument::checkDocument(os, secondAnnFileName);
    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    Document* doc1 = GTUtilsDocument::getDocument(os, firstAnnFileName);
    Document* doc2 = GTUtilsDocument::getDocument(os, secondAnnFileName);

    CHECK_SET_ERR(!doc1->isLoaded(), "1.gb is loaded");
    CHECK_SET_ERR(!doc2->isLoaded(), "2.gb is loaded");
    //     2) UGENE window titled with text "proj4 UGENE"
    GTMainWindow::checkTitle(os, "proj4 UGENE");

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select region 1..4 at sequence view. Right click to selected region open context menu. Use menu {Export->Export Selected Sequence region}
    // Expected state: Export DNA Sequences To FASTA Format dialog open
    // 4. Fill the next field in dialog:
    // {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    // {Add created document to project} set checked

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 4);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));

    QWidget* activeWindow = GTUtilsMdi::activeWindow(os);
    QPoint p = activeWindow->mapToGlobal(activeWindow->rect().center());
    GTMouseDriver::moveTo(QPoint(p.x(), 200));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence view [1..4] has been opened, with sequence "AAAT"
    GTUtilsDocument::checkDocument(os, "exp.fasta");

    QString seq;
    GTUtilsSequenceView::getSequenceAsString(os, seq);

    CHECK_SET_ERR(seq == "AAAT", "exported sequence differs from AAAT");
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

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened
    GTUtilsDocument::checkDocument(os, firstAnnFileName);
    GTUtilsDocument::checkDocument(os, secondAnnFileName);

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick();

    //     Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select annotation C. Use context menu item {Export->Export Sequence of Selected Annotations}
    // Expected state: Export Sequence of selected annotations will open
    //
    // 4. Fill the next field in dialog:
    //     {Format } FASTA
    //     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    //     {Add created document to project} set checked
    // 5. Click Export button.

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler = new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                     testDir + "_common_data/scenarios/sandbox/exp.fasta",
                                                                     ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                     ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate);
    GTUtilsDialog::add(os, filler);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: sequence view C has been opened, with sequence "GAATAGAAAAG"
    GTUtilsSequenceView::checkSequence(os, "GAATAGAAAAG");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
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

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened
    GTUtilsProjectTreeView::findIndex(os, firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(os, secondAnnFileName);

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();

    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select joined annotation B. Use context menu item {Export->Export Sequence of Selected Annotations}
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    GTMouseDriver::doubleClick();

    // Expected state: Export Sequence of Selected Annotations
    // 4. Fill the next field in dialog:
    //     {Format } FASTA
    //     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    //     {Add created document to project} set checked
    //     {Merge sequences} set selected
    //     {Gap length} 5
    // 5. Click Export button.
    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler = new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                     testDir + "_common_data/scenarios/sandbox/exp.fasta",
                                                                     ExportSequenceOfSelectedAnnotationsFiller::Fasta,
                                                                     ExportSequenceOfSelectedAnnotationsFiller::Merge,
                                                                     5,
                                                                     true);
    GTUtilsDialog::add(os, filler);
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: sequence view B part 1 of 3 has been opened, with sequence "ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG"
    GTUtilsSequenceView::checkSequence(os, "ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
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
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDocument::checkDocument(os, firstAnnFileName);
    GTUtilsDocument::checkDocument(os, secondAnnFileName);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    GTUtilsDialog::add(os, new PopupChooser(os, {ADV_MENU_EXPORT, ACTION_EXPORT_ANNOTATIONS}));
    GTUtilsDialog::add(os, new ExportAnnotationsFiller(os, testDir + "_common_data/scenarios/sandbox/1.csv", ExportAnnotationsFiller::csv));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool equals = GTFile::equals(os, testDir + "_common_data/scenarios/sandbox/1.csv", testDir + "_common_data/scenarios/project/test_0004.csv");
    CHECK_SET_ERR(equals == true, "Exported file differs from the test file");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
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

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDocument::checkDocument(os, firstAnnFileName);
    GTUtilsDocument::checkDocument(os, secondAnnFileName);

    QModelIndex nc_001363 = GTUtilsProjectTreeView::findIndex(os, "NC_001363 sequence", GTUtilsProjectTreeView::findIndex(os, firstAnnFileName));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, nc_001363));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDocument::checkDocument(os, firstAnnFileName, AnnotatedDNAViewFactory::ID);

    GTTreeWidget::doubleClick(os, GTUtilsAnnotationsTreeView::findItem(os, "C"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_annotations"}, GTGlobals::UseKey));

    GTUtilsDialog::add(os,
                       new ExportAnnotationsFiller(os,
                                                   testDir + "_common_data/scenarios/sandbox/1.csv",
                                                   ExportAnnotationsFiller::csv,
                                                   true,
                                                   true,
                                                   false));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    bool isEquals = GTFile::equals(os, testDir + "_common_data/scenarios/sandbox/1.csv", testDir + "_common_data/scenarios/project/test_0005.csv");
    CHECK_SET_ERR(isEquals, "Exported file differs from the test file");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    Runnable* filler = new CreateDocumentFiller(os,
                                                "ACGTGTGTGTACGACAGACGACAGCAGACGACAGACAGACAGACAGCAAGAGAGAGAGAG",
                                                true,
                                                CreateDocumentFiller::StandardRNA,
                                                true,
                                                false,
                                                "",
                                                testDir + "_common_data/scenarios/sandbox/",
                                                CreateDocumentFiller::Genbank,
                                                "Sequence",
                                                false);
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMainMenuItem(os, {"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, true, "misc_feature_group", "misc_feature", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "misc_feature"));
    GTMouseDriver::doubleClick();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler3 = new ExportSequenceOfSelectedAnnotationsFiller(os,
                                                                      testDir + "_common_data/scenarios/sandbox/exp.gb",
                                                                      ExportSequenceOfSelectedAnnotationsFiller::Genbank,
                                                                      ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
                                                                      0,
                                                                      true,
                                                                      true);
    GTUtilsDialog::add(os, filler3);
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter(os, "misc_feature"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    class CustomExportSelectedRegion : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);
            auto formatCombo = GTWidget::findComboBox(os, "formatCombo", dialog);
            auto withAnnotationsBox = GTWidget::findCheckBox(os, "withAnnotationsBox", dialog);

            CHECK_SET_ERR(!withAnnotationsBox->isEnabled(), "Export with annotations flag is enabled unexpectedly");
            CHECK_SET_ERR(!withAnnotationsBox->isChecked(), "Export with annotations flag is checked unexpectedly");

            GTComboBox::selectItemByText(os, formatCombo, "GenBank");
            CHECK_SET_ERR(withAnnotationsBox->isEnabled(), "Export with annotations flag is disabled unexpectedly");
            CHECK_SET_ERR(withAnnotationsBox->isChecked(), "Export with annotations flag is unchecked unexpectedly");

            GTComboBox::selectItemByText(os, formatCombo, "FASTQ");
            CHECK_SET_ERR(!withAnnotationsBox->isEnabled(), "Export with annotations flag is enabled unexpectedly");
            CHECK_SET_ERR(!withAnnotationsBox->isChecked(), "Export with annotations flag is checked unexpectedly");

            GTComboBox::selectItemByText(os, formatCombo, "GFF");
            CHECK_SET_ERR(withAnnotationsBox->isEnabled(), "Export with annotations flag is disabled unexpectedly");
            CHECK_SET_ERR(withAnnotationsBox->isChecked(), "Export with annotations flag is unchecked unexpectedly");

            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, new CustomExportSelectedRegion()));
    GTUtilsProjectTreeView::click(os, "NC_001363", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // the test checks that a sequence associated with an annotation table can be exported
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, sandBoxDir, "Project_export_test_0009.fa"));
    GTUtilsProjectTreeView::click(os, "NC_001363 features", Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex(os, "Project_export_test_0009.fa");
    GTUtilsProjectTreeView::findIndex(os, "NC_001363", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // negative test for an annotation table not associated with any sequence
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTFileDialog::openFile(os, dataDir + "samples/GFF/", "5prime_utr_intron_A21.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsProjectTreeView::click(os, "Ca21chr5 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // the rest part of the test checks that a newly created association can be used for sequence export

    QModelIndex idxGff = GTUtilsProjectTreeView::findIndex(os, "Ca21chr5 features");
    auto seqArea = GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::add(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, "Yes"));
    GTUtilsProjectTreeView::dragAndDrop(os, idxGff, seqArea);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, sandBoxDir, "Project_export_test_0010.fa"));
    GTUtilsProjectTreeView::click(os, "Ca21chr5 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex(os, "Project_export_test_0010.fa");
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // negative test for annotation table associated with a removed sequence

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "NC_001363");
    GTKeyboardDriver::keyPress(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsProjectTreeView::click(os, "NC_001363 features", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // test for an annotation table whose sequence association was changed
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    QModelIndex annIdx = GTUtilsProjectTreeView::findIndex(os, "NC_001363 features");
    auto seqArea = GTWidget::findWidget(os, "render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    GTUtilsProjectTreeView::dragAndDrop(os, annIdx, seqArea);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::add(os, new PopupChooser(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(os, new ExportSelectedRegionFiller(os, sandBoxDir, "Project_export_test_0012.fa"));
    GTUtilsProjectTreeView::click(os, "NC_001363 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex(os, "Project_export_test_0012.fa");
    GTUtilsProjectTreeView::findIndex(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // test for multiple annotation object selection associated sequence import is not available
    GTFileDialog::openFile(os, dataDir + "samples/GFF/", "5prime_utr_intron_A21.gff");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click(os, "Ca21chr5 features");
    GTUtilsProjectTreeView::click(os, "Ca21chr1 features");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled(os);

    GTUtilsDialog::waitForDialog(os, new PopupChecker(os, {ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}, PopupChecker::NotExists));
    GTMouseDriver::click(Qt::RightButton);
}

}  // namespace GUITest_common_scenarios_project_sequence_exporting

}  // namespace U2
