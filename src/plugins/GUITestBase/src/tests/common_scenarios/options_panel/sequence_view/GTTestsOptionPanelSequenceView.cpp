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

#include <QTableWidget>
#include <QGraphicsItem>
#include <QMainWindow>

#include "GTTestsOptionPanelSequenceView.h"

#include "primitives/GTAction.h"
#include "api/GTBaseCompleter.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include "system/GTFile.h"
#include <base_dialogs/GTFileDialog.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTWidget.h>

#include <base_dialogs/MessageBoxFiller.h>
#include <base_dialogs/ColorDialogFiller.h>

#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsTaskTreeView.h"

#include <U2Core/AppContext.h>

namespace U2{

namespace GUITest_common_scenarios_options_panel_sequence_view{
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001){
    //checking 'next' 'prev' buttons functionality
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isGetAnnotationsEnabled(os), "Get annotations is enabled");
    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAAAAAAAAA");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickNext(os);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 2/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickNext(os);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/2"), "Results string not match");

    GTUtilsOptionPanelSequenceView::clickPrev(os);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 2/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0002){
    //checking searching with invalid pattern
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "zz");

    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 0/0"), "Results string is not match");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isPrevNextEnabled(os), "Next and prev buttons are enabled");
}

GUI_TEST_CLASS_DEFINITION(test_0003){
    //checking results with diffirent algorithms
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::toggleInputFromFilePattern(os);
    GTUtilsOptionPanelSequenceView::enterPatternFromFile(os, testDir + "_common_data/FindAlgorithm/", "find_pattern_op_1.fa");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/40"), "Results string not match");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setStrand(os, "Direct");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/16"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0004){
    //checking saving annotations after search
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);
    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAAAAAAAAA");
    GTUtilsOptionPanelSequenceView::toggleSaveAnnotationsTo(os);
    GTUtilsOptionPanelSequenceView::enterFilepathForSavingAnnotations(os, sandBoxDir + "op_seqview_test_0001.gb");
    GTUtilsOptionPanelSequenceView::clickGetAnnotation(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    GTWidget::click(os, GTWidget::findWidget(os, "annotations_tree_widget"));
    GTUtilsAnnotationsTreeView::findItem(os, "Misc. Feature  (0, 2)");
}

GUI_TEST_CLASS_DEFINITION(test_0005){
    //checking searching with different parameter 'match percentage'
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/40"), "Results string not match");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search algorithm"));
    GTUtilsOptionPanelSequenceView::setAlgorithm(os, "Substitute");
    GTUtilsOptionPanelSequenceView::setMatchPercentage(os, 75);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/1649"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0006) {
//    1. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Open "Search In Sequence" options panel tab.
//    Expected state: all show/hide widgetsare collapsed.
    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSearchAlgorithmShowHideWidgetOpened(os), "'Search algorithm' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSearchInShowHideWidgetOpened(os), "'Search in' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isOtherSettingsShowHideWidgetOpened(os), "'Other settings' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isSaveAnnotationToShowHideWidgetOpened(os), "'Save annotations to' subwidget is unexpectedly opened");
    CHECK_SET_ERR(!GTUtilsOptionPanelSequenceView::isAnnotationParametersShowHideWidgetOpened(os), "'Annotation parameters' subwidget is unexpectedly opened");
}

GUI_TEST_CLASS_DEFINITION(test_0007){
    //checking results with searching in translation
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "FFFFFFFFF");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 0/0"), "Results string not match");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInTranslation(os, true);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0008){
    //checking results with searching in translation
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/40"), "Results string not match");

    GTUtilsSequenceView::selectSequenceRegion(os, 1, 9000);

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInLocation(os, "Selected region");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/2"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0009){
    //checking results with searching in translation
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "AAAAAAAAAAAAAAAAAAAA");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/40"), "Results string not match");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Search in"));
    GTUtilsOptionPanelSequenceView::setSearchInLocation(os, "Custom region");

    QLineEdit *regLE = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "editEnd", NULL, GTGlobals::FindOptions(false)));
    CHECK_SET_ERR(regLE != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, regLE, "40000");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/12"), "Results string not match");

    regLE = NULL;
    regLE = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "editStart", NULL, GTGlobals::FindOptions(false)));
    CHECK_SET_ERR(regLE != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, regLE, "9000");
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/10"), "Results string not match");
}

GUI_TEST_CLASS_DEFINITION(test_0010){
    //checking results with searching in translation
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsOptionPanelSequenceView::openTab(os, GTUtilsOptionPanelSequenceView::Search);

    GTUtilsOptionPanelSequenceView::enterPattern(os, "A");

    GTWidget::click(os, GTWidget::findWidget(os, "ArrowHeader_Other settings"));
    GTUtilsOptionPanelSequenceView::setSetMaxResults(os, 99900);
    CHECK_SET_ERR(GTUtilsOptionPanelSequenceView::checkResultsText(os, "Results: 1/99900"), "Results string not match");
}

}

}
