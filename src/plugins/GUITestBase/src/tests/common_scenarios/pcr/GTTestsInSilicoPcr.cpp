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

#include <GTUtilsMdi.h>
#include <GTUtilsNotifications.h>
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2IdTypes.h>

#include "GTTestsInSilicoPcr.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsMeltingTemperature.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPcr.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "primitives/PopupChooser.h"
#include "runnables/ugene/corelibs/U2View/temperature/MeltingTemperatureSettingsDialogFiller.h"
#include "runnables/ugene/plugins/pcr/PrimersDetailsDialogFiller.h"
#include "runnables/ugene/plugins/workflow_designer/WizardFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_in_silico_pcr {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    GTUtilsPcr::clearPcrDir();
    // The PCR options panel is available only for nucleic sequences

    // 1. Open "_common_data/fasta/alphabet.fa".
    GTUtilsDialog::waitForDialog(new SequenceReadingModeSelectorDialogFiller(SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(testDir + "_common_data/fasta/alphabet.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Activate the "Amino" sequence in the sequence view (set the focus for it).
    GTWidget::click(GTUtilsSequenceView::getPanOrDetView(0));

    // 3. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // Expected: The panel is unavailable, the info message about alphabets is shown.
    auto params = GTWidget::findWidget("runPcrWidget");
    auto warning = GTWidget::findWidget("algoWarningLabel");
    CHECK_SET_ERR(!params->isEnabled(), "The panel is enabled for a wrong alphabet");
    CHECK_SET_ERR(warning->isVisible(), "No alphabet warning");

    // 4. Activate the "Nucl" sequence.
    GTWidget::click(GTUtilsSequenceView::getPanOrDetView(1));

    // Expected: The panel is available, the info message is hidden.
    CHECK_SET_ERR(params->isEnabled(), "The panel is disabled for the right alphabet");
    CHECK_SET_ERR(!warning->isVisible(), "The alphabet warning is shown");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTUtilsPcr::clearPcrDir();
    // Primer group box

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Set the focus at the primer line edit and write "Q%1" (not ACGT).
    //  TODO: wrap into api
    auto forwardPrimerBox = GTWidget::findWidget("forwardPrimerBox");
    auto forwardPrimerLine = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", forwardPrimerBox));
    GTLineEdit::setText(forwardPrimerLine, "Q%1", true);

    // Expected: the line edit is empty, the statistics label is empty.
    CHECK_SET_ERR(forwardPrimerLine->text().isEmpty(), "Wrong input");

    // 4. Write "atcg".
    GTLineEdit::setText(forwardPrimerLine, "atcg", true);

    // Expected: the line edit content is "ATCG", the statistics label shows the temperature and "4-mer".
    auto statsLabel = dynamic_cast<QLabel*>(GTWidget::findWidget("characteristicsLabel", forwardPrimerBox));
    CHECK_SET_ERR(forwardPrimerLine->text() == "ATCG", "No upper-case");
    CHECK_SET_ERR(statsLabel->text().endsWith("4-mer"), "Wrong statistics label");

    // 5. Click the reverse complement button.
    GTWidget::click(GTWidget::findWidget("reverseComplementButton", forwardPrimerBox));

    // Expected: the line edit content is "CGAT".
    CHECK_SET_ERR(forwardPrimerLine->text() == "CGAT", "Incorrect reverse-complement");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsPcr::clearPcrDir();
    // Availability of the PCR OP find button

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // Expected: the find buttom is disabled.
    auto findButton = GTWidget::findWidget("findProductButton");
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 1");

    // 3. Enter the forward primer "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT");

    // Expected: the find buttom is disabled.
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 2");

    // 4. Enter the reverse primer "GTGACCTTGGATGACAATAGGTTCCAAGGCTC".
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "GTGACCTTGGATGACAATAGGTTCCAAGGCTC");

    // Expected: the find buttom is enabled.
    CHECK_SET_ERR(findButton->isEnabled(), "Find button is disabled");

    // 5. Clear the forward primer.
    GTUtilsPcr::setPrimer(U2Strand::Direct, "");

    // Expected: the find buttom is disabled.
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 3");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTUtilsPcr::clearPcrDir();
    // Products table:
    //     Availability of the extract button
    //     Extract product result
    //     Double click

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT" and "GTGACCTTGGATGACAATAGGTTCCAAGGCTC".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "GTGACCTTGGATGACAATAGGTTCCAAGGCTC");

    // 4. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the product table is shown with one result [9..1196].
    CHECK_SET_ERR(GTUtilsPcr::productsCount() == 1, "Wrong results count");
    CHECK_SET_ERR(GTUtilsPcr::getResultRegion(0) == "9 - 1196", "Wrong result");

    // 5. Click the result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(0));
    GTMouseDriver::click();

    // Expected: the extract button is enabled.
    auto extractButton = GTWidget::findWidget("extractProductButton");
    CHECK_SET_ERR(extractButton->isEnabled(), "Extract button is disabled");

    // 6. Click the empty place of the table.
    QPoint emptyPoint = QPoint(GTUtilsPcr::getResultPoint(0));
    emptyPoint.setY(emptyPoint.y() + 30);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: the extract button is disabled.
    CHECK_SET_ERR(!extractButton->isEnabled(), "Extract button is enabled");

    // 7. Double click the result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(0));
    GTMouseDriver::doubleClick();

    // Expected: the new file is opened "pIB2-SEC13_9-1196.gb".
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("pIB2-SEC13_9-1196.gb");

    // TODO: remove it after fixing UGENE-3657
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTUtilsPcr::clearPcrDir();
    // Products table:
    //     Sequence view selection
    //     Extract several products result

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" with 0 mismatched and "CGGCATCCGCTTACAGAC" with 6.
    GTUtilsPcr::setPrimer(U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "CGGCATCCGCTTACAGAC");
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 6);
    GTUtilsPcr::setPerfectMatch(0);

    // 4. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the product table is shown with three results.
    CHECK_SET_ERR(3 == GTUtilsPcr::productsCount(), "Wrong results count");

    // 5. Click the 3rd result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(2));
    GTMouseDriver::click();

    // Expected: the sequence selection is [2..3775].
    //  TODO

    // 6. Click the 2nd result with CTRL pressed.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(1));
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected: the sequence selection is not changed, two results are selected in the table.
    //  TODO

    // 7. Click the extract button.
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));
    GTWidget::click(GTWidget::findWidget("extractProductButton"));

    // Expected: two new files are opened "pIB2-SEC13_2-133.gb" and "pIB2-SEC13_2-3775.gb".
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsProjectTreeView::findIndex("pIB2-SEC13_2-133.gb");
    GTUtilsProjectTreeView::findIndex("pIB2-SEC13_2-3775.gb");

    // TODO: remove it after fixing UGENE-3657
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTUtilsPcr::clearPcrDir();
    // Primers pair warning and details dialog

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the forward primer "AGACTCTTTCGTCTCACGCACTTCGCTGATA".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "AGACTCTTTCGTCTCACGCACTTCGCTGATA");

    // Expected: primer warning is hidden.
    auto warning = GTWidget::findWidget("warningLabel");
    CHECK_SET_ERR(!warning->isVisible(), "Primer warning is visible");

    // 4. Enter the reverse primer  and "TGACCGTCTCAGGAGGTGGTTGTGTCAGAGGTTTT".
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "TGACCGTCTCAGGAGGTGGTTGTGTCAGAGGTTTT");

    // Expected: primer warning is shown, the find button text is "Find product(s) anyway".
    auto findButton = dynamic_cast<QAbstractButton*>(GTWidget::findWidget("findProductButton"));
    CHECK_SET_ERR(warning->isVisible(), "Primer warning is not visible");
    CHECK_SET_ERR(findButton->text() == "Find product(s) anyway", "Wrong find button text 1");

    // 5. Click "Show details".
    // Expected: the details dialog is shown, the GC note of the forward primer is red.
    GTUtilsDialog::waitForDialog(new PrimersDetailsDialogFiller());
    GTMouseDriver::moveTo(GTUtilsPcr::getDetailsPoint());
    GTMouseDriver::click();

    // 6. Remove the last character of the forward primer.
    GTUtilsPcr::setPrimer(U2Strand::Direct, "AGACTCTTTCGTCTCACGCACTTCGCTGAT");
    // Expected: the warning is hidden, the find button text is "Find product(s)".
    CHECK_SET_ERR(!warning->isVisible(), "Primer warning is visible");
    CHECK_SET_ERR(findButton->text() == "Find product(s)", "Wrong find button text 2");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTUtilsPcr::clearPcrDir();
    // Algorithm parameters:
    //     Mismatches
    //     Product size

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" and "CGGCATCCGCTTACAGAC".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "CGGCATCCGCTTACAGAC");

    // 4. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the product table is shown with one result.
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(), "Wrong results count");

    // 5. Set the reverse mismatches: 6. Click the find button.
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 6);
    GTUtilsPcr::setPerfectMatch(0);
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are 3 results in the table.
    CHECK_SET_ERR(3 == GTUtilsPcr::productsCount(), "Wrong results count");

    // 6. Set the maximum product size: 3773. Click the find button.
    GTUtilsPcr::setMaxProductSize(3773);
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are 2 results in the table.
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(), "Wrong results count");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTUtilsPcr::clearPcrDir();
    // Circular sequences

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "AGGCCCTTTCGTCTCGCGCGTTTCGGTGATG" and "TGACCGTCTCCGGGAGCTGCATGTGTCAGAGGTTTT".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "AGGCCCTTTCGTCTCGCGCGTTTCGGTGATG");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "TGACCGTCTCCGGGAGCTGCATGTGTCAGAGGTTTT");

    // 4. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: no results found.
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(), "Wrong results count 1");

    // 5. Right click on the sequence object in the project view -> Mark as circular.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter("pIB2-SEC13"));
    GTUtilsDialog::waitForDialog(new PopupChooserByText({"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);

    // 6. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: one result found that goes through the 0 position [7223-60].
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(), "Wrong results count 2");
    CHECK_SET_ERR("7223 - 60" == GTUtilsPcr::getResultRegion(0), "Wrong result");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsPcr::clearPcrDir();
    // Algorithm parameters:
    //     3' perfect match
    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" and "CGACATCCGCTTACAGAC".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "CGACATCCGCTTACAGAC");

    // 4. Set the reverse mismatches: 1.
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 1);

    // 5. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: the product table is shown with one result.
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(), "Wrong results count 1");

    // 6. Set the 3' perfect match: 16.
    auto perfectSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget("perfectSpinBox"));
    GTSpinBox::setValue(perfectSpinBox, 16, GTGlobals::UseKeyBoard);

    // 7. Click the find button.
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are no results in the table.
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(), "Wrong results count 2");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // Export annotations
    // 1. Open "_common_data/cmdline/pcr/begin-end.gb".
    GTFileDialog::openFile(testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive();

    // 2. Open the PCR OP.
    GTWidget::click(GTWidget::findWidget("OP_IN_SILICO_PCR"));

    // 3. Enter the forward primer "GGGCCAAACAGGATATCTGTGGTAAGCAGT".
    GTUtilsPcr::setPrimer(U2Strand::Direct, "GGGCCAAACAGGATATCTGTGGTAAGCAGT");

    // 4. Enter the reverse primer  and "AAGCGCGCGAACAGAAGCGAGAAGCGAACT".
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "AAGCGCGCGAACAGAAGCGAGAAGCGAACT");

    // 5. Click "Find product(s) anyway".
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: one product is found.
    CHECK_SET_ERR(GTUtilsPcr::productsCount() == 1, "Wrong results count");

    // 6. Choose "Inner" annotation extraction.
    auto annsComboBox = GTWidget::findComboBox("annsComboBox");
    GTComboBox::selectItemByIndex(annsComboBox, 1);

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));

    // 7. Click "Export product(s)".
    auto extractPB = GTWidget::findWidget("extractProductButton");
    GTWidget::click(extractPB);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are 3 annotations in the exported document: 2 primers and center 51..150.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("middle", nullptr, {false}) == nullptr, "Unexpected annotation 1");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString("center") == "complement(51..150)", "Wrong region 1");

    // 8. Choose "All annotations" annotation extraction.
    GTUtilsProjectTreeView::doubleClickItem("begin-end.gb");
    GTUtilsMdi::checkWindowIsActive("begin-end");
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));
    GTComboBox::selectItemByIndex(annsComboBox, 0);
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));

    // 9. Click "Export product(s)".
    extractPB = GTWidget::findWidget("extractProductButton");
    GTWidget::click(extractPB);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are 4 annotations in the exported document: 2 primers, center 51..150 and middle 1..200. Middle has the warning qualifier.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString("middle") == "1..200", "Wrong region 2");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString("center") == "complement(51..150)", "Wrong region 3");

    // 10. Choose "None" annotation extraction.
    GTUtilsProjectTreeView::doubleClickItem("begin-end.gb");
    GTUtilsMdi::checkWindowIsActive("begin-end");
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));
    GTComboBox::selectItemByIndex(annsComboBox, 2);
    GTWidget::click(GTWidget::findWidget("ArrowHeader_Settings"));

    // 11. Click "Export product(s)".
    extractPB = GTWidget::findWidget("extractProductButton");
    GTWidget::click(extractPB);
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: there are only 2 primers annotations in the exported document.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("middle", nullptr, {false}) == nullptr, "Unexpected annotation 2");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem("center", nullptr, {false}) == nullptr, "Unexpected annotation 3");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // The temperature label for one primer
    GTUtilsPcr::clearPcrDir();

    // 1. Open "_common_data/fasta/pcr_test.fa"
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(U2Strand::Direct).contains("N/A"), "The temperature is not configured")

    // 4. Clear the primer line edit
    GTUtilsPcr::setPrimer(U2Strand::Direct, "");

    // Expected: the temperature label is empty
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(U2Strand::Direct).isEmpty(), "The temperature was not updated");

    // 5. Enter the primer "TTCGGTS"
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTCGGTS");
    // Expected: the temperature is N/A, because the sequence contains a few regions that correspond to the primer
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(U2Strand::Direct).contains("N/A"), "The temperature is not configured");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // The warning messages for the primer pair
    GTUtilsPcr::clearPcrDir();

    // 1. Open "_common_data/fasta/begin-end.fa"
    GTFileDialog::openFile(testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the forward primer "KGGCCAHACAGRATATCTSTGGTAAGCAGT"
    GTUtilsPcr::setPrimer(U2Strand::Direct, "KGGCCAHACAGRATATCTSTGGTAAGCAGT");

    // Expected: the temperature is defined
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(U2Strand::Direct).contains("N/A"), "The temperature is not configured");

    // 4. Clear the reverse primer "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNR");

    // Expected: the temperature is N/A, the primer pair info contains the message about non-ACGTN symbols
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(U2Strand::Complementary).contains("N/A"), "The temperature is configured");

    auto warningLabel = GTWidget::findLabel("warningLabel");
    CHECK_SET_ERR(warningLabel->text().contains("The primers contain a character from the Extended DNA alphabet."), "Incorrect warning message");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Find the product with degenerated primers
    GTUtilsPcr::clearPcrDir();

    // 1. Open "_common_data/fasta/pcr_test.fa"
    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the forward primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(U2Strand::Direct).contains("N/A"), "The temperature is not configured");

    // 4. Enter the reverse primer "GBGNCCTTGGATGACAATVGGTTCCAAGRCTC"
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "GBGNCCTTGGATGACAATVGGTTCCAAGRCTC");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(U2Strand::Complementary).contains("N/A"), "The temperature is not configured");

    // 5. Find product
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: one product is found
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(), "Wrong results count");
    CHECK_SET_ERR("9 - 1196" == GTUtilsPcr::getResultRegion(0), "Wrong result");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Reverse-complement for the extended DNA alphabet
    // 1. Open the PCT OP
    // 2. Enter all symbols of extended DNA alphabet: "ACGTMRWSYKVHDBNX"
    // 3. Click "Reverse-complement" button
    // Expected state: the content of primer line edit is "XNVHDBMRSWYKACGT"
    GTUtilsPcr::clearPcrDir();

    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    GTUtilsPcr::setPrimer(U2Strand::Direct, "ACGTMRWSYKVHDBNX");

    GTWidget::click(GTWidget::findWidget("reverseComplementButton", GTUtilsPcr::primerBox(U2Strand::Direct)));

    auto primerLineEdit = GTWidget::findLineEdit("primerEdit", GTUtilsPcr::primerBox(U2Strand::Direct));

    CHECK_SET_ERR(primerLineEdit->text() == "XNVHDBMRSWYKACGT", "Incorrect reverse-complement primer translation");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // Degenerated character in perfect match region

    // 1. Open murine.gb
    GTUtilsPcr::clearPcrDir();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Go to the PCR OP tab and add primers:
    //    Forward: ACCCGTAGGTGGCAAGCTAGCTTAA
    //    Reverse: TTTTCTATTCTCAGTTATGTATTTTT
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsPcr::setPrimer(U2Strand::Direct, "ACCCGTAGGTGGCAAGCTAGCTTAA");
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "TTTTCTATTCTCAGTTATGTATTTTT");

    // 3. Find products
    //    Expected state: there are two results
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(), "Wrong results count 1");

    // 4. Replace reverse primer with TTTTCTATTCTCAGTTATGTATTTTA
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "TTTTCTATTCTCAGTTATGTATTTTA");

    // 5. Set mismatches to 1
    GTUtilsPcr::setMismatches(U2Strand::Complementary, 1);

    // 6. Find products
    //    Expected state: there no products because the mismatch is located in 'Perfect Match' area
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(), "Wrong results count 2");

    // 7. Replace reverse primer with ATTTCTATTCTCAGTTATGTATTTTW
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "ATTTCTATTCTCAGTTATGTATTTTW");

    // 8. Find products
    //    Expected state: there are two results, because W= {A, T}
    GTWidget::click(GTWidget::findWidget("findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(), "Wrong results count 3");
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    // Gaps are not allowed for primers
    // 1. Open murine.gb
    GTUtilsPcr::clearPcrDir();
    GTFileDialog::openFile(dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Go to the PCR OP tab
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Try to input gap symbol '-'
    GTUtilsPcr::setPrimer(U2Strand::Direct, "---");

    // Expected state: '-' pressing is ignored
    auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", GTUtilsPcr::primerBox(U2Strand::Direct)));
    CHECK_SET_ERR(primerEdit != nullptr, "Cannot find primer line edit");
    CHECK_SET_ERR(primerEdit->text().isEmpty(), "There are unexpected characters in PrimerLineEdit");
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Reverse-complement for the extended DNA alphabet
    // 1. Open the PCT OP
    // 2. Enter primer with whitespaces
    // Expected state: whitespaces removed successfuly
    GTUtilsPcr::clearPcrDir();

    GTFileDialog::openFile(testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget("primerEdit", GTWidget::findWidget("forwardPrimerBox")));
    GTLineEdit::setText(primerEdit, "AC\r\nCCTG   GAGAG\nCATCG\tAT", true, true);

    CHECK_SET_ERR(primerEdit->text() == "ACCCTGGAGAGCATCGAT", "Incorrect whitespaces removing");
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    // 1. Open the "pcr_check_ambiguous.seq" file
    GTUtilsPcr::clearPcrDir();
    GTFileDialog::openFile(testDir + "_common_data/cmdline/pcr/", "pcr_check_ambiguous.seq");
    GTUtilsTaskTreeView::waitTaskFinished();

    // 2. Go to the PCR OP tab
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Set "TTGTCAGAATTCACCA" as the direct primer
    GTUtilsPcr::setPrimer(U2Strand::Direct, "TTGTCAGAATTCACCA");

    // 4. Set "TAGTCTGATGGGCTTCCCTTTGT" as the complementary primer
    GTUtilsPcr::setPrimer(U2Strand::Complementary, "TAGTCTGATGGGCTTCCCTTTGT");

    // 5. Search for results
    GTWidget::click(GTWidget::findWidget("findProductButton"));

    // Expected: 1 result has been found
    int count = GTUtilsPcr::productsCount();
    CHECK_SET_ERR(count == 1, QString("Unexpected results count, expected: 1, current: %1").arg(count));

    // 6. Set "Use ambiguous bases" unchecked
    GTUtilsPcr::setUseAmbiguousBases(false);

    // 7. Search for results
    GTWidget::click(GTWidget::findWidget("findProductButton"));

    // Expected: 0 result has been found
    count = GTUtilsPcr::productsCount();
    CHECK_SET_ERR(count == 0, QString("Unexpected results count, expected: 0, current: %1").arg(count));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            // Expected state: "In Silico PCR" dialog has appered
            QWidget* wizard = GTWidget::getActiveModalWidget();
            GTWidget::click(wizard);

            // 3. Select "_common_data/cmdline/pcr/pcr_check_ambiguous.seq"
            GTUtilsWizard::setInputFiles({{testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous.seq"}});

            // 4. Push "Next"
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            // 5. Push "Next"
            GTUtilsWizard::setParameter("Primers URL", testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous_primers.fa");
            GTUtilsWizard::setParameter("Mismatches", 0);

            // 6. Push "Next"
            GTUtilsWizard::clickButton(GTUtilsWizard::Next);

            // 7. Push "Next" twice
            GTUtilsWizard::clickButton(GTUtilsWizard::Apply);
        }
    };

    // 2. Open the "In Silico PCR" sample
    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new Scenario()));
    GTUtilsWorkflowDesigner::addSample("In silico PCR");

    // 8. Set output to sandBoxDir + "result.gb"
    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", sandBoxDir + "result.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    // 9. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: result.gb in the result files
    auto outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(outputFiles.contains("result.gb"), "No expected file: \"result.gb\"");

    // 10. Go to dashboard
    GTUtilsWorkflowDesigner::returnToWorkflow();

    // 11. Set "Use ambiguous bases", change output to sandBoxDir + "result_1.gb"
    GTUtilsWorkflowDesigner::click("In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter("Use ambiguous bases", false, GTUtilsWorkflowDesigner::valueType::comboValue);
    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", sandBoxDir + "result_1.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    // 12. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    // Expected: result_1.gb is absent
    outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(outputFiles.size() == 1, "Unexpected PCR result exists");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    // Open "samples/FASTA/human_T1.fa".
    // Open the PCR OP tab.
    // Enter the forward primer: AAAGGAAAAAATGCT.
    // Enter the reverse primer: AGCATTTTTTCCTTT.
    // Check temperatures
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsOptionPanelSequenceView::setForwardPrimer("AAAGTTGAAATGAAGGAAAA");
    GTUtilsOptionPanelSequenceView::setReversePrimer("CGCTTCCTTCAGGAGCTCTTTTAGGG");

    auto detailsDirect = GTUtilsPcr::getPrimerInfo(U2Strand::Direct);
    CHECK_SET_ERR(detailsDirect.contains("49.09"), "Unexpected direct primer temperature, expected: 49.09, got details: " + detailsDirect);

    auto detailsComplementary = GTUtilsPcr::getPrimerInfo(U2Strand::Complementary);
    CHECK_SET_ERR(detailsComplementary.contains("65.14"), "Unexpected complementary primer temperature, expected: 65.14, got details: " + detailsComplementary);

    GTUtilsOptionPanelSequenceView::openInSilicoPcrMeltingTemperatureShowHideWidget();
    GTUtilsOptionsPanel::resizeToMaximum();
    struct Steps {
        Steps(GTUtilsMeltingTemperature::Parameter _step, const QString& _stringValue, const QString& _directTemp, const QString& _complementaryTemp)
            : step(_step), stringValue(_stringValue), directTemp(_directTemp), complementaryTemp(_complementaryTemp) {
        }
        GTUtilsMeltingTemperature::Parameter step;
        QString stringValue;
        QString directTemp;
        QString complementaryTemp;
    };

    static const QList<Steps> steps = {
        Steps(GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00", "49.12", "65.17"),
        Steps(GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00", "49.15", "65.2"),
        Steps(GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50", "43.47", "59.26"),
        Steps(GTUtilsMeltingTemperature::Parameter::DntpConc, "0", "48.2", "64.2"),
        Steps(GTUtilsMeltingTemperature::Parameter::DmsoConc, "1", "47.6", "63.6"),
        Steps(GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6", "46.6", "62.6"),
        Steps(GTUtilsMeltingTemperature::Parameter::FormamideConc, "1", "43.83", "59.97"),
        Steps(GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0", "58.34", "75.27"),
        Steps(GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2", "58.06", "76.78"),
        Steps(GTUtilsMeltingTemperature::Parameter::MaxLen, "19", "42.99", "61.87")};

    for (const auto& step : qAsConst(steps)) {
        GTUtilsMeltingTemperature::setParameters({{step.step, step.stringValue}}, nullptr);
        detailsDirect = GTUtilsPcr::getPrimerInfo(U2Strand::Direct);
        CHECK_SET_ERR(detailsDirect.contains(step.directTemp), QString("Unexpected direct primer temperature, expected: %1").arg(step.directTemp));

        detailsComplementary = GTUtilsPcr::getPrimerInfo(U2Strand::Complementary);
        CHECK_SET_ERR(detailsComplementary.contains(step.complementaryTemp), QString("Unexpected complementary primer temperature, expected: %1").arg(step.complementaryTemp));
    }
}

GUI_TEST_CLASS_DEFINITION(test_0021) {
    // Open WD
    // Open the "In Silico PCR" sample
    // Expected state: "In Silico PCR" dialog has appered
    // Click "Cancel"
    // Click on the "Read sequence" element
    // Set "_common_data/cmdline/pcr/pcr_check_ambiguous.seq"
    // Click on the "In Silico PCR" element
    // Set "_common_data/cmdline/pcr/pcr_check_ambiguous_primers.fa" as primers
    // Set not-default temperature settings
    // Click "Write sequence"
    // Set output to sandBoxDir + "result.gb"
    // Run workflow
    // Expected: no errors, result.gb in the result files
    GTUtilsWorkflowDesigner::openWorkflowDesigner();

    class Scenario : public CustomScenario {
    public:
        void run() override {
            GTUtilsWizard::clickButton(GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(new WizardFiller("In Silico PCR", new Scenario()));
    GTUtilsWorkflowDesigner::addSample("In silico PCR");

    GTUtilsWorkflowDesigner::click("Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous.seq");

    GTUtilsWorkflowDesigner::click("In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter("Primers URL", testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous_primers.fa", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);
    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
        {GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3"},
        {GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00"},
        {GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00"},
        {GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50"},
        {GTUtilsMeltingTemperature::Parameter::DntpConc, "0"},
        {GTUtilsMeltingTemperature::Parameter::DmsoConc, "1"},
        {GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6"},
        {GTUtilsMeltingTemperature::Parameter::FormamideConc, "1"},
        {GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0"},
        {GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2"},
        {GTUtilsMeltingTemperature::Parameter::MaxLen, "33"}};
    GTUtilsDialog::waitForDialog(new MeltingTemperatureSettingsDialogFiller(parameters));
    GTUtilsWorkflowDesigner::setParameter("Temperature settings", "", GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::click("Write Sequence");
    GTUtilsWorkflowDesigner::setParameter("Output file", sandBoxDir + "result.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::runWorkflow();
    GTUtilsTaskTreeView::waitTaskFinished();

    auto outputFiles = GTUtilsDashboard::getOutputFiles();
    CHECK_SET_ERR(outputFiles.contains("result.gb"), "No expected file: \"result.gb\"");
}

}  // namespace GUITest_common_scenarios_in_silico_pcr
}  // namespace U2
