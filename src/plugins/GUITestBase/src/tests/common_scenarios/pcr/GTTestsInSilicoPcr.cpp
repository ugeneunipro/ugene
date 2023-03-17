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
#include <primitives/GTTextEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <U2Core/U2IdTypes.h>

#include "GTTestsInSilicoPcr.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsDashboard.h"

#include "GTUtilsMeltingTemperature.h"
#include "GTUtilsOptionPanelSequenceView.h"
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
    GTUtilsPcr::clearPcrDir(os);
    // The PCR options panel is available only for nucleic sequences

    // 1. Open "_common_data/fasta/alphabet.fa".
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTUtilsProject::openFile(os, testDir + "_common_data/fasta/alphabet.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Activate the "Amino" sequence in the sequence view (set the focus for it).
    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os, 0));

    // 3. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // Expected: The panel is unavailable, the info message about alphabets is shown.
    auto params = GTWidget::findWidget(os, "runPcrWidget");
    auto warning = GTWidget::findWidget(os, "algoWarningLabel");
    CHECK_SET_ERR(!params->isEnabled(), "The panel is enabled for a wrong alphabet");
    CHECK_SET_ERR(warning->isVisible(), "No alphabet warning");

    // 4. Activate the "Nucl" sequence.
    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os, 1));

    // Expected: The panel is available, the info message is hidden.
    CHECK_SET_ERR(params->isEnabled(), "The panel is disabled for the right alphabet");
    CHECK_SET_ERR(!warning->isVisible(), "The alphabet warning is shown");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    GTUtilsPcr::clearPcrDir(os);
    // Primer group box

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Set the focus at the primer line edit and write "Q%1" (not ACGT).
    //  TODO: wrap into api
    auto forwardPrimerBox = GTWidget::findWidget(os, "forwardPrimerBox");
    auto forwardPrimerLine = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit", forwardPrimerBox));
    GTLineEdit::setText(os, forwardPrimerLine, "Q%1", true);

    // Expected: the line edit is empty, the statistics label is empty.
    CHECK_SET_ERR(forwardPrimerLine->text().isEmpty(), "Wrong input");

    // 4. Write "atcg".
    GTLineEdit::setText(os, forwardPrimerLine, "atcg", true);

    // Expected: the line edit content is "ATCG", the statistics label shows the temperature and "4-mer".
    auto statsLabel = dynamic_cast<QLabel*>(GTWidget::findWidget(os, "characteristicsLabel", forwardPrimerBox));
    CHECK_SET_ERR(forwardPrimerLine->text() == "ATCG", "No upper-case");
    CHECK_SET_ERR(statsLabel->text().endsWith("4-mer"), "Wrong statistics label");

    // 5. Click the reverse complement button.
    GTWidget::click(os, GTWidget::findWidget(os, "reverseComplementButton", forwardPrimerBox));

    // Expected: the line edit content is "CGAT".
    CHECK_SET_ERR(forwardPrimerLine->text() == "CGAT", "Incorrect reverse-complement");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    GTUtilsPcr::clearPcrDir(os);
    // Availability of the PCR OP find button

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // Expected: the find buttom is disabled.
    auto findButton = GTWidget::findWidget(os, "findProductButton");
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 1");

    // 3. Enter the forward primer "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT");

    // Expected: the find buttom is disabled.
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 2");

    // 4. Enter the reverse primer "GTGACCTTGGATGACAATAGGTTCCAAGGCTC".
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "GTGACCTTGGATGACAATAGGTTCCAAGGCTC");

    // Expected: the find buttom is enabled.
    CHECK_SET_ERR(findButton->isEnabled(), "Find button is disabled");

    // 5. Clear the forward primer.
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "");

    // Expected: the find buttom is disabled.
    CHECK_SET_ERR(!findButton->isEnabled(), "Find button is enabled 3");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    GTUtilsPcr::clearPcrDir(os);
    // Products table:
    //     Availability of the extract button
    //     Extract product result
    //     Double click

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT" and "GTGACCTTGGATGACAATAGGTTCCAAGGCTC".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTCGGTGATGACGGTGAAAACCTCTGACACATGCAGCT");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "GTGACCTTGGATGACAATAGGTTCCAAGGCTC");

    // 4. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the product table is shown with one result [9..1196].
    CHECK_SET_ERR(GTUtilsPcr::productsCount(os) == 1, "Wrong results count");
    CHECK_SET_ERR(GTUtilsPcr::getResultRegion(os, 0) == "9 - 1196", "Wrong result");

    // 5. Click the result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(os, 0));
    GTMouseDriver::click();

    // Expected: the extract button is enabled.
    auto extractButton = GTWidget::findWidget(os, "extractProductButton");
    CHECK_SET_ERR(extractButton->isEnabled(), "Extract button is disabled");

    // 6. Click the empty place of the table.
    QPoint emptyPoint = QPoint(GTUtilsPcr::getResultPoint(os, 0));
    emptyPoint.setY(emptyPoint.y() + 30);
    GTMouseDriver::moveTo(emptyPoint);
    GTMouseDriver::click();

    // Expected: the extract button is disabled.
    CHECK_SET_ERR(!extractButton->isEnabled(), "Extract button is enabled");

    // 7. Double click the result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(os, 0));
    GTMouseDriver::doubleClick();

    // Expected: the new file is opened "pIB2-SEC13_9-1196.gb".
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "pIB2-SEC13_9-1196.gb");

    // TODO: remove it after fixing UGENE-3657
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTUtilsPcr::clearPcrDir(os);
    // Products table:
    //     Sequence view selection
    //     Extract several products result

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" with 0 mismatched and "CGGCATCCGCTTACAGAC" with 6.
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CGGCATCCGCTTACAGAC");
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 6);
    GTUtilsPcr::setPerfectMatch(os, 0);

    // 4. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the product table is shown with three results.
    CHECK_SET_ERR(3 == GTUtilsPcr::productsCount(os), "Wrong results count");

    // 5. Click the 3rd result.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(os, 2));
    GTMouseDriver::click();

    // Expected: the sequence selection is [2..3775].
    //  TODO

    // 6. Click the 2nd result with CTRL pressed.
    GTMouseDriver::moveTo(GTUtilsPcr::getResultPoint(os, 1));
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTMouseDriver::click();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    // Expected: the sequence selection is not changed, two results are selected in the table.
    //  TODO

    // 7. Click the extract button.
    GTWidget::click(os, GTWidget::findWidget(os, "extractProductButton"));

    // Expected: two new files are opened "pIB2-SEC13_2-133.gb" and "pIB2-SEC13_2-3775.gb".
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsProjectTreeView::findIndex(os, "pIB2-SEC13_2-133.gb");
    GTUtilsProjectTreeView::findIndex(os, "pIB2-SEC13_2-3775.gb");

    // TODO: remove it after fixing UGENE-3657
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
    GTKeyboardDriver::keyClick('w', Qt::ControlModifier);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    GTUtilsPcr::clearPcrDir(os);
    // Primers pair warning and details dialog

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the forward primer "AGACTCTTTCGTCTCACGCACTTCGCTGATA".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "AGACTCTTTCGTCTCACGCACTTCGCTGATA");

    // Expected: primer warning is hidden.
    auto warning = GTWidget::findWidget(os, "warningLabel");
    CHECK_SET_ERR(!warning->isVisible(), "Primer warning is visible");

    // 4. Enter the reverse primer  and "TGACCGTCTCAGGAGGTGGTTGTGTCAGAGGTTTT".
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "TGACCGTCTCAGGAGGTGGTTGTGTCAGAGGTTTT");

    // Expected: primer warning is shown, the find button text is "Find product(s) anyway".
    auto findButton = dynamic_cast<QAbstractButton*>(GTWidget::findWidget(os, "findProductButton"));
    CHECK_SET_ERR(warning->isVisible(), "Primer warning is not visible");
    CHECK_SET_ERR(findButton->text() == "Find product(s) anyway", "Wrong find button text 1");

    // 5. Click "Show details".
    // Expected: the details dialog is shown, the GC note of the forward primer is red.
    GTUtilsDialog::waitForDialog(os, new PrimersDetailsDialogFiller(os));
    GTMouseDriver::moveTo(GTUtilsPcr::getDetailsPoint(os));
    GTMouseDriver::click();

    // 6. Remove the last character of the forward primer.
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "AGACTCTTTCGTCTCACGCACTTCGCTGAT");
    // Expected: the warning is hidden, the find button text is "Find product(s)".
    CHECK_SET_ERR(!warning->isVisible(), "Primer warning is visible");
    CHECK_SET_ERR(findButton->text() == "Find product(s)", "Wrong find button text 2");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    GTUtilsPcr::clearPcrDir(os);
    // Algorithm parameters:
    //     Mismatches
    //     Product size

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" and "CGGCATCCGCTTACAGAC".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CGGCATCCGCTTACAGAC");

    // 4. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the product table is shown with one result.
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(os), "Wrong results count");

    // 5. Set the reverse mismatches: 6. Click the find button.
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 6);
    GTUtilsPcr::setPerfectMatch(os, 0);
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are 3 results in the table.
    CHECK_SET_ERR(3 == GTUtilsPcr::productsCount(os), "Wrong results count");

    // 6. Set the maximum product size: 3773. Click the find button.
    GTUtilsPcr::setMaxProductSize(os, 3773);
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are 2 results in the table.
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(os), "Wrong results count");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    GTUtilsPcr::clearPcrDir(os);
    // Circular sequences

    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "AGGCCCTTTCGTCTCGCGCGTTTCGGTGATG" and "TGACCGTCTCCGGGAGCTGCATGTGTCAGAGGTTTT".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "AGGCCCTTTCGTCTCGCGCGTTTCGGTGATG");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "TGACCGTCTCCGGGAGCTGCATGTGTCAGAGGTTTT");

    // 4. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: no results found.
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(os), "Wrong results count 1");

    // 5. Right click on the sequence object in the project view -> Mark as circular.
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "pIB2-SEC13"));
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, {"Mark as circular"}));
    GTMouseDriver::click(Qt::RightButton);

    // 6. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: one result found that goes through the 0 position [7223-60].
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(os), "Wrong results count 2");
    CHECK_SET_ERR("7223 - 60" == GTUtilsPcr::getResultRegion(os, 0), "Wrong result");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    GTUtilsPcr::clearPcrDir(os);
    // Algorithm parameters:
    //     3' perfect match
    // 1. Open "_common_data/fasta/pcr_test.fa".
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the primers: "CGCGCGTTTCGGTGA" and "CGACATCCGCTTACAGAC".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "CGCGCGTTTCGGTGA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CGACATCCGCTTACAGAC");

    // 4. Set the reverse mismatches: 1.
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 1);

    // 5. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: the product table is shown with one result.
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(os), "Wrong results count 1");

    // 6. Set the 3' perfect match: 16.
    auto perfectSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "perfectSpinBox"));
    GTSpinBox::setValue(os, perfectSpinBox, 16, GTGlobals::UseKeyBoard);

    // 7. Click the find button.
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are no results in the table.
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(os), "Wrong results count 2");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // Export annotations
    // 1. Open "_common_data/cmdline/pcr/begin-end.gb".
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // 2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    // 3. Enter the forward primer "GGGCCAAACAGGATATCTGTGGTAAGCAGT".
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "GGGCCAAACAGGATATCTGTGGTAAGCAGT");

    // 4. Enter the reverse primer  and "AAGCGCGCGAACAGAAGCGAGAAGCGAACT".
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "AAGCGCGCGAACAGAAGCGAGAAGCGAACT");

    // 5. Click "Find product(s) anyway".
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: one product is found.
    CHECK_SET_ERR(GTUtilsPcr::productsCount(os) == 1, "Wrong results count");

    // 6. Choose "Inner" annotation extraction.
    auto annsComboBox = GTWidget::findComboBox(os, "annsComboBox");
    GTComboBox::selectItemByIndex(os, annsComboBox, 1);

    // 7. Click "Export product(s)".
    auto extractPB = GTWidget::findWidget(os, "extractProductButton");
    GTUtilsNotifications::waitAllNotificationsClosed(os);
    GTWidget::click(os, extractPB);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are 3 annotations in the exported document: 2 primers and center 51..150.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "middle", nullptr, {false}) == nullptr, "Unexpected annotation 1");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "center") == "complement(51..150)", "Wrong region 1");

    // 8. Choose "All annotations" annotation extraction.
    GTUtilsProjectTreeView::doubleClickItem(os, "begin-end.gb");
    GTUtilsMdi::checkWindowIsActive(os, "begin-end");
    GTComboBox::selectItemByIndex(os, annsComboBox, 0);

    // 9. Click "Export product(s)".
    extractPB = GTWidget::findWidget(os, "extractProductButton");
    GTUtilsNotifications::waitAllNotificationsClosed(os);
    GTWidget::click(os, extractPB);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are 4 annotations in the exported document: 2 primers, center 51..150 and middle 1..200. Middle has the warning qualifier.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "middle") == "1..200", "Wrong region 2");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "center") == "complement(51..150)", "Wrong region 3");

    // 10. Choose "None" annotation extraction.
    GTUtilsProjectTreeView::doubleClickItem(os, "begin-end.gb");
    GTUtilsMdi::checkWindowIsActive(os, "begin-end");
    GTComboBox::selectItemByIndex(os, annsComboBox, 2);

    // 11. Click "Export product(s)".
    extractPB = GTWidget::findWidget(os, "extractProductButton");
    GTUtilsNotifications::waitAllNotificationsClosed(os);
    GTWidget::click(os, extractPB);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: there are only 2 primers annotations in the exported document.
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "middle", nullptr, {false}) == nullptr, "Unexpected annotation 2");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findItem(os, "center", nullptr, {false}) == nullptr, "Unexpected annotation 3");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // The temperature label for one primer
    GTUtilsPcr::clearPcrDir(os);

    // 1. Open "_common_data/fasta/pcr_test.fa"
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct).contains("N/A"), "The temperature is not configured")

    // 4. Clear the primer line edit
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "");

    // Expected: the temperature label is empty
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct).isEmpty(), "The temperature was not updated");

    // 5. Enter the primer "TTCGGTS"
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTCGGTS");
    // Expected: the temperature is N/A, because the sequence contains a few regions that correspond to the primer
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct).contains("N/A"), "The temperature is not configured");
}

GUI_TEST_CLASS_DEFINITION(test_0012) {
    // The warning messages for the primer pair
    GTUtilsPcr::clearPcrDir(os);

    // 1. Open "_common_data/fasta/begin-end.fa"
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/pcr/begin-end.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the forward primer "KGGCCAHACAGRATATCTSTGGTAAGCAGT"
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "KGGCCAHACAGRATATCTSTGGTAAGCAGT");

    // Expected: the temperature is defined
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct).contains("N/A"), "The temperature is not configured");

    // 4. Clear the reverse primer "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNR"
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNR");

    // Expected: the temperature is N/A, the primer pair info contains the message about non-ACGTN symbols
    CHECK_SET_ERR(GTUtilsPcr::getPrimerInfo(os, U2Strand::Complementary).contains("N/A"), "The temperature is configured");

    auto warningLabel = GTWidget::findLabel(os, "warningLabel");
    CHECK_SET_ERR(warningLabel->text().contains("The primers contain a character from the Extended DNA alphabet."), "Incorrect warning message");
}

GUI_TEST_CLASS_DEFINITION(test_0013) {
    // Find the product with degenerated primers
    GTUtilsPcr::clearPcrDir(os);

    // 1. Open "_common_data/fasta/pcr_test.fa"
    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Open the PCR OP
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Enter the forward primer "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT"
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTNGGTGATGWCGGTGAAARCCTCTGACMCATGCAGCT");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct).contains("N/A"), "The temperature is not configured");

    // 4. Enter the reverse primer "GBGNCCTTGGATGACAATVGGTTCCAAGRCTC"
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "GBGNCCTTGGATGACAATVGGTTCCAAGRCTC");

    // Expected: the temperature label contains the correct temperature, because the sequence has only one suitable region
    CHECK_SET_ERR(!GTUtilsPcr::getPrimerInfo(os, U2Strand::Complementary).contains("N/A"), "The temperature is not configured");

    // 5. Find product
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: one product is found
    CHECK_SET_ERR(1 == GTUtilsPcr::productsCount(os), "Wrong results count");
    CHECK_SET_ERR("9 - 1196" == GTUtilsPcr::getResultRegion(os, 0), "Wrong result");
}

GUI_TEST_CLASS_DEFINITION(test_0014) {
    // Reverse-complement for the extended DNA alphabet
    // 1. Open the PCT OP
    // 2. Enter all symbols of extended DNA alphabet: "ACGTMRWSYKVHDBNX"
    // 3. Click "Reverse-complement" button
    // Expected state: the content of primer line edit is "XNVHDBMRSWYKACGT"
    GTUtilsPcr::clearPcrDir(os);

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "ACGTMRWSYKVHDBNX");

    GTWidget::click(os, GTWidget::findWidget(os, "reverseComplementButton", GTUtilsPcr::primerBox(os, U2Strand::Direct)));

    auto primerLineEdit = GTWidget::findLineEdit(os, "primerEdit", GTUtilsPcr::primerBox(os, U2Strand::Direct));

    CHECK_SET_ERR(primerLineEdit->text() == "XNVHDBMRSWYKACGT", "Incorrect reverse-complement primer translation");
}

GUI_TEST_CLASS_DEFINITION(test_0015) {
    // Degenerated character in perfect match region

    // 1. Open murine.gb
    GTUtilsPcr::clearPcrDir(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Go to the PCR OP tab and add primers:
    //    Forward: ACCCGTAGGTGGCAAGCTAGCTTAA
    //    Reverse: TTTTCTATTCTCAGTTATGTATTTTT
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "ACCCGTAGGTGGCAAGCTAGCTTAA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "TTTTCTATTCTCAGTTATGTATTTTT");

    // 3. Find products
    //    Expected state: there are two results
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(os), "Wrong results count 1");

    // 4. Replace reverse primer with TTTTCTATTCTCAGTTATGTATTTTA
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "TTTTCTATTCTCAGTTATGTATTTTA");

    // 5. Set mismatches to 1
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 1);

    // 6. Find products
    //    Expected state: there no products because the mismatch is located in 'Perfect Match' area
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(0 == GTUtilsPcr::productsCount(os), "Wrong results count 2");

    // 7. Replace reverse primer with ATTTCTATTCTCAGTTATGTATTTTW
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "ATTTCTATTCTCAGTTATGTATTTTW");

    // 8. Find products
    //    Expected state: there are two results, because W= {A, T}
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(2 == GTUtilsPcr::productsCount(os), "Wrong results count 3");
}

GUI_TEST_CLASS_DEFINITION(test_0016) {
    // Gaps are not allowed for primers
    // 1. Open murine.gb
    GTUtilsPcr::clearPcrDir(os);
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Go to the PCR OP tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Try to input gap symbol '-'
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "---");

    // Expected state: '-' pressing is ignored
    auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit", GTUtilsPcr::primerBox(os, U2Strand::Direct)));
    CHECK_SET_ERR(primerEdit != nullptr, "Cannot find primer line edit");
    CHECK_SET_ERR(primerEdit->text().isEmpty(), "There are unexpected characters in PrimerLineEdit");
}

GUI_TEST_CLASS_DEFINITION(test_0017) {
    // Reverse-complement for the extended DNA alphabet
    // 1. Open the PCT OP
    // 2. Enter primer with whitespaces
    // Expected state: whitespaces removed successfuly
    GTUtilsPcr::clearPcrDir(os);

    GTFileDialog::openFile(os, testDir + "_common_data/fasta", "pcr_test.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    auto primerEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "primerEdit", GTWidget::findWidget(os, "forwardPrimerBox")));
    GTLineEdit::setText(os, primerEdit, "AC\r\nCCTG   GAGAG\nCATCG\tAT", true, true);

    CHECK_SET_ERR(primerEdit->text() == "ACCCTGGAGAGCATCGAT", "Incorrect whitespaces removing");
}

GUI_TEST_CLASS_DEFINITION(test_0018) {
    // 1. Open the "pcr_check_ambiguous.seq" file
    GTUtilsPcr::clearPcrDir(os);
    GTFileDialog::openFile(os, testDir + "_common_data/cmdline/pcr/", "pcr_check_ambiguous.seq");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Go to the PCR OP tab
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);

    // 3. Set "TTGTCAGAATTCACCA" as the direct primer
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTGTCAGAATTCACCA");

    // 4. Set "TAGTCTGATGGGCTTCCCTTTGT" as the complementary primer
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "TAGTCTGATGGGCTTCCCTTTGT");

    // 5. Search for results
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));

    // Expected: 1 result has been found
    int count = GTUtilsPcr::productsCount(os);
    CHECK_SET_ERR(count == 1, QString("Unexpected results count, expected: 1, current: %1").arg(count));

    // 6. Set "Use ambiguous bases" unchecked
    GTUtilsPcr::setUseAmbiguousBases(os, false);

    // 7. Search for results
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));

    // Expected: 0 result has been found
    count = GTUtilsPcr::productsCount(os);
    CHECK_SET_ERR(count == 0, QString("Unexpected results count, expected: 0, current: %1").arg(count));
}

GUI_TEST_CLASS_DEFINITION(test_0019) {
    // 1. Open WD
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {
            // Expected state: "In Silico PCR" dialog has appered
            QWidget* wizard = GTWidget::getActiveModalWidget(os);
            GTWidget::click(os, wizard);

            // 3. Select "_common_data/cmdline/pcr/pcr_check_ambiguous.seq"
            GTUtilsWizard::setInputFiles(os, {{testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous.seq"}});

            // 4. Push "Next"
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            // 5. Push "Next"
            GTUtilsWizard::setParameter(os, "Primers URL", testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous_primers.fa");
            GTUtilsWizard::setParameter(os, "Mismatches", 0);

            // 6. Push "Next"
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

            // 7. Push "Next" twice
            GTUtilsWizard::clickButton(os, GTUtilsWizard::Apply);
        }
    };

    // 2. Open the "In Silico PCR" sample
    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "In Silico PCR", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "In silico PCR");

    // 8. Set output to sandBoxDir + "result.gb"
    GTUtilsWorkflowDesigner::click(os, "Write Sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", sandBoxDir + "result.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    // 9. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: result.gb in the result files
    auto outputFiles = GTUtilsDashboard::getOutputFiles(os);
    CHECK_SET_ERR(outputFiles.contains("result.gb"), "No expected file: \"result.gb\"");

    // 10. Go to dashboard
    GTUtilsWorkflowDesigner::returnToWorkflow(os);

    // 11. Set "Use ambiguous bases", change output to sandBoxDir + "result_1.gb"
    GTUtilsWorkflowDesigner::click(os, "In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter(os, "Use ambiguous bases", false, GTUtilsWorkflowDesigner::valueType::comboValue);
    GTUtilsWorkflowDesigner::click(os, "Write Sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", sandBoxDir + "result_1.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    // 12. Run workflow
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected: result_1.gb is absent
    outputFiles = GTUtilsDashboard::getOutputFiles(os);
    CHECK_SET_ERR(outputFiles.size() == 1, "Unexpected PCR result exists");
}

GUI_TEST_CLASS_DEFINITION(test_0020) {
    // Open "samples/FASTA/human_T1.fa".
    // Open the PCR OP tab.
    // Enter the forward primer: AAAGGAAAAAATGCT.
    // Enter the reverse primer: AGCATTTTTTCCTTT.
    // Check temperatures
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::InSilicoPcr);
    GTUtilsOptionPanelSequenceView::setForwardPrimer(os, "AAAGTTGAAATGAAGGAAAA");
    GTUtilsOptionPanelSequenceView::setReversePrimer(os, "CGCTTCCTTCAGGAGCTCTTTTAGGG");

    auto detailsDirect = GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct);
    CHECK_SET_ERR(detailsDirect.contains("41.53"), "Unexpected direct primer temperature, expected: 41.53");

    auto detailsComplementary = GTUtilsPcr::getPrimerInfo(os, U2Strand::Complementary);
    CHECK_SET_ERR(detailsComplementary.contains("61.12"), "Unexpected complementary primer temperature, expected: 61.12");

    GTUtilsOptionPanelSequenceView::openInSilicoPcrMeltingTemperatureShowHideWidget(os);
    struct Steps {
        Steps(GTUtilsMeltingTemperature::Parameter _step, const QString& _stringValue, const QString& _directTemp, const QString& _complementaryTemp) :
            step(_step), stringValue(_stringValue), directTemp(_directTemp), complementaryTemp(_complementaryTemp) {}
        GTUtilsMeltingTemperature::Parameter step;
        QString stringValue;
        QString directTemp;
        QString complementaryTemp;
    };

    static const QList<Steps> steps = { Steps(GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3", "49.09", "65.14"),
    Steps(GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00", "49.12", "65.17"),
    Steps(GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00", "49.15", "65.2"),
    Steps(GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50", "43.47", "59.26"),
    Steps(GTUtilsMeltingTemperature::Parameter::DntpConc, "0", "48.2", "64.2"),
    Steps(GTUtilsMeltingTemperature::Parameter::DmsoConc, "1", "47.6", "63.6"),
    Steps(GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6", "46.6", "62.6"),
    Steps(GTUtilsMeltingTemperature::Parameter::FormamideConc, "1", "43.83", "59.97"),
    Steps(GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0", "58.34", "75.27"),
    Steps(GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2", "58.06", "76.78"),
    Steps(GTUtilsMeltingTemperature::Parameter::MaxLen, "19", "42.99", "61.87") };

    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters;
    for (const auto& step : qAsConst(steps)) {
        parameters.insert(step.step, step.stringValue);
        GTUtilsMeltingTemperature::setParameters(os, parameters, nullptr);
        detailsDirect = GTUtilsPcr::getPrimerInfo(os, U2Strand::Direct);
        CHECK_SET_ERR(detailsDirect.contains(step.directTemp), QString("Unexpected direct primer temperature, expected: %1").arg(step.directTemp));

        detailsComplementary = GTUtilsPcr::getPrimerInfo(os, U2Strand::Complementary);
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
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus& os) override {

            GTUtilsWizard::clickButton(os, GTUtilsWizard::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new WizardFiller(os, "In Silico PCR", new Scenario()));
    GTUtilsWorkflowDesigner::addSample(os, "In silico PCR");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous.seq");

    GTUtilsWorkflowDesigner::click(os, "In Silico PCR");
    GTUtilsWorkflowDesigner::setParameter(os, "Primers URL", testDir + "_common_data/cmdline/pcr/pcr_check_ambiguous_primers.fa", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);
    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters = {
        {GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3" },
        { GTUtilsMeltingTemperature::Parameter::DnaConc, "51.00" },
        { GTUtilsMeltingTemperature::Parameter::MonovalentConc, "51.00" },
        { GTUtilsMeltingTemperature::Parameter::DivalentConc, "0.50" },
        { GTUtilsMeltingTemperature::Parameter::DntpConc, "0" },
        { GTUtilsMeltingTemperature::Parameter::DmsoConc, "1" },
        { GTUtilsMeltingTemperature::Parameter::DmsoFactor, "1.6" },
        { GTUtilsMeltingTemperature::Parameter::FormamideConc, "1" },
        { GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0" },
        { GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2" },
        { GTUtilsMeltingTemperature::Parameter::MaxLen, "33"} };
    GTUtilsDialog::waitForDialog(os, new MeltingTemperatureSettingsDialogFiller(os, parameters));
    GTUtilsWorkflowDesigner::setParameter(os, "Temperature settings", "", GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::click(os, "Write Sequence");
    GTUtilsWorkflowDesigner::setParameter(os, "Output file", sandBoxDir + "result.gb", GTUtilsWorkflowDesigner::valueType::lineEditWithFileSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    auto outputFiles = GTUtilsDashboard::getOutputFiles(os);
    CHECK_SET_ERR(outputFiles.contains("result.gb"), "No expected file: \"result.gb\"");
}

}  // namespace GUITest_common_scenarios_in_silico_pcr
}  // namespace U2
