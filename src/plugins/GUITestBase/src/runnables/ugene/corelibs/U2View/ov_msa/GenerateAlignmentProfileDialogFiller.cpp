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
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTGlobals::sleep(500);
    if (counts) {
        auto countsRB = GTWidget::findRadioButton(os, "countsRB", dialog);
        GTRadioButton::click(os, countsRB);
    } else {
        auto percentsRB = GTWidget::findRadioButton(os, "percentsRB", dialog);
        GTRadioButton::click(os, percentsRB);
    }

    if (format != NONE) {
        GTGroupBox::setChecked(os, "saveBox", dialog);
        GTGlobals::sleep(500);

        GTLineEdit::setText(os, "fileEdit", filePath, dialog);

        auto formatRB = GTWidget::findRadioButton(os, checkBoxItems[format], dialog);
        GTRadioButton::click(os, formatRB);
    }

    auto gapCB = GTWidget::findCheckBox(os, "gapCB", dialog);
    GTCheckBox::setChecked(os, gapCB, gapScore);

    auto unusedCB = GTWidget::findCheckBox(os, "unusedCB", dialog);
    GTCheckBox::setChecked(os, unusedCB, symdolScore);

    auto skipGapPositionsCB = GTWidget::findCheckBox(os, "skipGapPositionsCB", dialog);
    GTCheckBox::setChecked(os, skipGapPositionsCB, skipGaps);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

}  // namespace U2
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
