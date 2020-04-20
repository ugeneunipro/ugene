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

#include <QDir>
#include <QApplication>
#include <QPushButton>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QGroupBox>

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <base_dialogs/MessageBoxFiller.h>

#include "ReplaceSubsequenceDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::replaceSubsequenceDialogFiller"
ReplaceSubsequenceDialogFiller::ReplaceSubsequenceDialogFiller(HI::GUITestOpStatus &_os, const QString &_pasteDataHere, bool recalculateQuals)
    : Filler(_os, "EditSequenceDialog"),
      pasteDataHere(_pasteDataHere),
      recalculateQuals(recalculateQuals)
{

}

ReplaceSubsequenceDialogFiller::ReplaceSubsequenceDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
    : Filler(os, "EditSequenceDialog", scenario),
      recalculateQuals(false)
{

}

#define GT_METHOD_NAME "commonScenario"
void ReplaceSubsequenceDialogFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QPlainTextEdit *plainText = dialog->findChild<QPlainTextEdit*>("sequenceEdit");
    GT_CHECK(plainText != NULL, "plain text not found");
    //GTKeyboardDriver::keyClick( GTKeyboardDriver::key["a"], Qt::ControlModifier);
    //GTGlobals::sleep();
    GTKeyboardDriver::keyClick( Qt::Key_Delete);
    GTGlobals::sleep();
    GTPlainTextEdit::setPlainText(os, plainText, pasteDataHere);

    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "recalculateQualsCheckBox"), recalculateQuals);

    GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
