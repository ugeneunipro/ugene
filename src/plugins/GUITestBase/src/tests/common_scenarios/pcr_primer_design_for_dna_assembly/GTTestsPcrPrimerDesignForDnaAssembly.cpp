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

#include <QTableWidget>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsMdi.h"
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
#include "primitives/GTWidget.h"

namespace U2 {
namespace GUITest_common_scenarios_pcr_primer_design_for_dna_assembly {
using namespace HI;
using GTUtilsPcrPrimer = GTUtilsPcrPrimerDesignForDnaAssembly;

GUI_TEST_CLASS_DEFINITION(test_0001) {
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::filterGeneratedSequences(os, "AAACACA");
	CHECK_SET_ERR(GTTableView::rowCount(os, GTUtilsPcrPrimer::getGeneratedSequenceTableWgt(os)) == 2, "The number of "
		"generated sequences is not 2")
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::selectGeneratedSequence(os, 0);
	GTUtilsPcrPrimer::addUserPrimerEnd(os, GTUtilsPcrPrimer::UserPrimerEndButton::Forward5);
	GTUtilsPcrPrimer::selectGeneratedSequence(os, 1);
	GTUtilsPcrPrimer::addUserPrimerEnd(os, GTUtilsPcrPrimer::UserPrimerEndButton::Forward3);
	GTLineEdit::checkText(os, GTUtilsPcrPrimer::getUserPrimer(os, U2Strand::Direct), "AAACACAGAAACACCA");

	GTUtilsPcrPrimer::selectGeneratedSequence(os, 2);
	GTUtilsPcrPrimer::addUserPrimerEnd(os, GTUtilsPcrPrimer::UserPrimerEndButton::Reverse5);
	GTUtilsPcrPrimer::selectGeneratedSequence(os, 3);
	GTUtilsPcrPrimer::addUserPrimerEnd(os, GTUtilsPcrPrimer::UserPrimerEndButton::Reverse3);
	GTLineEdit::checkText(os, GTUtilsPcrPrimer::getUserPrimer(os, U2Strand::Complementary), "AAACACCCAAACACCT");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::selectGeneratedSequence(os, 0);
	GTUtilsPcrPrimer::setPrimerSearchArea(os, true, { 1, 100 }, true);
	QPair<QSpinBox*, QSpinBox*> spinboxes = GTUtilsPcrPrimer::getAreaSpinboxes(os, true);
	int min = GTSpinBox::getValue(os, spinboxes.first),
	    max = GTSpinBox::getValue(os, spinboxes.second);
	CHECK_SET_ERR(min == 1 && max == 100, QString("Left area spinbox: expect 1-100, current %1-%2").arg(min).arg(max))

	GTUtilsPcrPrimer::setPrimerSearchArea(os, false, { 100, 200 }, true);
	spinboxes = GTUtilsPcrPrimer::getAreaSpinboxes(os, false);
	min = GTSpinBox::getValue(os, spinboxes.first);
	max = GTSpinBox::getValue(os, spinboxes.second);
	CHECK_SET_ERR(min == 100 && max == 200, QString("Right area spinbox: expect 100-200, current %1-%2").arg(min).
		                                                                                                 arg(max))
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
	// Open _common_data/fasta/alphabet.fa.
	// Click the PCR Primer Design tab of the Options Panel.
	// Select Amino sequence.
	//    Expected: all settings on the tab are disabled, a warning is displayed.
	// Select Nucl sequence.
	//    Expected: all settings on the tab are enabled, no warning.
	QList<ADVSingleSequenceWidget*> seqWidgets = GTUtilsProject::openFileExpectSequences(os,
		testDir + "_common_data/fasta/", "alphabet.fa", { "Amino", "Nucl" });
	GTUtilsOptionPanelSequenceView::toggleTab(os, GTUtilsOptionPanelSequenceView::PcrPrimerDesign);
	auto mainWidget = GTWidget::findWidget(os, "runPcrPrimerDesignWidget");
	auto warnLabel =
		GTWidget::findLabelByText(os, "Info: choose a nucleic sequence for running PCR Primer Design").first();

	GTWidget::click(os, seqWidgets.first());
	GTWidget::checkEnabled(os, mainWidget, false);
	CHECK_SET_ERR(warnLabel->isVisible(), "Expected: warning label visible")

	GTWidget::click(os, seqWidgets.last());
	GTWidget::checkEnabled(os, mainWidget);
	CHECK_SET_ERR(!warnLabel->isVisible(), "Expected: warning label doesn't displayed")
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
	using Op = GTUtilsOptionPanelSequenceView;
	auto checkExpectedTabs = [&os](const QSet<Op::Tabs>& tabs) {
		// Tabs that should be.
		for (const auto tab : qAsConst(tabs)) {
			GTWidget::findWidget(os, Op::tabsNames[tab], GTUtilsSequenceView::getActiveSequenceViewWindow(os));
		}
		// Tabs that shouldn't be.
		for (const auto tab : Op::tabsNames.keys().toSet() - tabs) {
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

	const QSet<Op::Tabs> all = Op::tabsNames.keys().toSet();
	const QSet<Op::Tabs> withoutPcr = all - QSet<Op::Tabs> {Op::PcrPrimerDesign};
	const QSet<Op::Tabs> three = { Op::Search, Op::AnnotationsHighlighting, Op::Statistics };

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
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::start(os);
	GTUtilsTaskTreeView::waitTaskFinished(os);
	CHECK_SET_ERR(GTTableView::rowCount(os, GTUtilsPcrPrimer::getResultTableWgt(os)) == 0, "Results table is not empty")
	GTUtilsNotifications::checkNotificationReportText(os, "There are no primers that meet the specified parameters");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::setParamsOfPrimingSeqs(os, { { -40, -30 }, {  48,  65 }, {  18,  25 } });
	GTUtilsPcrPrimer::start(os);
	GTUtilsTaskTreeView::waitTaskFinished(os);
	QTableWidget* table = GTUtilsPcrPrimer::getResultTableWgt(os);
	CHECK_SET_ERR(GTTableView::rowCount(os, table) == 8, "There are no 8 results in the table")

	const auto checkResultTableRow = [table, &os](int i, const QString& expectedName, U2Range<int> expectedResult) {
		QString resultName = GTTableView::data(os, table, i, 0);
		CHECK_SET_ERR(expectedName == resultName, QString("Invalid result name: expected '%1', current '%2'").
			                                      arg(expectedName, resultName))
		GTUtilsPcrPrimer::checkResultTable(os, i, expectedResult);
	};
	checkResultTableRow(0, "A Forward", { 67, 87 });
	checkResultTableRow(1, "A Reverse", { 337, 357 });
	checkResultTableRow(2, "B1 Forward", { 67, 87 });
	checkResultTableRow(3, "B1 Reverse", { 337, 357 });
	checkResultTableRow(4, "B2 Forward", { 48, 70 });
	checkResultTableRow(5, "B2 Reverse", { 354, 376 });
	checkResultTableRow(6, "B3 Forward", { 44, 66 });
	checkResultTableRow(7, "B3 Reverse", { 358, 380 });

	const auto checkAnnotations = [&os](const QString& annotationName, const QString& expectedRegion) {
		const QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, annotationName);
		CHECK_SET_ERR(region == expectedRegion, QString("Invalid region for '%1' annotation: expected '%2', current "
			                                            "'%3'").arg(annotationName, expectedRegion, region))
	};
	checkAnnotations("A Forward", "67..87");
	checkAnnotations("A Reverse", "complement(337..357)");
	checkAnnotations("B1 Forward", "67..87");
	checkAnnotations("B1 Reverse", "complement(337..357)");
	checkAnnotations("B2 Forward", "48..70");
	checkAnnotations("B2 Reverse", "complement(354..376)");
	checkAnnotations("B3 Forward", "44..66");
	checkAnnotations("B3 Reverse", "complement(358..380)");

	GTUtilsNotifications::checkNotificationReportText(os, { "A Forward", "A Reverse", "B1 Forward", "B1 Reverse",
															"B2 Forward", "B2 Reverse", "B3 Forward", "B3 Reverse" });
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
	GTUtilsProject::openFileExpectSequence(os, testDir + "_common_data/pcr_primer_design/gfp.fa", "gfp");
	GTUtilsPcrPrimer::setParamsOfPrimingSeqs(os, { { -40, -30 }, {  48,  65 }, {  18,  25 } });
	GTUtilsPcrPrimer::openBackbone(os, testDir + "_common_data/pcr_primer_design/backbone.fa");
	GTUtilsPcrPrimer::start(os);
	GTUtilsTaskTreeView::waitTaskFinished(os);
	QTableWidget* table = GTUtilsPcrPrimer::getResultTableWgt(os);
	CHECK_SET_ERR(GTTableView::rowCount(os, table) == 6, "There are no 6 results in the table")

	const auto checkResultTableRow = [table, &os](int i, const QString& expectedName, U2Range<int> expectedResult) {
		QString resultName = GTTableView::data(os, table, i, 0);
	    CHECK_SET_ERR(expectedName == resultName, QString("Invalid result name: expected '%1', current '%2'").
			                                  arg(expectedName, resultName))
		GTUtilsPcrPrimer::checkResultTable(os, i, expectedResult);
	};
	checkResultTableRow(0, "A Forward", { 59, 80 });
	checkResultTableRow(1, "A Reverse", { 337, 357 });
	checkResultTableRow(2, "B1 Forward", { 58, 78 });
	checkResultTableRow(3, "B1 Reverse", { 346, 366 });
	checkResultTableRow(4, "B2 Forward", { 39, 61 });
	checkResultTableRow(5, "B2 Reverse", { 363, 385 });

	const auto checkAnnotations = [&os](const QString& annotationName, const QString& expectedRegion) {
		const QString region = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, annotationName);
		CHECK_SET_ERR(region == expectedRegion, QString("Invalid region for '%1' annotation: expected '%2', current "
			                                            "'%3'").arg(annotationName, expectedRegion, region))
	};
	checkAnnotations("A Forward", "59..80");
	checkAnnotations("A Reverse", "complement(337..357)");
	checkAnnotations("B1 Forward", "58..78");
	checkAnnotations("B1 Reverse", "complement(346..366)");
	checkAnnotations("B2 Forward", "39..61");
	checkAnnotations("B2 Reverse", "complement(363..385)");

	GTUtilsNotifications::clickOnNotificationWidget(os);
	if (const QTextEdit* reportEdit = GTWidget::findTextEdit(os, "reportTextEdit",
		                                                     GTUtilsMdi::checkWindowIsActive(os, "Task report "))) {
	    const QString html = reportEdit->toHtml();
		CHECK_SET_ERR(html.contains("A Forward") && html.contains("A Reverse") && html.contains("B1 Forward") &&
			          html.contains("B1 Reverse") && html.contains("B2 Forward") && html.contains("B2 Reverse"),
			          "Not all found sequences are present in the report")
		const QString expectedSubsequence = "<span style=\" text-decoration: underline;\">GACAGA</span>";
		const int subsequenceNum = html.split(expectedSubsequence).size() - 1;
	    CHECK_SET_ERR(subsequenceNum == 6, QString("Not all sequences contain expected subsequence. '%1' sequence "
			                                       "contain '%2'").arg(subsequenceNum).arg(expectedSubsequence))
	}
}

}  // namespace GUITest_common_scenarios_pcr_primer_design_for_dna_assembly

}  // namespace U2
