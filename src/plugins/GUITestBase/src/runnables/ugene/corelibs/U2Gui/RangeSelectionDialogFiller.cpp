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

#include "RangeSelectionDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>

#include <QApplication>
#include <QPushButton>
#include <QToolButton>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(HI::GUITestOpStatus &_os, int *_len) : Filler(_os, "RangeSelectionDialog")
{
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

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(HI::GUITestOpStatus &_os, CustomScenario* scenario) : Filler(_os, "RangeSelectionDialog", scenario)
{
    rangeType = Single;
    selectAll = true;
    fromBegin = false;
    minVal = 0;
    maxVal = 0;
    length = 0;
    len = NULL;
    multipleRange = QString();
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(HI::GUITestOpStatus &_os, int _minVal, int _maxVal) : Filler(_os, "RangeSelectionDialog")
{
    rangeType = Single;
    selectAll = false;
    fromBegin = false;
    minVal = _minVal;
    maxVal = _maxVal;
    length = 0;
    len = NULL;
    multipleRange = QString();
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(HI::GUITestOpStatus &_os, const QString &range) : Filler(_os, "RangeSelectionDialog")
{
    rangeType = Multiple;
    selectAll = false;
    fromBegin = false;
    minVal = 0;
    maxVal = 0;
    length = 0;
    len = NULL;
    multipleRange = range;
    circular = false;
}

SelectSequenceRegionDialogFiller::SelectSequenceRegionDialogFiller(HI::GUITestOpStatus &_os, int _length, bool selectFromBegin) : Filler(_os, "RangeSelectionDialog")
{
    rangeType = Single;
    selectAll = false;
    fromBegin = selectFromBegin;
    minVal = 0;
    maxVal = 0;
    length = _length;
    len = NULL;
    multipleRange = QString();
    circular = false;
}

void SelectSequenceRegionDialogFiller::setCircular(bool v) {
    circular = v;
}


#define GT_METHOD_NAME "commonScenario"
void SelectSequenceRegionDialogFiller::commonScenario()
{
    GTGlobals::sleep(500);
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if (selectAll) {
        QToolButton *min = dialog->findChild<QToolButton*>("minButton");
        QToolButton *max = dialog->findChild<QToolButton*>("maxButton");
        GT_CHECK(min != NULL, "Min button not found");
        GT_CHECK(max != NULL, "Max button not found");

        GTWidget::click(os, min);
        GTGlobals::sleep(500);
        GTWidget::click(os, max);
        GTGlobals::sleep(500);

        if (len != NULL) {
            QLineEdit *endEdit = dialog->findChild<QLineEdit*>("endEdit");
            GT_CHECK(endEdit != NULL, "QLineEdit \"endEdit\" not found");
            *len = endEdit->text().toInt();
        }
    } else if (rangeType == Single) {
        GT_CHECK(circular || minVal <= maxVal, "Value \"min\" greater then \"max\"");

        QLineEdit *startEdit = dialog->findChild<QLineEdit*>("startEdit");
        QLineEdit *endEdit = dialog->findChild<QLineEdit*>("endEdit");
        GT_CHECK(startEdit != NULL, "QLineEdit \"startEdit\" not found");
        GT_CHECK(endEdit != NULL, "QLineEdit \"endEdit\" not found");

        if (length == 0) {
            GTLineEdit::setText(os, startEdit, QString::number(minVal));
            GTLineEdit::setText(os, endEdit, QString::number(maxVal));
        } else {
            int min = startEdit->text().toInt();
            int max = endEdit->text().toInt();
            GT_CHECK(max - min >= length, "Invalid argument \"length\"");

            if (fromBegin) {
                GTLineEdit::setText(os, startEdit, QString::number(1));
                GTLineEdit::setText(os, endEdit, QString::number(length));
            } else {
                GTLineEdit::setText(os, startEdit, QString::number(max - length + 1));
                GTLineEdit::setText(os, endEdit, QString::number(max));
            }
        }
    } else {
        GT_CHECK(! multipleRange.isEmpty(), "Range is empty");

        QRadioButton *multipleButton = dialog->findChild<QRadioButton*>("miltipleButton");
        GT_CHECK(multipleButton != NULL, "RadioButton \"miltipleButton\" not found");
        GTRadioButton::click(os, multipleButton);

        QLineEdit *regionEdit = dialog->findChild<QLineEdit*>("multipleRegionEdit");
        GT_CHECK(regionEdit != NULL, "QLineEdit \"multipleRegionEdit\" not foud");
        GTLineEdit::setText(os, regionEdit, multipleRange);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
