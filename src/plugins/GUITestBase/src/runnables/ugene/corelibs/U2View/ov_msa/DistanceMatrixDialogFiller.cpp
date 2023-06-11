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

#include <GTGlobals.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include "DistanceMatrixDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DistanceMatrixDialogFiller"

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(bool _hamming, bool _counts, bool _excludeGaps)
    : Filler("DistanceMatrixMSAProfileDialog"),
      hamming(_hamming),
      counts(_counts),
      excludeGaps(_excludeGaps),
      saveToFile(false),
      format(HTML) {
}

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(DistanceMatrixDialogFiller::SaveFormat _format, QString _path)
    : Filler("DistanceMatrixMSAProfileDialog"),
      hamming(true),
      counts(true),
      excludeGaps(true),
      saveToFile(true),
      format(_format),
      path(QDir::toNativeSeparators(_path)) {
}

DistanceMatrixDialogFiller::DistanceMatrixDialogFiller(CustomScenario* c)
    : Filler("DistanceMatrixMSAProfileDialog", c),
      hamming(false),
      counts(false),
      excludeGaps(false),
      saveToFile(false),
      format(NONE) {
}

#define GT_METHOD_NAME "run"
void DistanceMatrixDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto algoCombo = GTWidget::findComboBox("algoCombo", dialog);
    if (hamming) {
        GTComboBox::selectItemByIndex(algoCombo, 0);
    } else {
        GTComboBox::selectItemByIndex(algoCombo, 1);
    }

    if (counts) {
        auto countsRB = GTWidget::findRadioButton("countsRB", dialog);
        GTRadioButton::click(countsRB);
    } else {
        auto percentsRB = GTWidget::findRadioButton("percentsRB", dialog);
        GTRadioButton::click(percentsRB);
    }

    if (format != NONE) {
        if (saveToFile) {
            GTGroupBox::setChecked("saveBox", dialog);

            GTLineEdit::setText("fileEdit", QDir::toNativeSeparators(path), dialog);

            if (format == HTML) {
                auto htmlRB = GTWidget::findRadioButton("htmlRB", dialog);
                GTRadioButton::click(htmlRB);
            } else {
                auto csvRB = GTWidget::findRadioButton("csvRB", dialog);
                GTRadioButton::click(csvRB);
            }
        }
    }
    auto checkBox = GTWidget::findCheckBox("checkBox", dialog);
    GTCheckBox::setChecked(checkBox, excludeGaps);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}  // namespace U2
