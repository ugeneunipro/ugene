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

#include "GenerateAlignmentProfileDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::GenerateAlignmentProfileDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void GenerateAlignmentProfileDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTGlobals::sleep(500);
    if (counts) {
        auto countsRB = GTWidget::findRadioButton("countsRB", dialog);
        GTRadioButton::click(countsRB);
    } else {
        auto percentsRB = GTWidget::findRadioButton("percentsRB", dialog);
        GTRadioButton::click(percentsRB);
    }

    if (format != NONE) {
        GTGroupBox::setChecked("saveBox", dialog);
        GTGlobals::sleep(500);

        GTLineEdit::setText("fileEdit", filePath, dialog);

        auto formatRB = GTWidget::findRadioButton(checkBoxItems[format], dialog);
        GTRadioButton::click(formatRB);
    }

    auto gapCB = GTWidget::findCheckBox("gapCB", dialog);
    GTCheckBox::setChecked(gapCB, gapScore);

    auto unusedCB = GTWidget::findCheckBox("unusedCB", dialog);
    GTCheckBox::setChecked(unusedCB, symdolScore);

    auto skipGapPositionsCB = GTWidget::findCheckBox("skipGapPositionsCB", dialog);
    GTCheckBox::setChecked(skipGapPositionsCB, skipGaps);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

}  // namespace U2
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
