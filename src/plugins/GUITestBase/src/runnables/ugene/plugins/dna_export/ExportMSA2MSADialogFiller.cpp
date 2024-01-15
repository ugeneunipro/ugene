/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ExportMSA2MSADialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportToSequenceFormatFiller"

ExportMSA2MSADialogFiller::ExportMSA2MSADialogFiller(int _formatVal, QString _path, bool _includeGaps, bool _unknownAsGaps, int _frame)
    : Filler("U2__ExportMSA2MSADialog"),
      formatVal(_formatVal),
      path(_path),
      includeGaps(_includeGaps),
      unknownAsGaps(_unknownAsGaps),
      frame(_frame) {
}

void ExportMSA2MSADialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (!path.isEmpty()) {
        GTLineEdit::setText("fileNameEdit", path, dialog);
    }
    if (formatVal >= 0) {
        auto formatCombo = GTWidget::findComboBox("formatCombo", dialog);
        GTComboBox::selectItemByIndex(formatCombo, formatVal);
    }
    if (includeGaps) {
        GTCheckBox::setChecked("cbIncludeGaps", dialog);

        if (unknownAsGaps) {
            GTRadioButton::click("rbUseGaps", dialog);
        }
    }

    QString widgetName;
    if (frame == 1) {
        widgetName = "rbFirstDirectFrame";
    } else if (frame == 2) {
        widgetName = "rbSecondDirectFrame";
    } else if (frame == 3) {
        widgetName = "rbThirdDirectFrame";
    } else if (frame == -1) {
        widgetName = "rbFirstComplementFrame";
    } else if (frame == -2) {
        widgetName = "rbSecondComplementFrame";
    } else if (frame == -3) {
        widgetName = "rbThirdComplementFrame";
    } else {
        GT_FAIL("incorrect frame", );
    }
    GTRadioButton::click(widgetName, dialog);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

}  // namespace U2
