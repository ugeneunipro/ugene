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

#include "SmithWatermanDialogBaseFiller.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSpinBox>

const QString TEST_NAME_FOR_MA_RESULTS = "_test]";
const QString TEST_NAME_FOR_ANNOT_RESULTS = "test";
const QString ANNOTATION_RESULT_FILE_NAME = "Annotations.gb";
static const QString CLASSIC_SW_REALIZATION_NAME = "Classic 2";
static const QString SSE2_SW_REALIZATION_NAME = "SSE2";

const double GAP_OPEN_PENALTY = -3.0;
const double GAP_EXTENDING_PENALTY = -2.0;
const int RESULT_SCORE_PERCENTAGE = 45;

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::SmithWatermanDialogFiller"
#define GT_METHOD_NAME "run"

SmithWatermanDialogFiller::SmithWatermanDialogFiller(SwRealization _realization, const SmithWatermanSettings::SWResultView _resultView, const QString& _resultFilesPath, const QString& _pattern, const GTRegionSelector::RegionSelectorSettings& _s)
    : Filler("SmithWatermanDialogBase"), button(Search), pattern(_pattern), s(_s),
      resultView(_resultView), resultFilesPath(_resultFilesPath), autoSetupAlgorithmParams(true),
      realization(_realization) {
}

SmithWatermanDialogFiller::SmithWatermanDialogFiller(CustomScenario* scenario)
    : Filler("SmithWatermanDialogBase", scenario) {
}

SmithWatermanDialogFiller::SmithWatermanDialogFiller(const QString& _pattern, const GTRegionSelector::RegionSelectorSettings& _s, SwRealization _realization)
    : Filler("SmithWatermanDialogBase"), button(Search), pattern(_pattern), s(_s),
      resultView(SmithWatermanSettings::ANNOTATIONS), resultFilesPath(""),
      autoSetupAlgorithmParams(false), realization(_realization) {
}

void SmithWatermanDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (button == Cancel) {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        return;
    }

    auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);

    GTTabWidget::setCurrentIndex(tabWidget, 1);
    auto resultViewVariants = GTWidget::findComboBox("resultViewVariants", dialog);
    int resultViewIndex;
    if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
        resultViewIndex = 0;
    } else if (resultView == SmithWatermanSettings::ANNOTATIONS) {
        resultViewIndex = 1;
    } else {
        GT_FAIL("Unsupported mode: " + QString::number(resultView), );
    }
    GTComboBox::selectItemByIndex(resultViewVariants, resultViewIndex);

    if (!resultFilesPath.isEmpty()) {
        QLineEdit* resultFilePathContainer;
        if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
            resultFilePathContainer = GTWidget::findLineEdit("alignmentFilesPath", dialog);
        } else {
            GTRadioButton::click("rbCreateNewTable", dialog);
            resultFilePathContainer = GTWidget::findLineEdit("leNewTablePath", dialog);
            resultFilesPath += ANNOTATION_RESULT_FILE_NAME;
        }
        GTLineEdit::setText(resultFilePathContainer, resultFilesPath);
    }

    if (autoSetupAlgorithmParams) {
        if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
            GTGroupBox::setChecked("advOptions", true, dialog);
            GTLineEdit::setText("mObjectNameTmpl", GTLineEdit::getText("mObjectNameTmpl") + TEST_NAME_FOR_MA_RESULTS, dialog);

            GTLineEdit::setText("refSubseqNameTmpl", GTLineEdit::getText("refSubseqNameTmpl") + TEST_NAME_FOR_MA_RESULTS, dialog);
        } else {
            GTLineEdit::setText("leAnnotationName", TEST_NAME_FOR_ANNOT_RESULTS, dialog);
        }
    }

    GTTabWidget::setCurrentIndex(tabWidget, 0);

    auto regionSelector = GTWidget::findExactWidget<RegionSelector*>("range_selector", dialog);
    GTRegionSelector::setRegion(regionSelector, s);

    auto textEdit = GTWidget::findTextEdit("teditPattern", dialog);
    GTTextEdit::setText(textEdit, pattern);

    if (autoSetupAlgorithmParams) {
        auto comboRealization = GTWidget::findComboBox("comboRealization", dialog);
        QString realizationName;
        switch (realization) {
            case CLASSIC:
                realizationName = CLASSIC_SW_REALIZATION_NAME;
                break;
            case SSE2:
                realizationName = SSE2_SW_REALIZATION_NAME;
                break;
            default:
                Q_ASSERT(false);
        }

        const int swRealizationIndex = comboRealization->findText(realizationName);
        GTComboBox::selectItemByIndex(comboRealization, swRealizationIndex);

        auto comboResultFilter = GTWidget::findComboBox("comboResultFilter", dialog);
        GTComboBox::selectItemByText(comboResultFilter, "filter-intersections");

        auto spinScorePercent = GTWidget::findSpinBox("spinScorePercent", dialog);
        GTSpinBox::setValue(spinScorePercent, RESULT_SCORE_PERCENTAGE, GTGlobals::UseKeyBoard);

        auto dblSpinGapOpen = GTWidget::findSpinBox("spinGapOpen", dialog);
        GTSpinBox::setValue(dblSpinGapOpen, GAP_OPEN_PENALTY);

        auto dblSpinGapExtd = GTWidget::findSpinBox("spinGapExtd", dialog);
        GTSpinBox::setValue(dblSpinGapExtd, GAP_EXTENDING_PENALTY);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
