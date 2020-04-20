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

#include "api/GTSequenceReadingModeDialogUtils.h"
#include "api/GTSequenceReadingModeDialog.h"
#include "GTGlobals.h"
#include <drivers/GTMouseDriver.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <QApplication>
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <primitives/GTRadioButton.h>

#define SEPARATE_MODE "separateMode"
#define MERGE_MODE "mergeMode"
#define INTERAL_GAP "internalGap"
#define FILE_GAP "fileGap"
#define NEW_DOC_NAME "newUrl"
#define SAVE_BOX "saveBox"



namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTSequenceReadingModeDialogUtils"

GTSequenceReadingModeDialogUtils::GTSequenceReadingModeDialogUtils(HI::GUITestOpStatus &os, CustomScenario *scenario) : Filler(os, "MultipleDocumentsReadingModeSelectorController", scenario), dialog(NULL)
{
}

#define GT_METHOD_NAME "run"
void GTSequenceReadingModeDialogUtils::commonScenario()
{
    GTGlobals::sleep(1000);
    QWidget *openDialog = QApplication::activeModalWidget();
    GT_CHECK(openDialog != NULL, "dialog not found");

    dialog = openDialog;

    selectMode();

    if (GTSequenceReadingModeDialog::mode == GTSequenceReadingModeDialog::Merge) {
        setNumSymbolsParts();
        setNumSymbolsFiles();
        setNewDocumentName();
        selectSaveDocument();
    }

    clickButton();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectMode"
void GTSequenceReadingModeDialogUtils::selectMode()
{
    QString buttonName = SEPARATE_MODE;
    if (GTSequenceReadingModeDialog::mode == GTSequenceReadingModeDialog::Merge) {
        buttonName = MERGE_MODE;
    }

    QRadioButton *radioButton = dialog->findChild<QRadioButton*>(buttonName);
    GT_CHECK(radioButton != NULL, "radio button not found");

    if (! radioButton->isChecked()) {
        switch (GTSequenceReadingModeDialog::useMethod) {
        case GTGlobals::UseMouse:
            GTRadioButton::click(os, radioButton);
            break;

        case GTGlobals::UseKey:
            GTWidget::setFocus(os, radioButton);
            GTKeyboardDriver::keyClick(Qt::Key_Space);
            break;
        default:
            break;
        }
    }

    while (! radioButton->isChecked()) {
        GTGlobals::sleep(100);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNumSymbolsParts"
void GTSequenceReadingModeDialogUtils::setNumSymbolsParts()
{
    /*QSpinBox *spinBox = dialog->findChild<QSpinBox*>(INTERAL_GAP);
    GT_CHECK(spinBox != NULL, "spinBox not found");

    changeSpinBoxValue(spinBox, GTSequenceReadingModeDialog::numSymbolParts);*/
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNumSymbolsFiles"
void GTSequenceReadingModeDialogUtils::setNumSymbolsFiles()
{
    QSpinBox *spinBox = dialog->findChild<QSpinBox*>(FILE_GAP);
    GT_CHECK(spinBox != NULL, "spinBox not found");

    changeSpinBoxValue(spinBox, GTSequenceReadingModeDialog::numSymbolFiles);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNewDocumentName"
void GTSequenceReadingModeDialogUtils::setNewDocumentName()
{
    if (GTSequenceReadingModeDialog::newDocName == QString()) {
        return;
    }

    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>(NEW_DOC_NAME);
    GT_CHECK(lineEdit != NULL, "lineEdit not found");

    GTLineEdit::clear(os, lineEdit);
    GTLineEdit::setText(os, lineEdit, GTSequenceReadingModeDialog::newDocName);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSaveDocument"
void GTSequenceReadingModeDialogUtils::selectSaveDocument()
{
    QCheckBox *saveBox = dialog->findChild<QCheckBox*>(SAVE_BOX);
    GT_CHECK(saveBox != NULL, "save check box not found");

    if (GTSequenceReadingModeDialog::saveDocument != saveBox->isChecked()) {
        switch (GTSequenceReadingModeDialog::useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(saveBox->mapToGlobal(QPoint(saveBox->rect().left() + 10, saveBox->rect().height() / 2)));
            GTMouseDriver::click();
            break;

        case GTGlobals::UseKey:
            while (! saveBox->hasFocus()) {
                GTKeyboardDriver::keyClick(Qt::Key_Tab);
                GTGlobals::sleep(100);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Space);
        default:
            break;
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButton"
void GTSequenceReadingModeDialogUtils::clickButton()
{
    QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox*>(QString::fromUtf8("buttonBox"));
    GT_CHECK(buttonBox != NULL, "button box not found");

    QList<QAbstractButton*> buttonList = buttonBox->buttons();
    GT_CHECK(buttonList.size() != 0, "button not found");

    QPushButton *btn = NULL;
    foreach(QAbstractButton* b, buttonList) {
        if (buttonBox->standardButton(b) == GTSequenceReadingModeDialog::button) {
            btn =  qobject_cast<QPushButton*> (b);
            break;
        }
    }
    GT_CHECK(btn != NULL, "button not found");

    GTWidget::click(os, btn/*, UseMethod*/);
}
#undef GT_METHOD_NAME

void GTSequenceReadingModeDialogUtils::changeSpinBoxValue(QSpinBox *sb, int val)
{
    QPoint arrowPos;
    QRect spinBoxRect;

    if (sb->value() != val) {
        switch(GTSequenceReadingModeDialog::useMethod) {
        case GTGlobals::UseMouse:
            spinBoxRect = sb->rect();
            if (val > sb->value()) {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() / 4); // -5 it's needed that area under cursor was clickable
            } else {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() * 3 / 4);
            }

            GTMouseDriver::moveTo(sb->mapToGlobal(arrowPos));
            while (sb->value() != val) {
                GTMouseDriver::click();
                GTGlobals::sleep(100);
            }
            break;

        case GTGlobals::UseKey: {
            Qt::Key key;
            if (val > sb->value()) {
                key = Qt::Key_Up;
            } else {
                key = Qt::Key_Down;
            }

            GTWidget::setFocus(os, sb);
            while (sb->value() != val) {
                GTKeyboardDriver::keyClick( key);
                GTGlobals::sleep(100);
            }
        }
        default:
            break;
        }
    }
}

#undef GT_CLASS_NAME

} // namespace
