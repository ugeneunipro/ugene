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

#include "EditQualifierDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>
#include <QToolButton>

#include "utils/GTKeyboardUtils.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::EditQualifierFiller"
void EditQualifierFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("nameEdit", qualifierName, dialog, noCheck);

    auto valueEdit = GTWidget::findTextEdit("valueEdit", dialog);
    if (!valueName.isEmpty()) {
        GTTextEdit::setText(valueEdit, valueName);
    }

    if (closeErrormessageBox) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
        GTGlobals::sleep();
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
        GTGlobals::sleep();

        GTLineEdit::setText("nameEdit", "nice_name", dialog, noCheck);
        GTTextEdit::setText(valueEdit, "nice_val");
    }
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

}  // namespace U2
