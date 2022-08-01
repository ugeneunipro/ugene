/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "CreateElementWithScriptDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>

namespace U2 {

#define GT_CLASS_NAME "CreateElementWithScriptDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void CreateElementWithScriptDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "nameEdit", name, dialog);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "ScriptEditorDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ScriptEditorDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "scriptPathEdit", url, dialog);

    QTextEdit* edit = nullptr;
    foreach (QTextEdit* textEdit,   dialog->findChildren<QTextEdit*>()) {
        if (!textEdit->isReadOnly()) {
            edit = textEdit;
        }
    }
    GT_CHECK(edit, "textEdit not found");
    GTTextEdit::setText(os, edit, text);

    if (checkSyntax) {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, checkSyntaxResult));
        GTWidget::click(os, GTWidget::findWidget(os, "checkButton", dialog));
        GTUtilsDialog::checkNoActiveWaiters(os);
    }
    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "ScriptEditorDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ScriptEditorDialogSyntaxChecker::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QTextEdit* edit = nullptr;
    foreach (QTextEdit* textEdit, dialog->findChildren<QTextEdit*>()) {
        if (!textEdit->isReadOnly()) {
            edit = textEdit;
        }
    }
    GT_CHECK(edit != nullptr, "textEdit not found");
    GTTextEdit::setText(os, edit, text);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, message));
    GTWidget::click(os, GTWidget::findWidget(os, "checkButton", dialog));

    GTGlobals::sleep(1000);

    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}  // namespace U2
