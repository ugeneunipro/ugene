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
#include "primitives/GTMenu.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTTableView.h"
#include "primitives/GTTextEdit.h"
#include "primitives/GTWidget.h"
#include "runnables/ugene/corelibs/U2Gui/RangeSelectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/RemovePartFromSequenceDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ReplaceSubsequenceDialogFiller.h"
#include "utils/GTKeyboardUtils.h"

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
    CHECK_SET_ERR(expectedRegion == region, QString("Invalid region for '%1' annotation: expected '%2', current '%3'")
        .arg(annotationName, expectedRegion, region))
}

// Checks that the values of the area spinboxes are as expected. Specify area position as name (left/right).
static void checkAreaSpinboxValues(GUITestOpStatus &os,
                                   const QString &name,
                                   int expectedStart,
                                   int expectedEnd,
                                   int currentStart,
                                   int currentEnd) {
    CHECK_SET_ERR(expectedStart == currentStart && expectedEnd == currentEnd,
                  QString("%1 area spinbox: expected %2-%3, current %4-%5")
                      .arg(name)
                      .arg(expectedStart)
                      .arg(expectedEnd)
                      .arg(currentStart)
                      .arg(currentEnd))
}

// Finds left area spinboxes and checks for expected values.
static void checkLeftAreaSpinboxValues(GUITestOpStatus &os, int expectedStart, int expectedEnd, QWidget *parent) {
    checkAreaSpinboxValues(os,
                           "Left",
                           expectedStart,
                           expectedEnd,
                           GTSpinBox::getValue(os, "sbLeftAreaStart", parent),
                           GTSpinBox::getValue(os, "sbLeftAreaEnd", parent));
}

// Finds right area spinboxes and checks for expected values.
static void checkRightAreaSpinboxValues(GUITestOpStatus &os, int expectedStart, int expectedEnd, QWidget *parent) {
    checkAreaSpinboxValues(os,
                           "Right",
                           expectedStart,
                           expectedEnd,
                           GTSpinBox::getValue(os, "sbRightAreaStart", parent),
                           GTSpinBox::getValue(os, "sbRightAreaEnd", parent));
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
    CHECK_SET_ERR(currentRowCount == 2, QString("Number of sequences after filtration: expected 2, current %1")
        .arg(currentRowCount))
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Select the first sequence in the "Choose generated sequences" table and click "Forward 5'".
    // Select the second sequence in the "Choose generated sequences" table and click "Forward 3'".
    //     Expected: forward user primer = AAACACAGAAACACCA.
    // Select the third sequence in the "Choose generated sequences" table and click "Reverse 5'".
    // Select the fourth sequence in the "Choose generated sequences" table and click "Reverse 3'".
    //     Expected: reverse user primer = AAACACCTAAACACCC.
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
    GTLineEdit::checkText(os, "leReversePrimer", sequence, "AAACACCTAAACACCC");
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
    checkLeftAreaSpinboxValues(os, expectedStart, expectedEnd, sequence);

    expectedStart = 100,
    expectedEnd = 200;
    GTWidget::click(os, GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequence));
    GTUtilsSequenceView::selectSequenceRegion(os, expectedStart, expectedEnd);
    checkRightAreaSpinboxValues(os, expectedStart, expectedEnd, sequence);
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
    auto warnLabel = GTWidget::findLabelByText(os, "Info: choose a nucleic sequence for running PCR Primer Design")
        .first();

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

GUI_TEST_CLASS_DEFINITION(test_0006) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Click the PCR Primer Design tab of the Options Panel.
    // Click "Primer search areas for insert" -> "Left area" -> "Select manually".
    // Reopen tab.
    //     Expected: left "Select manually" not pressed.
    // Select region 2-3.
    //     Expected: left area spinboxes not changed.
    //
    // Click "Primer search areas for insert" -> "Right area" -> "Select manually".
    // Reopen tab.
    //     Expected: right "Select manually" not pressed.
    // Select region 2-3.
    //     Expected: right area spinboxes not changed.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    QWidget *sequence = GTUtilsSequenceView::getActiveSequenceViewWindow(os);

    GTUtilsPcrPrimerDesign::setOtherSequences(os, "");  // For scroll down.
    GTWidget::click(os, GTWidget::findToolButton(os, "tbLeftAreaSelectManually", sequence));
    GTUtilsOptionPanelSequenceView::closeTab(os, GTUtilsOptionPanelSequenceView::PcrPrimerDesign);
    GTUtilsPcrPrimerDesign::openTab(os);
    CHECK_SET_ERR(!GTWidget::findToolButton(os, "tbLeftAreaSelectManually", sequence)->isChecked(),
                  "Expected: left 'Select manually' not pressed")

    QSpinBox *start = GTWidget::findSpinBox(os, "sbLeftAreaStart", sequence),
             *end = GTWidget::findSpinBox(os, "sbLeftAreaEnd", sequence);
    int expectedStart = GTSpinBox::getValue(os, start),
        expectedEnd = GTSpinBox::getValue(os, end);
    GTUtilsSequenceView::selectSequenceRegion(os, 2, 3);
    int currentStart = GTSpinBox::getValue(os, start),
        currentEnd = GTSpinBox::getValue(os, end);
    checkAreaSpinboxValues(os, "Left", expectedStart, expectedEnd, currentStart, currentEnd);

    GTUtilsPcrPrimerDesign::setOtherSequences(os, "");  // For scroll down.
    GTWidget::click(os, GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequence));
    GTUtilsOptionPanelSequenceView::closeTab(os, GTUtilsOptionPanelSequenceView::PcrPrimerDesign);
    GTUtilsPcrPrimerDesign::openTab(os);
    CHECK_SET_ERR(!GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequence)->isChecked(),
                  "Expected: right 'Select manually' not pressed")

    start = GTWidget::findSpinBox(os, "sbRightAreaStart", sequence);
    end = GTWidget::findSpinBox(os, "sbRightAreaEnd", sequence);
    expectedStart = GTSpinBox::getValue(os, start);
    expectedEnd = GTSpinBox::getValue(os, end);
    GTUtilsSequenceView::selectSequenceRegion(os, 3, 4);
    currentStart = GTSpinBox::getValue(os, start),
    currentEnd = GTSpinBox::getValue(os, end);
    checkAreaSpinboxValues(os, "Right", expectedStart, expectedEnd, currentStart, currentEnd);
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // Open common_data/pcr_primer_design/gfp.fa.
    // Open the PCR Primer Design tab.
    // Click "Primer search areas for insert" -> "Left area" -> "Select manually".
    //     Expected: region 1-71 is selected in the Sequence View.
    // Select area 1-1.
    //     Expected: left area values have changed to 1-1.
    // Click "Zoom to Selection".
    //     Expected state: no crash.

    // Click "Primer search areas for insert" -> "Right area" -> "Select manually".
    //     Expected: region 355-426 is selected in the Sequence View.
    // Select area 717-717.
    //     Expected: right area values have changed to 717-717.
    // Click "Start" and wait the task for finish.
    // Open the task report.
    //     Expected: "There are no primers that meet the specified parameters" in the report.
    GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
    GTUtilsPcrPrimerDesign::openTab(os);
    auto sequenceWidget = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto selectManually = GTWidget::findToolButton(os, "tbLeftAreaSelectManually", sequenceWidget);
    GTUtilsPcrPrimerDesign::setOtherSequences(os, "");  // For scroll down.

    GTWidget::click(os, selectManually);
    QVector<U2Region> selection = GTUtilsSequenceView::getSelection(os);
    int regionNumber = selection.size();
    CHECK_SET_ERR(regionNumber == 1, QString("Left. Expected: one region is selected, current: %1").arg(regionNumber))
    U2Region region = selection.first();
    CHECK_SET_ERR(region == U2Region(0, 71),
                  QString("Selected region: expected 1-71, current: %1").arg(region.toString()))

    int expectedStart = 1,
        expectedEnd = 1;
    GTUtilsSequenceView::selectSequenceRegion(os, expectedStart, expectedEnd);
    checkLeftAreaSpinboxValues(os, expectedStart, expectedEnd, sequenceWidget);

    auto zoomButton = GTWidget::findButtonByText(os, "Zoom to Selection", sequenceWidget);
    GTWidget::click(os, zoomButton);

    selectManually = GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequenceWidget);
    GTWidget::click(os, selectManually);
    selection = GTUtilsSequenceView::getSelection(os);
    regionNumber = selection.size();
    CHECK_SET_ERR(regionNumber == 1, QString("Right. Expected: one region is selected, current: %1").arg(regionNumber))
    region = selection.first();
    CHECK_SET_ERR(region == U2Region(354, 72),
                  QString("Selected region: expected 355-426, current: %1").arg(region.toString()))

    expectedStart = 717;
    expectedEnd = 717;
    GTUtilsSequenceView::selectSequenceRegion(os, expectedStart, expectedEnd);
    checkRightAreaSpinboxValues(os, expectedStart, expectedEnd, sequenceWidget);

    GTUtilsPcrPrimerDesign::clickStart(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsNotifications::checkNotificationReportText(os, "There are no primers that meet the specified parameters");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // Open common_data/fasta/random_primers2.fa.
    // Open the PCR Primer Design tab.
    //     Expected: area ranges 1:1 - 1:19 (for left area spinboxes),
    //                           1:6 - 5:19 (for right).
    // For convenience, set the areas to 6-10.
    //     Expected: area ranges 1:10 - 6:19.

    // ---- Increase/Decrease ----
    // Increment the values of each spinbox one by one by 1.
    //     Expected: area ranges are increased by 1.
    // Decrement the values of each spinbox one by one by 1.
    //     Expected: region ranges are decreased by 1.

    // ---- Select manually ----
    // Click "Primer search areas for insert" -> "Left area" -> "Select manually".
    // Select area to the left of the current one, for example 3-5.
    //     Expected: left area ranges are now 1:5 - 3:19.
    // Click "Primer search areas for insert" -> "Right area" -> "Select manually".
    // Select area to the right of the current until to the end of the sequence, for example 17-19.
    //     Expected: right area ranges are now 1:19 - 17:19.

    // ---- Changes/edit sequence ----
    // Click on primer5.
    //     Expected: area ranges are now 1:5  -  3:16,
    //                                   1:16 - 16:16.
    // Remove subsequence.
    //     Expected: area ranges have changed.
    // Insert subsequence.
    //     Expected: area ranges have changed.

    GTUtilsProject::openMultiSequenceFileAsSequences(os, testDir + "_common_data/fasta/random_primers2.fa");
    GTUtilsPcrPrimerDesign::openTab(os);
    QWidget *sequenceWidget = GTUtilsSequenceView::getActiveSequenceViewWindow(os);
    auto leftStart = GTWidget::findSpinBox(os, "sbLeftAreaStart", sequenceWidget),
         leftEnd = GTWidget::findSpinBox(os, "sbLeftAreaEnd", sequenceWidget),
         rightStart = GTWidget::findSpinBox(os, "sbRightAreaStart", sequenceWidget),
         rightEnd = GTWidget::findSpinBox(os, "sbRightAreaEnd", sequenceWidget);
    auto checkLeftRanges = [&os, leftStart, leftEnd](int startMax, int endMin, int endMax) {
        GTSpinBox::checkLimits(os, leftStart, 1, startMax);
        GTSpinBox::checkLimits(os, leftEnd, endMin, endMax);
    };
    auto checkRightRanges = [&os, rightStart, rightEnd](int startMax, int endMin, int endMax) {
        GTSpinBox::checkLimits(os, rightStart, 1, startMax);
        GTSpinBox::checkLimits(os, rightEnd, endMin, endMax);
    };

    checkLeftRanges(1, 1, 19);
    checkRightRanges(6, 5, 19);
    GTUtilsPcrPrimerDesign::setSearchArea(os, {{6, 10}}, GTUtilsPcrPrimerDesign::AreaType::Left);
    GTUtilsPcrPrimerDesign::setSearchArea(os, {{6, 10}}, GTUtilsPcrPrimerDesign::AreaType::Right);
    GTWidget::click(os, leftStart);  // Change focus.
    checkLeftRanges(10, 6, 19);
    checkRightRanges(10, 6, 19);

    // Increase/Decrease.
    {
        auto setLeftStart = [&os, leftStart, leftEnd](int start) {
            GTSpinBox::setValue(os, leftStart, start);
            GTWidget::click(os, leftEnd);  // Change focus. Similarly in other lambdas.
        };
        auto setLeftEnd = [&os, leftStart, leftEnd](int end) {
            GTSpinBox::setValue(os, leftEnd, end);
            GTWidget::click(os, leftStart);
        };
        auto setRightStart = [&os, rightStart, rightEnd](int start) {
            GTSpinBox::setValue(os, rightStart, start);
            GTWidget::click(os, rightEnd);
        };
        auto setRightEnd = [&os, rightStart, rightEnd](int end) {
            GTSpinBox::setValue(os, rightEnd, end);
            GTWidget::click(os, rightStart);
        };

        setLeftStart(7);
        checkLeftRanges(10, 7, 19);
        setLeftEnd(11);
        checkLeftRanges(11, 7, 19);
        setRightEnd(11);
        checkRightRanges(11, 6, 19);
        setRightStart(7);
        checkRightRanges(11, 7, 19);

        setLeftStart(6);
        checkLeftRanges(11, 6, 19);
        setLeftEnd(10);
        checkLeftRanges(10, 6, 19);
        setRightEnd(10);
        checkRightRanges(10, 7, 19);
        setRightStart(6);
        checkRightRanges(10, 6, 19);
    }

    // Select manually.
    GTWidget::click(os, GTWidget::findToolButton(os, "tbLeftAreaSelectManually", sequenceWidget));
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 3, 5));
    GTKeyboardUtils::selectAll();
    checkLeftRanges(5, 3, 19);

    GTWidget::click(os, GTWidget::findToolButton(os, "tbRightAreaSelectManually", sequenceWidget));
    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 17, 19));
    GTKeyboardUtils::selectAll();
    checkRightRanges(19, 17, 19);

    // Changes/edit sequence.
    GTWidget::click(os, GTUtilsSequenceView::getPanOrDetView(os));
    checkLeftRanges(5, 3, 16);
    checkRightRanges(16, 16, 16);

    GTUtilsDialog::waitForDialog(os, new RemovePartFromSequenceDialogFiller(os, "1..5"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Remove subsequence..."});
    checkLeftRanges(5, 3, 11);
    checkRightRanges(11, 11, 11);

    GTUtilsDialog::waitForDialog(os, new SelectSequenceRegionDialogFiller(os, 1, 1));
    GTKeyboardUtils::selectAll();
    GTUtilsDialog::waitForDialog(os, new ReplaceSubsequenceDialogFiller(os, "AAAAAAAAAA"));
    GTMenu::clickMainMenuItem(os, {"Actions", "Edit", "Replace subsequence..."});
    checkLeftRanges(5, 3, 20);
    checkRightRanges(11, 11, 20);
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
    int currentRowCount = getRowCount(os, "productsTable");
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

    int currentRowCount = getRowCount(os, "productsTable");
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

    int currentRowCount = getRowCount(os, "productsTable");
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
    // Set "Backbone length" -> "5' backbone length" = 12.
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
    // Set "Backbone length" -> "3' backbone length" = 12.
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
                                                                  "Melting temperature:</span> 42%1C")
                                                              .arg(QChar(0260)));
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
    int currentRowCount = getRowCount(os, "productsTable");
    CHECK_SET_ERR(currentRowCount == 2, QString("Result table size: expected 2, current %1").arg(currentRowCount))
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 0, "A Forward", {29, 50});
    GTUtilsPcrPrimerDesign::checkEntryInResultsTable(os, 1, "A Reverse", {337, 358});
}

}  // namespace GUITest_common_scenarios_pcr_primer_design_algo

}  // namespace U2
