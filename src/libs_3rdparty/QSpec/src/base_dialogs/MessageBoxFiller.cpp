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

#include "base_dialogs/MessageBoxFiller.h"

#include <QAbstractButton>
#include <QApplication>

#include "drivers/GTKeyboardDriver.h"
#include "primitives/GTSpinBox.h"
#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTUtilsDialog::MessageBoxDialogFiller"

MessageBoxDialogFiller::MessageBoxDialogFiller(QMessageBox::StandardButton b, const QString& _message, const QString& objectName)
    : Filler(objectName),
      b(b),
      message(_message) {
    settings.logName = "MessageBoxDialogFiller, message: '" + message + "', button code: " + QString::number(b);
}

MessageBoxDialogFiller::MessageBoxDialogFiller(const QString& _buttonText, const QString& _message)
    : Filler(""),
      b(QMessageBox::NoButton),
      buttonText(_buttonText),
      message(_message) {
    settings.logName = "MessageBoxDialogFiller, message: '" + message + "', buttonText: '" + buttonText + "'";
}

void MessageBoxDialogFiller::commonScenario() {
    auto modalWidget = GTWidget::getActiveModalWidget();
    auto messageBox = qobject_cast<QMessageBox*>(modalWidget);
    GT_CHECK(messageBox != nullptr, "messageBox is NULL");

    if (!message.isEmpty()) {
        QString actualText = messageBox->text();
        GT_CHECK(messageBox->text().contains(message, Qt::CaseInsensitive),
                 QString("Expected: %1, found: %2").arg(message).arg(actualText));
    }

    if (!buttonText.isEmpty()) {
        for (QAbstractButton* button : messageBox->buttons()) {
            if (button->text().contains(buttonText, Qt::CaseInsensitive)) {
                GTWidget::click(button);
                return;
            }
        }
    }

    QAbstractButton* button = messageBox->button(b);
    GT_CHECK(button != nullptr, QString("There is no such button in messagebox: 0x%1").arg(QString::number(b, 16)));
    GTWidget::click(button);
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::AppCloseMessageBoxDialogFiller"

AppCloseMessageBoxDialogFiller::AppCloseMessageBoxDialogFiller()
    : Filler("") {
}

void AppCloseMessageBoxDialogFiller::commonScenario() {
    QWidget* activeModal = GTWidget::getActiveModalWidget();
    auto messageBox = qobject_cast<QMessageBox*>(activeModal);
    GT_CHECK(messageBox != NULL, "messageBox is NULL");

    QAbstractButton* noButton = messageBox->button(QMessageBox::No);
    QAbstractButton* noToAllButton = messageBox->button(QMessageBox::NoToAll);

    if (NULL != noToAllButton) {
        GTWidget::click(noToAllButton);
    } else if (NULL != noButton) {
        GTWidget::click(noButton);
    } else {
        GT_FAIL("There are neither \"No\" or \"No to all\" buttons in the message box", );
    }
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::MessageBoxNoToAllOrNo"

MessageBoxNoToAllOrNo::MessageBoxNoToAllOrNo()
    : Filler("") {
}

void MessageBoxNoToAllOrNo::commonScenario() {
    QWidget* activeModal = GTWidget::getActiveModalWidget();
    auto messageBox = qobject_cast<QMessageBox*>(activeModal);
    GT_CHECK(messageBox != NULL, "messageBox is NULL");

    QAbstractButton* button = messageBox->button(QMessageBox::NoToAll);
    if (!button) {
        button = messageBox->button(QMessageBox::No);
    }
    GT_CHECK(button != NULL, "There are no No buttons in messagebox");

    GTWidget::click(button);
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "MessageBoxOpenAnotherProject"

MessageBoxOpenAnotherProject::MessageBoxOpenAnotherProject()
    : Filler("") {
}

void MessageBoxOpenAnotherProject::commonScenario() {
    QWidget* activeModal = GTWidget::getActiveModalWidget();
    auto messageBox = qobject_cast<QMessageBox*>(activeModal);
    GT_CHECK(messageBox != NULL, "messageBox is NULL");

    QAbstractButton* button = messageBox->findChild<QAbstractButton*>("New Window");
    GT_CHECK(button != NULL, "There are no New Window buttons in messagebox");

    button = messageBox->findChild<QAbstractButton*>("This Window");
    GT_CHECK(button != NULL, "There are no This Window buttons in messagebox");

    button = messageBox->button(QMessageBox::Abort);
    GT_CHECK(button != NULL, "There are no Abort buttons in messagebox");

    GTWidget::click(button);
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "MessageBoxOpenAnotherProject"

InputIntFiller::InputIntFiller(int value)
    : Filler(""),
      value(value) {
}

void InputIntFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    auto spinBox = GTWidget::findSpinBox("", dialog);

    GTSpinBox::setValue(spinBox, value);

    GTWidget::click(GTWidget::findButtonByText("OK", dialog));
}

#undef GT_CLASS_NAME

}  // namespace HI
