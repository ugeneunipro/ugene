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

#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>

#include "ReplaceSubsequenceDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::replaceSubsequenceDialogFiller"
ReplaceSubsequenceDialogFiller::ReplaceSubsequenceDialogFiller(const QString& _pasteDataHere, bool recalculateQuals, bool _expectAlphabetChanged)
    : Filler("EditSequenceDialog"),
      pasteDataHere(_pasteDataHere),
      recalculateQuals(recalculateQuals),
      expectAlphabetChanged(_expectAlphabetChanged) {
}

ReplaceSubsequenceDialogFiller::ReplaceSubsequenceDialogFiller(CustomScenario* scenario, bool _expectAlphabetChanged)
    : Filler("EditSequenceDialog", scenario),
      recalculateQuals(false),
      expectAlphabetChanged(_expectAlphabetChanged) {
}

#define GT_METHOD_NAME "commonScenario"
void ReplaceSubsequenceDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto plainText = GTWidget::findPlainTextEdit("sequenceEdit", dialog);
    // GTKeyboardDriver::keyClick( GTKeyboardDriver::key["a"], Qt::ControlModifier);
    // GTGlobals::sleep();
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep();
    GTPlainTextEdit::setText(plainText, pasteDataHere);

    GTCheckBox::setChecked(GTWidget::findCheckBox("recalculateQualsCheckBox"), recalculateQuals);
    if (expectAlphabetChanged) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    }
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
