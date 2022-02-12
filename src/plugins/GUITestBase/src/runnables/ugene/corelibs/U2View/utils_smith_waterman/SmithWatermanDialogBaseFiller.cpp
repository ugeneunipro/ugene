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

SmithWatermanDialogFiller::SmithWatermanDialogFiller(HI::GUITestOpStatus& _os, SwRealization _realization, const SmithWatermanSettings::SWResultView _resultView, const QString& _resultFilesPath, const QString& _pattern, const GTRegionSelector::RegionSelectorSettings& _s)
    : Filler(_os, "SmithWatermanDialogBase"), button(Search), pattern(_pattern), s(_s),
      resultView(_resultView), resultFilesPath(_resultFilesPath), autoSetupAlgorithmParams(true),
      realization(_realization) {
}

SmithWatermanDialogFiller::SmithWatermanDialogFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "SmithWatermanDialogBase", scenario) {
}

SmithWatermanDialogFiller::SmithWatermanDialogFiller(HI::GUITestOpStatus& _os, const QString& _pattern, const GTRegionSelector::RegionSelectorSettings& _s, SwRealization _realization)
    : Filler(_os, "SmithWatermanDialogBase"), button(Search), pattern(_pattern), s(_s),
      resultView(SmithWatermanSettings::ANNOTATIONS), resultFilesPath(""),
      autoSetupAlgorithmParams(false), realization(_realization) {
}

void SmithWatermanDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (button == Cancel) {
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(GTWidget::findWidget(os, "tabWidget", dialog));

    GTTabWidget::setCurrentIndex(os, tabWidget, 1);
    QComboBox* resultViewVariants = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "resultViewVariants", dialog));
    int resultViewIndex;
    if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
        resultViewIndex = 0;
    } else if (resultView == SmithWatermanSettings::ANNOTATIONS) {
        resultViewIndex = 1;
    } else {
        GT_FAIL("Unsupported mode: " + QString::number(resultView), );
    }
    GTComboBox::selectItemByIndex(os, resultViewVariants, resultViewIndex);

    if (!resultFilesPath.isEmpty()) {
        QLineEdit* resultFilePathContainer;
        if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
            resultFilePathContainer = GTWidget::findLineEdit(os, "alignmentFilesPath", dialog);
        } else {
            GTRadioButton::click(os, "rbCreateNewTable", dialog);
            resultFilePathContainer = GTWidget::findLineEdit(os, "leNewTablePath", dialog);
            resultFilesPath += ANNOTATION_RESULT_FILE_NAME;
        }
        GTLineEdit::setText(os, resultFilePathContainer, resultFilesPath);
    }

    if (autoSetupAlgorithmParams) {
        if (resultView == SmithWatermanSettings::MULTIPLE_ALIGNMENT) {
            GTGroupBox::setChecked(os, "advOptions", true, dialog);
            auto objectNameEdit = GTWidget::findLineEdit(os, "mObjectNameTmpl", dialog);
            GTLineEdit::setText(os, objectNameEdit, objectNameEdit->text() + TEST_NAME_FOR_MA_RESULTS);

            auto referenceSequenceNameEdit = GTWidget::findLineEdit(os, "refSubseqNameTmpl", dialog);
            GTLineEdit::setText(os, referenceSequenceNameEdit, referenceSequenceNameEdit->text() + TEST_NAME_FOR_MA_RESULTS);
        } else {
            auto annotationNameEdit = GTWidget::findLineEdit(os, "leAnnotationName", dialog);
            GTLineEdit::setText(os, annotationNameEdit, TEST_NAME_FOR_ANNOT_RESULTS);
        }
    }

    GTTabWidget::setCurrentIndex(os, tabWidget, 0);

    RegionSelector* regionSelector = qobject_cast<RegionSelector*>(GTWidget::findWidget(os, "range_selector", dialog));
    GTRegionSelector::setRegion(os, regionSelector, s);

    QTextEdit* textEdit = qobject_cast<QTextEdit*>(GTWidget::findWidget(os, "teditPattern", dialog));
    GTTextEdit::setText(os, textEdit, pattern);

    if (autoSetupAlgorithmParams) {
        QComboBox* comboRealization = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "comboRealization", dialog));
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
        GTComboBox::selectItemByIndex(os, comboRealization, swRealizationIndex);

        QComboBox* comboResultFilter = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "comboResultFilter", dialog));
        GTComboBox::selectItemByText(os, comboResultFilter, "filter-intersections");

        QSpinBox* spinScorePercent = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "spinScorePercent", dialog));
        GTSpinBox::setValue(os, spinScorePercent, RESULT_SCORE_PERCENTAGE, GTGlobals::UseKeyBoard);

        QSpinBox* dblSpinGapOpen = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "spinGapOpen", dialog));
        GTSpinBox::setValue(os, dblSpinGapOpen, GAP_OPEN_PENALTY);

        QSpinBox* dblSpinGapExtd = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "spinGapExtd", dialog));
        GTSpinBox::setValue(os, dblSpinGapExtd, GAP_EXTENDING_PENALTY);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
