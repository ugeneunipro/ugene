/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GTTestsPcrPrimerDesignForDnaAssembly.h"

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsPcrPrimerDesignForDnaAssembly.h"
#include "GTUtilsProject.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "base_dialogs/MessageBoxFiller.h"
#include "primitives/GTLineEdit.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTTableView.h"
#include "primitives/GTTextEdit.h"
#include "primitives/GTWidget.h"

namespace U2 {
using namespace HI;

// Searches for a table with a given name and returns its current row count.
static int getRowCount(GUITestOpStatus &os, const QString &tableName) {
    return GTTableView::rowCount(os, GTWidget::findExactWidget<QTableView *>(os, tableName,
        GTUtilsSequenceView::getActiveSequenceViewWindow(os)));
}

// Checks if the annotation table has an annotation with a given name and if its region matches an expected one.
static void checkAnnotation(GUITestOpStatus &os, const QString &annotationName, const QString &expectedRegion) {
    QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, annotationName);
    CHECK_SET_ERR(expectedRegion == region, QString("Invalid region for '%1' annotation: expected '%2', current '%3'").
                                                arg(annotationName, expectedRegion, region))
}

namespace GUITest_common_scenarios_pcr_primer_design_tab {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Enter "AAACACA" in the "Filter" field.
    //     Expected: 2 sequences left.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::filterGeneratedSequences(os, "AAACACA");
    int currentRowCount = getRowCount(os, "twGeneratedSequences");
    CHECK_SET_ERR(currentRowCount == 2, QString("Number of sequences after filtration: expected 2, current %1").
                                            arg(currentRowCount))
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Select the first sequence in the "Choose generated sequences" table and click "Forward 5'".
    // Select the second sequence in the "Choose generated sequences" table and click "Forward 3'".
    //     Expected: forward user primer = AAACACAGAAACACCA.
    // Select the third sequence in the "Choose generated sequences" table and click "Reverse 5'".
    // Select the fourth sequence in the "Choose generated sequences" table and click "Reverse 3'".
    //     Expected: reverse user primer = AAACACCCAAACACCT.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    QWidget *sequence = GTUtilsSequenceView::getActiveSequenceViewWindow(os);

    GTUtilsPcrPrimerDesign::selectGeneratedSequence(os, 0);
    GTUtilsPcrPrimerDesign::addToUserPrimer(os, GTUtilsPcrPrimerDesign::UserPrimer::Forward5);
    GTUtilsPcrPrimerDesign::selectGeneratedSequence(os, 1);
    GTUtilsPcrPrimerDesign::addToUserPrimer(os, GTUtilsPcrPrimerDesign::UserPrimer::Forward3);
    GTLineEdit::checkText(os, "leForwardPrimer", sequence, "AAACACAGAAACACCA");

    GTUtilsPcrPrimerDesign::selectGeneratedSequence(os, 2);
    GTUtilsPcrPrimerDesign::addToUserPrimer(os, GTUtilsPcrPrimerDesign::UserPrimer::Reverse5);
    GTUtilsPcrPrimerDesign::selectGeneratedSequence(os, 3);
    GTUtilsPcrPrimerDesign::addToUserPrimer(os, GTUtilsPcrPrimerDesign::UserPrimer::Reverse3);
    GTLineEdit::checkText(os, "leReversePrimer", sequence, "AAACACCCAAACACCT");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Click "Primer search areas for insert" -> "Left area" -> "Select manually".
    // Select area 1-100.
    //     Expected: left area values have changed to 1-100.
    // Click "Primer search areas for insert" -> "Right area" -> "Select manually".
    // Select area 100-200.
    //     Expected: right area values have changed to 100-200.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    QWidget *sequence = GTUtilsSequenceView::getActiveSequenceViewWindow(os);

    int expectedStart = 1,
        expectedEnd = 100;
    GTUtilsPcrPrimerDesign::setOtherSequences(os, "");  // For scroll down.
    GTWidget::click(os, GTWidget::findToolButton(os, "tbLeftAreaSelectManually", sequence));
    GTUtilsSequenceView::selectSequenceRegion(os, expectedStart, expectedEnd);
    int currentStart = GTSpinBox::getValue(os, "sbLeftAreaStart", sequence),
        currentEnd = GTSpinBox::getValue(os, "sbLeftAreaEnd", sequence);
    CHECK_SET_ERR(expectedStart == currentStart && expectedEnd == currentEnd, QString("Left area spinbox: "
                                                                                      "expected %1-%2, current %3-%4")
                                                                                  .arg(expectedStart)
                                                                                  .arg(expectedEnd)
                                                                                  .arg(currentStart)
                                                                                  .arg(currentEnd))

    expectedStart = 100,
    expectedEnd = 200;
    GTWidget::click(os, GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequence));
    GTUtilsSequenceView::selectSequenceRegion(os, expectedStart, expectedEnd);
    currentStart = GTSpinBox::getValue(os, "sbRightAreaStart", sequence),
    currentEnd = GTSpinBox::getValue(os, "sbRightAreaEnd", sequence);
    CHECK_SET_ERR(expectedStart == currentStart && expectedEnd == currentEnd, QString("Right area spinbox: "
                                                                                      "expected %1-%2, current %3-%4")
                                                                                  .arg(expectedStart)
                                                                                  .arg(expectedEnd)
                                                                                  .arg(currentStart)
                                                                                  .arg(currentEnd))
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // Open _common_data/fasta/alphabet.fa.
    // Click the PCR Primer Design tab of the Options Panel.
    // Select Amino sequence.
    //    Expected: all settings on the tab are disabled, a warning is displayed.
    // Select Nucl sequence.
    //    Expected: all settings on the tab are enabled, no warning.
    QList<ADVSingleSequenceWidget *> seqWidgets = GTUtilsProject::openFileExpectSequences(os,
                                                  testDir + "_common_data/fasta/", "alphabet.fa", {"Amino", "Nucl"});
    GTUtilsOptionPanelSequenceView::toggleTab(os, GTUtilsOptionPanelSequenceView::PcrPrimerDesign);
    auto mainWidget = GTWidget::findWidget(os, "runPcrPrimerDesignWidget");
    auto warnLabel = GTWidget::findLabelByText(os, "Info: choose a nucleic sequence for running PCR Primer Design").
        first();

    GTWidget::click(os, seqWidgets.first());
    GTWidget::checkEnabled(os, mainWidget, false);
    CHECK_SET_ERR(warnLabel->isVisible(), "Expected: warning label visible")

    GTWidget::click(os, seqWidgets.last());
    GTWidget::checkEnabled(os, mainWidget);
    CHECK_SET_ERR(!warnLabel->isVisible(), "Expected: warning label doesn't displayed")
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    using Op = GTUtilsOptionPanelSequenceView;
    auto checkExpectedTabs = [&os](const QSet<Op::Tabs> &tabs) {
        // Tabs that should be.
        for (auto tab : qAsConst(tabs)) {
            GTWidget::findWidget(os, Op::tabsNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow(os));
        }
        // Tabs that shouldn't be.
        for (auto tab : Op::tabsNames.keys().toSet() - tabs) {
            auto opWidget = GTWidget::findWidget(os, Op::tabsNames[tab],
                GTUtilsSequenceView::getActiveSequenceViewWindow(os), GTGlobals::FindOptions(false));
            CHECK_SET_ERR(opWidget == nullptr, QString("Expected: there is no %1 tab").arg(Op::tabsNames[tab]))
        }
    };
    // DNA: _common_data/fasta/fa1.fa.
    //    Expected: all option panel tabs (6) are present.
    // RNA: _common_data/fasta/RNA_1_seq.fa.
    //    Expected: all option panel tabs without PCR Primer Design are present.
    // Amino: _common_data/fasta/AMINO.fa.
    //    Expected: yes: Search, Highlight, Statistics, no: PCR Primer Design, In Silico, Circular.
    // All alphabets: _common_data/fasta/all_and_raw_alphabets.fa.
    //    Expected: all option panel tabs.
    // 2 RNA: _common_data/fasta/RNA.fa.
    //    Expected: all option panel tabs without PCR Primer Design.
    // Aminos: _common_data/fasta/amino_multy_ext.fa.
    //    Expected: yes: Search, Highlight, Statistics, no: PCR Primer Design, In Silico, Circular.

    QSet<Op::Tabs> all = Op::tabsNames.keys().toSet();
    QSet<Op::Tabs> withoutPcr = all - QSet<Op::Tabs> {Op::PcrPrimerDesign};
    QSet<Op::Tabs> three = {Op::Search, Op::AnnotationsHighlighting, Op::Statistics};

    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/fasta/fa1.fa", "fasta file part 1");
    checkExpectedTabs(all);

    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/fasta/RNA_1_seq.fa", "AB000263");
    checkExpectedTabs(withoutPcr);

    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/fasta/AMINO.fa", "AMINO263");
    checkExpectedTabs(three);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTUtilsProject::openMultiSequenceFileAsSequences(os, testDir + "_common_data/fasta/all_and_raw_alphabets.fa");
    checkExpectedTabs(all);

    GTUtilsProject::openMultiSequenceFileAsSequences(os, testDir + "_common_data/fasta/RNA.fa");
    checkExpectedTabs(withoutPcr);

    GTUtilsProject::openMultiSequenceFileAsSequences(os, testDir + "_common_data/fasta/amino_multy_ext.fa");
    checkExpectedTabs(three);
}

}  // namespace GUITest_common_scenarios_pcr_primer_design_tab

namespace GUITest_common_scenarios_pcr_primer_design_algo {

GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Click "Start" and wait the task for finish.
    //     Expected: result table is empty.
    // Open the task report.
    //     Expected: "There are no primers that meet the specified parameters" in the report.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const int currentRowCount = getRowCount(os, "productsTable");
    CHECK_SET_ERR(currentRowCount == 0, QString("Result table size: expected 0, current %1").arg(currentRowCount))
    GTUtilsNotifications::checkNotificationReportText(os, "There are no primers that meet the specified parameters");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set "Parameters of priming sequences" -> "Melting point" -> Minimum = 48.
    // Click "Start" and wait the task for finish.
    //     Expected: in the table 8 results:
    //               A Forward   67-87
    //               A Reverse   337-357
    //               B1 Forward  67-87
    //               B1 Reverse  337-357
    //               B2 Forward  48-70
    //               B2 Reverse  354-376
    //               B3 Forward  44-66
    //               B3 Reverse  358-380
    //     Expected: annotation table contains annotations with similar names and regions.
    // Open the task report.
    //     Expected: the report contains 8 sequences:
    //               A Forward   AATGGGCACAAATTTTCTGTC
    //               A Reverse   AAGGGTATCACCTTCAAACTT
    //               B1 Forward  AATGGGCACAAATTTTCTGTC
    //               B1 Reverse  AAGGGTATCACCTTCAAACTT
    //               B2 Forward  TGAATTAGATGGTGATGTTAATG
    //               B2 Reverse  TTAACTCGATTCTATTAACAAGG
    //               B3 Forward  TTGTTGAATTAGATGGTGATGTT
    //               B3 Reverse  CCTTTTAACTCGATTCTATTAAC
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::ParametersOfPrimingSequences params;
    params.meltingPoint.minValue = 48;
    GTUtilsPcrPrimerDesign::setParametersOfPrimingSequences(os, params);
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const int currentRowCount = getRowCount(os, "productsTable");
    CHECK_SET_ERR(currentRowCount == 8, QString("Result table size: expected 8, current %1").arg(currentRowCount))
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 0, "A Forward", {67, 87});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 1, "A Reverse", {337, 357});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 2, "B1 Forward", {67, 87});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 3, "B1 Reverse", {337, 357});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 4, "B2 Forward", {48, 70});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 5, "B2 Reverse", {354, 376});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 6, "B3 Forward", {44, 66});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 7, "B3 Reverse", {358, 380});

    checkAnnotation(os, "A Forward", "67..87");
    checkAnnotation(os, "A Reverse", "complement(337..357)");
    checkAnnotation(os, "B1 Forward", "67..87");
    checkAnnotation(os, "B1 Reverse", "complement(337..357)");
    checkAnnotation(os, "B2 Forward", "48..70");
    checkAnnotation(os, "B2 Reverse", "complement(354..376)");
    checkAnnotation(os, "B3 Forward", "44..66");
    checkAnnotation(os, "B3 Reverse", "complement(358..380)");

    GTUtilsNotifications::checkNotificationReportText(os, {"A Forward", "AATGGGCACAAATTTTCTGTC",
                                                           "A Reverse", "AAGGGTATCACCTTCAAACTT",
                                                           "B1 Forward", "AATGGGCACAAATTTTCTGTC",
                                                           "B1 Reverse", "AAGGGTATCACCTTCAAACTT",
                                                           "B2 Forward", "TGAATTAGATGGTGATGTTAATG",
                                                           "B2 Reverse", "TTAACTCGATTCTATTAACAAGG",
                                                           "B3 Forward", "TTGTTGAATTAGATGGTGATGTT",
                                                           "B3 Reverse", "CCTTTTAACTCGATTCTATTAAC"});
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set "Parameters of priming sequences" -> "Melting point" -> Minimum = 48.
    // Set common_data/pcr_primer_design/backbone.fa as "Open the backbone sequence".
    // Click "Start" and wait the task for finish.
    //     Expected: in the table 6 results:
    //               A Forward   59-80
    //               A Reverse   337-357
    //               B1 Forward  58-78
    //               B1 Reverse  346-366
    //               B2 Forward  39-61
    //               B2 Reverse  363-385
    //     Expected: annotation table contains annotations with similar names and regions.
    // Open the task report.
    //     Expected: the report contains 6 sequences, each sequence contains underlined "GACAGA":
    //               A Forward   {GACAGA}GTGATGTTAATGGGCACAAATT
    //               A Reverse   AAGGGTATCACCTTCAAACTT{GACAGA}
    //               B1 Forward  {GACAGA}GGTGATGTTAATGGGCACAAA
    //               B1 Reverse  TCTATTAACAAGGGTATCACC{GACAGA}
    //               B2 Forward  {GACAGA}AATTCTTGTTGAATTAGATGGTG
    //               B2 Reverse  CAATACCTTTTAACTCGATTCTA{GACAGA}
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::ParametersOfPrimingSequences params;
    params.meltingPoint.minValue = 48;
    GTUtilsPcrPrimerDesign::setParametersOfPrimingSequences(os, params);
    GTUtilsPcrPrimerDesign::setBackbone(os, testDir + "_common_data/pcr_primer_design/backbone.fa");
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const int currentRowCount = getRowCount(os, "productsTable");
    CHECK_SET_ERR(currentRowCount == 6, QString("Result table size: expected 6, current %1").arg(currentRowCount))
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 0, "A Forward", {59, 80});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 1, "A Reverse", {337, 357});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 2, "B1 Forward", {58, 78});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 3, "B1 Reverse", {346, 366});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 4, "B2 Forward", {39, 61});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 5, "B2 Reverse", {363, 385});

    checkAnnotation(os, "A Forward", "59..80");
    checkAnnotation(os, "A Reverse", "complement(337..357)");
    checkAnnotation(os, "B1 Forward", "58..78");
    checkAnnotation(os, "B1 Reverse", "complement(346..366)");
    checkAnnotation(os, "B2 Forward", "39..61");
    checkAnnotation(os, "B2 Reverse", "complement(363..385)");

    GTUtilsNotifications::checkNotificationReportText(os, {
        "A Forward",  "<span style=\" text-decoration: underline;\">GACAGA</span>"
                      "<span style=\" font-weight:600;\">GTGATGTTAATGGGCACAAATT</span>",
        "A Reverse",  "<span style=\" font-weight:600;\">AAGGGTATCACCTTCAAACTT</span>"
                      "<span style=\" text-decoration: underline;\">GACAGA</span>",
        "B1 Forward", "<span style=\" text-decoration: underline;\">GACAGA</span>"
                      "<span style=\" font-weight:600;\">GGTGATGTTAATGGGCACAAA</span>",
        "B1 Reverse", "<span style=\" font-weight:600;\">TCTATTAACAAGGGTATCACC</span>"
                      "<span style=\" text-decoration: underline;\">GACAGA</span>",
        "B2 Forward", "<span style=\" text-decoration: underline;\">GACAGA</span>"
                      "<span style=\" font-weight:600;\">AATTCTTGTTGAATTAGATGGTG</span>",
        "B2 Reverse", "<span style=\" font-weight:600;\">CAATACCTTTTAACTCGATTCTA</span>"
                      "<span style=\" text-decoration: underline;\">GACAGA</span>"});
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    class CheckTextBadBackboneDialog : public CustomScenario {
        void run(GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTWidget::findLabelByText(os, "The unwanted structures have been found in the following backbone sequence "
                                          "candidate:",
                                      dialog);
            GTTextEdit::containsString(os, GTWidget::findTextEdit(os, "unwantedTextEdit", dialog),
                                       "Delta G: -20.7 kcal/mole Base Pairs: 12 Melting temperature: 36°C");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::No);
        }
    };
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set common_data/pcr_primer_design/backbone_bad.fa as "Open the backbone sequence".
    // Set "Insert to backbone bearings" -> "5' backbone length" = 12.
    // Click "Start".
    //     Expected: the "The unwanted structures have been found in the following backbone sequence candidate" dialog
    //               has appeared.
    //     Expected: the bottom text field contains "Delta G: -20.7 kcal/mole Base Pairs: 12 Melting temperature: 36°C".
    // Click "No" and wait the task for finish.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::setBackbone(os, testDir + "_common_data/pcr_primer_design/backbone_bad.fa");
    GTUtilsPcrPrimerDesign::InsertToBackboneBearings params;
    params.backbone5Len = 12;
    GTUtilsPcrPrimerDesign::configureInsertToBackboneBearings(os, params);
    GTUtilsDialog::waitForDialog(os, new GTUtilsPcrPrimerDesign::BadBackboneFiller(os, new CheckTextBadBackboneDialog));
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    class CheckTextBadBackboneDialog : public CustomScenario {
        void run(GUITestOpStatus &os) {
            QWidget *dialog = GTWidget::getActiveModalWidget(os);
            GTWidget::findLabelByText(os, "The unwanted structures have been found in the following backbone sequence "
                                          "candidate:",
                                      dialog);
            GTTextEdit::containsString(os, GTWidget::findTextEdit(os, "unwantedTextEdit", dialog),
                                       "Delta G: -11.7 kcal/mole Base Pairs: 6 Melting temperature: 20°C");
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::No);
        }
    };
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set common_data/pcr_primer_design/backbone_bad.fa as "Open the backbone sequence".
    // Click "Insert to backbone bearings" -> "5' insert to 3' backbone".
    // Set "Insert to backbone bearings" -> "5' backbone length" = 12.
    // Click "Start".
    //     Expected: the "The unwanted structures have been found in the following backbone sequence candidate" dialog
    //               has appeared.
    //     Expected: the bottom text field contains "Delta G: -11.7 kcal/mole Base Pairs: 6 Melting temperature: 20°C".
    // Click "No" and wait the task for finish.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::setBackbone(os, testDir + "_common_data/pcr_primer_design/backbone_bad.fa");
    GTUtilsPcrPrimerDesign::InsertToBackboneBearings params;
    params.insertTo = GTUtilsPcrPrimerDesign::InsertToBackboneBearings::InsertTo::Backbone3;
    params.backbone3Len = 12;
    GTUtilsPcrPrimerDesign::configureInsertToBackboneBearings(os, params);
    GTUtilsDialog::waitForDialog(os, new GTUtilsPcrPrimerDesign::BadBackboneFiller(os, new CheckTextBadBackboneDialog));
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set "Forward user primer" as "AGGAGAAGA".
    // Click "Start".
    //     Expected: there is a line in the log containing the phrase "No reverse user primer".
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::setUserPrimer(os, "AGGAGAAGA", U2Strand::Direct);
    GTLogTracer lt("No reverse user primer");
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsLog::checkContainsMessage(os, lt);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set "Forward user primer" as "GCAGCCAGAGAGA".
    // Set "Reverse user primer" as "TCTCTCTGGCTGC".
    // Click "Start" and wait the task for finish.
    // Open the task report.
    //     Expected: the report contains "Delta G: -24.5 kcal/mole Base Pairs: 13 Melting temperature: 42°C".
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::setUserPrimer(os, "GCAGCCAGAGAGA", U2Strand::Direct);
    GTUtilsPcrPrimerDesign::setUserPrimer(os, "TCTCTCTGGCTGC", U2Strand::Complementary);
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsNotifications::checkNotificationReportText(os, QString("<span style=\" font-weight:600;\">Delta</span> G: "
        "-24.5 kcal/mole <span style=\" font-weight:600;\">Base Pairs:</span> 13 <span style=\" font-weight:600;\">"
        "Melting temperature:</span> 42%1C").arg(QChar(0260)));
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Set "Parameters of priming sequences" -> "Melting point" -> Minimum = 51.
    // Set common_data/pcr_primer_design/other_in_pcr.fa as "Other sequences in PCR reaction".
    // Click "Start" and wait the task for finish.
    //     Expected: in the table 2 results:
    //               A Forward  29-50
    //               A Reverse  337-358
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    GTUtilsPcrPrimerDesign::ParametersOfPrimingSequences params;
    params.meltingPoint.minValue = 51;
    GTUtilsPcrPrimerDesign::setParametersOfPrimingSequences(os, params);
    GTUtilsPcrPrimerDesign::setOtherSequences(os, testDir + "_common_data/pcr_primer_design/other_in_pcr.fa");
    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    const int currentRowCount = getRowCount(os, "productsTable");
    CHECK_SET_ERR(currentRowCount == 2, QString("Result table size: expected 2, current %1").arg(currentRowCount))
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 0, "A Forward", {29, 50});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 1, "A Reverse", {337, 358});
}

}  // namespace GUITest_common_scenarios_pcr_primer_design_algo

}  // namespace U2
