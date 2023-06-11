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

#include "RangeSelectionDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QToolButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(int* _len)
    : Filler("RangeSelectionDialog") {
    rangeType = Single;
    selectAll = true;
    fromBegin = false;
    minVal = 0;
    maxVal = 0;
    length = 0;
    len = _len;
    multipleRange = QString();
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(CustomScenario* scenario)
    : Filler("RangeSelectionDialog", scenario) {
    rangeType = Single;
    selectAll = true;
    fromBegin = false;
    minVal = 0;
    maxVal = 0;
    length = 0;
    len = nullptr;
    multipleRange = QString();
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(int _minVal, int _maxVal)
    : Filler("RangeSelectionDialog") {
    rangeType = Single;
    selectAll = false;
    fromBegin = false;
    minVal = _minVal;
    maxVal = _maxVal;
    length = 0;
    len = nullptr;
    multipleRange = QString();
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(const QString& range)
    : Filler("RangeSelectionDialog") {
    rangeType = Multiple;
    selectAll = false;
    fromBegin = false;
    minVal = 0;
    maxVal = 0;
    length = 0;
    len = nullptr;
    multipleRange = range;
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(int _length, bool selectFromBegin)
    : Filler("RangeSelectionDialog") {
    rangeType = Single;
    selectAll = false;
    fromBegin = selectFromBegin;
    minVal = 0;
    maxVal = 0;
    length = _length;
    len = nullptr;
    multipleRange = QString();
    circular = false;
}

void SelectSequenceRegionDialogFiller::setCircular(bool v) {
    circular = v;
}

#define GT_METHOD_NAME "commonScenario"
void SelectSequenceRegionDialogFiller::commonScenario() {
    GTGlobals::sleep(500);
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (selectAll) {
        auto min = GTWidget::findToolButton("minButton", dialog);
        auto max = GTWidget::findToolButton("maxButton", dialog);

        GTWidget::click(min);
        GTGlobals::sleep(500);
        GTWidget::click(max);
        GTGlobals::sleep(500);

        if (len != nullptr) {
            auto endEdit = GTWidget::findLineEdit("endEdit", dialog);
            *len = endEdit->text().toInt();
        }
    } else if (rangeType == Single) {
        GT_CHECK(circular || minVal <= maxVal, "Value \"min\" greater then \"max\"");

        auto startEdit = GTWidget::findLineEdit("startEdit", dialog);
        auto endEdit = GTWidget::findLineEdit("endEdit", dialog);

        if (length == 0) {
            GTLineEdit::setText(startEdit, QString::number(minVal));
            GTLineEdit::setText(endEdit, QString::number(maxVal));
        } else {
            int min = startEdit->text().toInt();
            int max = endEdit->text().toInt();
            GT_CHECK(max - min >= length, "Invalid argument \"length\"");

            if (fromBegin) {
                GTLineEdit::setText(startEdit, QString::number(1));
                GTLineEdit::setText(endEdit, QString::number(length));
            } else {
                GTLineEdit::setText(startEdit, QString::number(max - length + 1));
                GTLineEdit::setText(endEdit, QString::number(max));
            }
        }
    } else {
        GT_CHECK(!multipleRange.isEmpty(), "Range is empty");

        auto multipleButton = GTWidget::findRadioButton("miltipleButton", dialog);
        GTRadioButton::click(multipleButton);

        GTLineEdit::setText("multipleRegionEdit", multipleRange, dialog);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
