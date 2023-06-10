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

#include "GTTestsOptionPanelSequenceView.h"
#include <base_dialogs/ColorDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTWidget.h>

#include <QGraphicsItem>
#include <QMainWindow>
#include <QTableWidget>

#include <U2Core/AppContext.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTBaseCompleter.h"
#include "primitives/GTAction.h"
#include "runnables/ugene/corelibs/U2View/temperature/MeltingTemperatureSettingsDialogFiller.h"
#include "system/GTFile.h"

namespace U2 {

namespace GUITest_common_scenarios_options_panel_sequence_view {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    // checking 'next' 'prev' buttons functionality
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isGetAnnotationsEnabled(), "Get annotations is enabled");
    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAAAAAAAAAAAAAAAAAAAAAA");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickNext();
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 2/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickNext();
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickPrev();
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 2/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
    // checking searching with invalid pattern
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("zz");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: -/0"), "Results string is not match");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isPrevNextEnabled(), "Next and prev buttons are enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0003) {
    // checking results with diffirent algorithms
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern();
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(testDir + "_common_data/FindAlgorithm/", "find_pattern_op_1.fa");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/40"), "Results string not match");

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setStrand("Direct");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/16"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {
    // checking saving annotations after search
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAAAAAAAAAAAAAAAAAAAAAA");
    GTUtilsOptionPanelSequenceView::toggleSaveAnnotationsTo();
    GTUtilsOptionPanelSequenceView::enterFilepathForSavingAnnotations(sandBoxDir + "op_seqview_test_0001.gb");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation();
    GTUtilsTaskTreeView::waitTaskFinished();
    GTWidget::click(GTWidget::findWidget("annotations_tree_widget"));
    GTUtilsAnnotationsTreeView::findItem("misc_feature  (0, 2)");
}

GUI_TEST_CLASS_DEFINITION(test_0005) {
    // checking searching with different parameter 'match percentage'
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/40"), "Results string not match");

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search algorithm"));
    GTUtilsOptionPanelSequenceView::setAlgorithm("Substitute");
    GTUtilsOptionPanelSequenceView::setMatchPercentage(75);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/1649"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
    //    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    //    2. Open "Search In Sequence" options panel tab.
    //    Expected state: all show/hide widgetsare collapsed.
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSearchAlgorithmShowHideWidgetOpened(), "'Search algorithm' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSearchInShowHideWidgetOpened(), "'Search in' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isOtherSettingsShowHideWidgetOpened(), "'Other settings' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSaveAnnotationToShowHideWidgetOpened(), "'Save annotations to' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isAnnotationParametersShowHideWidgetOpened(), "'Annotation parameters' subwidget is unexpectedly opened");
}

GUI_TEST_CLASS_DEFINITION(test_0007) {
    // checking results with searching in translation
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("FFFFFFFFF");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: -/0"), "Results string not match");

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInTranslation(true);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0008) {
    // checking results with searching in translation
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/40"), "Results string not match");

    GTUtilsSequenceView::selectSequenceRegion(1, 9000);

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInLocation("Selected region");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: -/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0009) {
    // checking results with searching in translation
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/40"), "Results string not match");

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInLocation("Custom region");

    auto regLE = GTWidget::findLineEdit("editEnd", nullptr, {false});
    CHECK_SET_ERR(regLE != nullptr, "LineEdit is NULL");
    GTLineEdit::setText(regLE, "40000");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/12"), "Results string not match");

    regLE = nullptr;
    regLE = GTWidget::findLineEdit("editStart", nullptr, {false});
    CHECK_SET_ERR(regLE != nullptr, "LineEdit is NULL");
    GTLineEdit::setText(regLE, "9000");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/10"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0010) {
    // checking results with searching in translation
    GTFileDialog::openFile(dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished();

    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern("A");

    GTWidget::click(GTWidget::findWidget("ArrowHeader_Other settings"));
    GTUtilsOptionPanelSequenceView::setSetMaxResults(99900);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText("Results: 1/99900"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0011) {
    // Open human_T1_cutted.fa
    // Open the "Statistics" tab
    // Expected: Melting temperature = 79.78
    // Set Primer 3 settings
    // Expected: Melting temperature = 78.57
    // Open and close the "Statistics" tab again
    // Expected: Melting temperature = 78.57
    GTFileDialog::openFile(testDir + "_common_data/fasta", "human_T1_cutted.fa");
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);
    auto statisticsLabel = GTWidget::findLabel("Common Statistics");
    QString roughMeltTemp = GTUtilsOptionPanelSequenceView::meltingTmReportString.arg("83.70");
    CHECK_SET_ERR(statisticsLabel->text().contains(roughMeltTemp), QString("No expected string: %1").arg(roughMeltTemp));

    QMap<GTUtilsMeltingTemperature::Parameter, QString> parameters;
    parameters.insert(GTUtilsMeltingTemperature::Parameter::Algorithm, "Primer 3");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::DnaConc, "49.5");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::MonovalentConc, "55.0");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::DivalentConc, "2.5");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::DntpConc, "0.5");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::DmsoConc, "10.5");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::DmsoFactor, "0.4");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::FormamideConc, "1.2");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::MaxLen, "33");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::ThermodynamicTable, "0");
    parameters.insert(GTUtilsMeltingTemperature::Parameter::SaltCorrectionFormula, "2");
    GTUtilsDialog::waitForDialog(new MeltingTemperatureSettingsDialogFiller(parameters));
    GTUtilsOptionPanelSequenceView::showMeltingTemperatureDialog();
    GTUtilsTaskTreeView::waitTaskFinished();

    QString primer3MeltTemp = GTUtilsOptionPanelSequenceView::meltingTmReportString.arg("78.57");
    CHECK_SET_ERR(statisticsLabel->text().contains(primer3MeltTemp), QString("No expected string: %1").arg(primer3MeltTemp));

    GTUtilsOptionPanelSequenceView::closeTab(GTUtilsOptionPanelSequenceView::Statistics);
    GTUtilsOptionPanelSequenceView::openTab(GTUtilsOptionPanelSequenceView::Statistics);
    statisticsLabel = GTWidget::findLabel("Common Statistics");
    CHECK_SET_ERR(statisticsLabel->text().contains(primer3MeltTemp), QString("No expected string: %1").arg(primer3MeltTemp));
}

}  // namespace GUITest_common_scenarios_options_panel_sequence_view

}  // namespace U2
