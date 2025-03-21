/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "HmmerSearchDialogFiller.h"
namespace U2 {

#define GT_CLASS_NAME "UHMM3SearchDialogFiller"

void HmmerSearchDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (domE != -1) {
        auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
        GTTabWidget::clickTab(tabWidget, "Reporting thresholds");
        GTSpinBox::setValue("domESpinBox", domE, GTGlobals::UseKeyBoard, dialog);
        GTTabWidget::clickTab(tabWidget, "Input and output");
    }

    GTLineEdit::setText("queryHmmFileEdit", profile, dialog);

    auto radio = GTWidget::findRadioButton("rbCreateNewTable", dialog);
    GTRadioButton::click(radio);

    GTLineEdit::setText("leNewTablePath", newFilePath, dialog);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME
}  // namespace U2
