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

#include "api/GTSequenceReadingModeDialogUtils.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "GTGlobals.h"
#include "api/GTSequenceReadingModeDialog.h"

#define SEPARATE_MODE "separateMode"
#define MERGE_MODE "mergeMode"
#define FILE_GAP "fileGap"
#define NEW_DOC_NAME "newUrl"
#define SAVE_BOX "saveBox"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTSequenceReadingModeDialogUtils"

GTSequenceReadingModeDialogUtils::GTSequenceReadingModeDialogUtils(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "MultipleDocumentsReadingModeSelectorController", scenario), dialog(nullptr) {
}

#define GT_METHOD_NAME "run"
void GTSequenceReadingModeDialogUtils::commonScenario() {
    QWidget* openDialog = GTWidget::getActiveModalWidget(os);

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
void GTSequenceReadingModeDialogUtils::selectMode() {
    QString buttonName = GTSequenceReadingModeDialog::mode == GTSequenceReadingModeDialog::Merge ? MERGE_MODE : SEPARATE_MODE;
    auto radioButton = GTWidget::findRadioButton(os, buttonName, dialog);

    if (!radioButton->isChecked()) {
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
    GTRadioButton::checkIsChecked(os, radioButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNumSymbolsParts"
void GTSequenceReadingModeDialogUtils::setNumSymbolsParts() {
    /*QSpinBox *spinBox = dialog->findChild<QSpinBox*>(INTERAL_GAP);
    GT_CHECK(spinBox != NULL, "spinBox not found");

    changeSpinBoxValue(spinBox, GTSequenceReadingModeDialog::numSymbolParts);*/
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNumSymbolsFiles"
void GTSequenceReadingModeDialogUtils::setNumSymbolsFiles() {
    auto spinBox = GTWidget::findSpinBox(os, FILE_GAP, dialog);

    changeSpinBoxValue(spinBox, GTSequenceReadingModeDialog::numSymbolFiles);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNewDocumentName"
void GTSequenceReadingModeDialogUtils::setNewDocumentName() {
    if (GTSequenceReadingModeDialog::newDocName == QString()) {
        return;
    }

    GTLineEdit::setText(os, NEW_DOC_NAME, GTSequenceReadingModeDialog::newDocName, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSaveDocument"
void GTSequenceReadingModeDialogUtils::selectSaveDocument() {
    auto saveBox = GTWidget::findCheckBox(os, SAVE_BOX, dialog);

    if (GTSequenceReadingModeDialog::saveDocument != saveBox->isChecked()) {
        switch (GTSequenceReadingModeDialog::useMethod) {
            case GTGlobals::UseMouse:
                GTMouseDriver::moveTo(saveBox->mapToGlobal(QPoint(saveBox->rect().left() + 10, saveBox->rect().height() / 2)));
                GTMouseDriver::click();
                break;

            case GTGlobals::UseKey:
                while (!saveBox->hasFocus()) {
                    GTKeyboardDriver::keyClick(Qt::Key_Tab);
                }
                GTKeyboardDriver::keyClick(Qt::Key_Space);
            default:
                break;
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButton"
void GTSequenceReadingModeDialogUtils::clickButton() {
    auto buttonBox = GTWidget::findDialogButtonBox(os, "buttonBox", dialog);

    QList<QAbstractButton*> buttonList = buttonBox->buttons();
    GT_CHECK(buttonList.size() != 0, "button not found");

    QPushButton* btn = nullptr;
    foreach (QAbstractButton* b, buttonList) {
        if (buttonBox->standardButton(b) == GTSequenceReadingModeDialog::button) {
            btn = qobject_cast<QPushButton*>(b);
            break;
        }
    }
    GT_CHECK(btn != nullptr, "button not found");

    GTWidget::click(os, btn /*, UseMethod*/);
}
#undef GT_METHOD_NAME

void GTSequenceReadingModeDialogUtils::changeSpinBoxValue(QSpinBox* sb, int val) {
    QPoint arrowPos;
    QRect spinBoxRect;

    if (sb->value() != val) {
        int maxClicksCount = 20;  // Avoid tests with too many clicks.
        int currentClickCount = 0;
        switch (GTSequenceReadingModeDialog::useMethod) {
            case GTGlobals::UseMouse: {
                spinBoxRect = sb->rect();
                if (val > sb->value()) {
                    arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() / 4);  // -5 it's needed that area under cursor was clickable
                } else {
                    arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() * 3 / 4);
                }

                GTMouseDriver::moveTo(sb->mapToGlobal(arrowPos));
                while (sb->value() != val && currentClickCount++ < maxClicksCount) {
                    GTMouseDriver::click();
                }
                CHECK_SET_ERR(sb->value() == val, "Failed to set correct value in spinbox with mouse");
                break;
            }
            case GTGlobals::UseKey: {
                Qt::Key key;
                key = val > sb->value() ? Qt::Key_Up : Qt::Key_Down;
                GTWidget::setFocus(os, sb);
                while (sb->value() != val && currentClickCount++ < maxClicksCount) {
                    GTKeyboardDriver::keyClick(key);
                }
                CHECK_SET_ERR(sb->value() == val, "Failed to set correct value in spinbox with keyboard");
            }
            default:
                break;
        }
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
