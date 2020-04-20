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

#include "EditQualifierDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTCheckBox.h>
#include <drivers/GTKeyboardDriver.h>
#include "utils/GTKeyboardUtils.h"
#include <base_dialogs/MessageBoxFiller.h>

#include <QDir>
#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QToolButton>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::EditQualifierFiller"
#define GT_METHOD_NAME "commonScenario"
void EditQualifierFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *nameEdit = dialog->findChild<QLineEdit*>("nameEdit");
    GT_CHECK(nameEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, nameEdit, qualifierName, noCheck);

    QTextEdit *valueEdit = dialog->findChild<QTextEdit*>("valueEdit");
    GT_CHECK(valueEdit != NULL, "value line edit not found");
    if (!valueName.isEmpty()) {
        GTTextEdit::setText(os, valueEdit, valueName);
    }

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");

    if(closeErrormessageBox){
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        GTGlobals::sleep();
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
        GTGlobals::sleep();

        GTLineEdit::setText(os, nameEdit, "nice_name", noCheck);
        GTTextEdit::setText(os, valueEdit, "nice_val");
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
