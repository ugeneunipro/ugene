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

    GTFile::copy(projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(sandBoxDir, projectFileName);
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDocument::checkDocument(firstAnnFileName);
    GTUtilsDocument::checkDocument(secondAnnFileName);
    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded
    Document* doc1 = GTUtilsDocument::getDocument(firstAnnFileName);
    Document* doc2 = GTUtilsDocument::getDocument(secondAnnFileName);

    CHECK_SET_ERR(!doc1->isLoaded(), "1.gb is loaded");
    CHECK_SET_ERR(!doc2->isLoaded(), "2.gb is loaded");
    //     2) UGENE window titled with text "proj4 UGENE"
    GTMainWindow::checkTitle("proj4 UGENE");

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTMouseDriver::doubleClick();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select region 1..4 at sequence view. Right click to selected region open context menu. Use menu {Export->Export Selected Sequence region}
    // Expected state: Export DNA Sequences To FASTA Format dialog open
    // 4. Fill the next field in dialog:
    // {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    // {Add created document to project} set checked

    GTUtilsSequenceView::selectSequenceRegion(1, 4);

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_selected_sequence_region"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(testDir + "_common_data/scenarios/sandbox/", "exp.fasta"));

    QWidget* activeWindow = GTUtilsMdi::activeWindow();
    QPoint p = activeWindow->mapToGlobal(activeWindow->rect().center());
    GTMouseDriver::moveTo(QPoint(p.x(), 200));
    GTMouseDriver::click(Qt::RightButton);

    // Expected state: sequence view [1..4] has been opened, with sequence "AAAT"
    GTUtilsDocument::checkDocument("exp.fasta");

    QString seq;
    GTUtilsSequenceView::getSequenceAsString(seq);

    CHECK_SET_ERR(seq == "AAAT", "exported sequence differs from AAAT");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened
    GTUtilsDocument::checkDocument(firstAnnFileName);
    GTUtilsDocument::checkDocument(secondAnnFileName);

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTMouseDriver::doubleClick();

    //     Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select annotation C. Use context menu item {Export->Export Sequence of Selected Annotations}
    // Expected state: Export Sequence of selected annotations will open
    //
    // 4. Fill the next field in dialog:
    //     {Format } FASTA
    //     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    //     {Add created document to project} set checked
    // 5. Click Export button.

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler = new ExportSequenceOfSelectedAnnotationsFiller(
        testDir + "_common_data/scenarios/sandbox/exp.fasta",
        ExportSequenceOfSelectedAnnotationsFiller::Fasta,
        ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate);
    GTUtilsDialog::add(filler);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("C"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: sequence view C has been opened, with sequence "GAATAGAAAAG"
    GTUtilsSequenceView::checkSequence("GAATAGAAAAG");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    // Expected state:
    //     1) Project view with document "1.gb" and "2.gb" is opened
    GTUtilsProjectTreeView::findIndex(firstAnnFileName);  // checks inside
    GTUtilsProjectTreeView::findIndex(secondAnnFileName);

    // 2. Double-click on "[a] Annotations" sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter("Annotations");
    GTMouseDriver::moveTo(itemPos);
    GTMouseDriver::doubleClick();

    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(firstAnnFileName, AnnotatedDNAViewFactory::ID);

    // 3. Select joined annotation B. Use context menu item {Export->Export Sequence of Selected Annotations}
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B_joined"));
    GTMouseDriver::doubleClick();

    // Expected state: Export Sequence of Selected Annotations
    // 4. Fill the next field in dialog:
    //     {Format } FASTA
    //     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
    //     {Add created document to project} set checked
    //     {Merge sequences} set selected
    //     {Gap length} 5
    // 5. Click Export button.
    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler = new ExportSequenceOfSelectedAnnotationsFiller(
        testDir + "_common_data/scenarios/sandbox/exp.fasta",
        ExportSequenceOfSelectedAnnotationsFiller::Fasta,
        ExportSequenceOfSelectedAnnotationsFiller::Merge,
        5,
        true);
    GTUtilsDialog::add(filler);
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B_joined"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected state: sequence view B part 1 of 3 has been opened, with sequence "ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG"
    GTUtilsSequenceView::checkSequence("ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(sandBoxDir, projectFileName);
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDocument::checkDocument(firstAnnFileName);
    GTUtilsDocument::checkDocument(secondAnnFileName);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("Annotations"));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDocument::checkDocument(firstAnnFileName, AnnotatedDNAViewFactory::ID);

    GTUtilsDialog::add(new PopupChooser({ADV_MENU_EXPORT, ACTION_EXPORT_ANNOTATIONS}));
    GTUtilsDialog::add(new ExportAnnotationsFiller(testDir + "_common_data/scenarios/sandbox/1.csv", ExportAnnotationsFiller::csv, false));
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("B_joined"));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    bool isEquals = GTFile::equals(testDir + "_common_data/scenarios/sandbox/1.csv", testDir + "_common_data/scenarios/project/test_0004.csv");
    CHECK_SET_ERR(isEquals, "Exported file differs from the test file");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    QString projectFilePath = testDir + "_common_data/scenarios/project/proj4.uprj";
    QString projectFileName = "proj4.uprj";
    QString firstAnnFilePath = testDir + "_common_data/scenarios/project/1.gb";
    QString firstAnnFileName = "1.gb";
    QString secondAnnFilePath = testDir + "_common_data/scenarios/project/2.gb";
    QString secondAnnFileName = "2.gb";

    GTFile::copy(projectFilePath, sandBoxDir + "/" + projectFileName);
    GTFile::copy(firstAnnFilePath, sandBoxDir + "/" + firstAnnFileName);
    GTFile::copy(secondAnnFilePath, sandBoxDir + "/" + secondAnnFileName);
    // 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(sandBoxDir, projectFileName);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDocument::checkDocument(firstAnnFileName);
    GTUtilsDocument::checkDocument(secondAnnFileName);

    QModelIndex nc_001363 = GTUtilsProjectTreeView::findIndex("NC_001363 sequence", GTUtilsProjectTreeView::findIndex(firstAnnFileName));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(nc_001363));
    GTMouseDriver::doubleClick();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDocument::checkDocument(firstAnnFileName, AnnotatedDNAViewFactory::ID);

    GTTreeWidget::doubleClick(GTUtilsAnnotationsTreeView::findItem("C"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_annotations"}, GTGlobals::UseKey));

    GTUtilsDialog::add(
        new ExportAnnotationsFiller(
            testDir + "_common_data/scenarios/sandbox/1.csv",
            ExportAnnotationsFiller::csv,
            true,
            true,
            false));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    bool isEquals = GTFile::equals(testDir + "_common_data/scenarios/sandbox/1.csv", testDir + "_common_data/scenarios/project/test_0005.csv");
    CHECK_SET_ERR(isEquals, "Exported file differs from the test file");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    Runnable* filler = new CreateDocumentFiller(
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
    GTUtilsDialog::waitForDialog(filler);
    GTMenu::clickMainMenuItem({"File", "New document from text..."}, GTGlobals::UseKey);

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(true, "misc_feature_group", "misc_feature", "complement(1.. 20)"));
    GTKeyboardDriver::keyClick('n', Qt::ControlModifier);

    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("misc_feature"));
    GTMouseDriver::doubleClick();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::add(new PopupChooser({"ADV_MENU_EXPORT", "action_export_sequence_of_selected_annotations"}, GTGlobals::UseKey));
    Runnable* filler3 = new ExportSequenceOfSelectedAnnotationsFiller(
        testDir + "_common_data/scenarios/sandbox/exp.gb",
        ExportSequenceOfSelectedAnnotationsFiller::Genbank,
        ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
        0,
        true,
        true);
    GTUtilsDialog::add(filler3);
    GTMouseDriver::moveTo(GTUtilsAnnotationsTreeView::getItemCenter("misc_feature"));
    GTMouseDriver::click(Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    class CustomExportSelectedRegion : public CustomScenario {
    public:
        void run() override {
            QWidget* dialog = GTWidget::getActiveModalWidget();
            auto formatCombo = GTWidget::findComboBox("formatCombo", dialog);
            auto withAnnotationsBox = GTWidget::findCheckBox("withAnnotationsBox", dialog);

            CHECK_SET_ERR(!withAnnotationsBox->isEnabled(), "Export with annotations flag is enabled unexpectedly");
            CHECK_SET_ERR(!withAnnotationsBox->isChecked(), "Export with annotations flag is checked unexpectedly");

            GTComboBox::selectItemByText(formatCombo, "GenBank");
            CHECK_SET_ERR(withAnnotationsBox->isEnabled(), "Export with annotations flag is disabled unexpectedly");
            CHECK_SET_ERR(withAnnotationsBox->isChecked(), "Export with annotations flag is unchecked unexpectedly");

            GTComboBox::selectItemByText(formatCombo, "FASTQ");
            CHECK_SET_ERR(!withAnnotationsBox->isEnabled(), "Export with annotations flag is enabled unexpectedly");
            CHECK_SET_ERR(!withAnnotationsBox->isChecked(), "Export with annotations flag is checked unexpectedly");

            GTComboBox::selectItemByText(formatCombo, "GFF");
            CHECK_SET_ERR(withAnnotationsBox->isEnabled(), "Export with annotations flag is disabled unexpectedly");
            CHECK_SET_ERR(withAnnotationsBox->isChecked(), "Export with annotations flag is unchecked unexpectedly");

            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        }
    };

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_SEQUENCE}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(new CustomExportSelectedRegion()));
    GTUtilsProjectTreeView::click("NC_001363", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // the test checks that a sequence associated with an annotation table can be exported
    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(sandBoxDir, "Project_export_test_0009.fa"));
    GTUtilsProjectTreeView::click("NC_001363 features", Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex("Project_export_test_0009.fa");
    GTUtilsProjectTreeView::findIndex("NC_001363", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // negative test for an annotation table not associated with any sequence
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    GTFileDialog::openFile(dataDir + "samples/GFF/", "5prime_utr_intron_A21.gff");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsProjectTreeView::click("Ca21chr5 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    // the rest part of the test checks that a newly created association can be used for sequence export

    QModelIndex idxGff = GTUtilsProjectTreeView::findIndex("Ca21chr5 features");
    auto seqArea = GTWidget::findWidget("render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::add(new CreateObjectRelationDialogFiller());
    GTUtilsDialog::add(new MessageBoxDialogFiller("Yes"));
    GTUtilsProjectTreeView::dragAndDrop(idxGff, seqArea);
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(sandBoxDir, "Project_export_test_0010.fa"));
    GTUtilsProjectTreeView::click("Ca21chr5 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex("Project_export_test_0010.fa");
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // negative test for annotation table associated with a removed sequence

    GTFileDialog::openFile(dataDir + "samples/Genbank", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsProjectTreeView::click("NC_001363");
    GTKeyboardDriver::keyPress(Qt::Key_Delete);

    GTUtilsTaskTreeView::waitTaskFinished();

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Ok));
    GTUtilsProjectTreeView::click("NC_001363 features", Qt::RightButton);
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // test for an annotation table whose sequence association was changed
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::checkProjectViewIsOpened();

    QModelIndex annIdx = GTUtilsProjectTreeView::findIndex("NC_001363 features");
    auto seqArea = GTWidget::findWidget("render_area_human_T1 (UCSC April 2002 chr7:115977709-117855134)");

    GTUtilsDialog::waitForDialog(new CreateObjectRelationDialogFiller());
    GTUtilsProjectTreeView::dragAndDrop(annIdx, seqArea);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::add(new PopupChooser({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}));
    GTUtilsDialog::add(new ExportSelectedRegionFiller(sandBoxDir, "Project_export_test_0012.fa"));
    GTUtilsProjectTreeView::click("NC_001363 features", Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished();

    QModelIndex docIndex = GTUtilsProjectTreeView::findIndex("Project_export_test_0012.fa");
    GTUtilsProjectTreeView::findIndex("human_T1 (UCSC April 2002 chr7:115977709-117855134)", docIndex);
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // test for multiple annotation object selection associated sequence import is not available
    GTFileDialog::openFile(dataDir + "samples/GFF/", "5prime_utr_intron_A21.gff");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsProjectTreeView::click("Ca21chr5 features");
    GTUtilsProjectTreeView::click("Ca21chr1 features");
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Check that export service was already loaded & is enabled.
    GTUtils::checkExportServiceIsEnabled();

    GTUtilsDialog::waitForDialog(new PopupChecker({ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION, ACTION_EXPORT_CORRESPONDING_SEQ}, PopupChecker::NotExists));
    GTMouseDriver::click(Qt::RightButton);
}

}  // namespace GUITest_common_scenarios_project_sequence_exporting

}  // namespace U2
