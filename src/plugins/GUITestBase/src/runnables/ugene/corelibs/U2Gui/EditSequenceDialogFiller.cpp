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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include "EditSequenceDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::insertSequenceFiller"
InsertSequenceFiller::InsertSequenceFiller(HI::GUITestOpStatus& _os, const QString& _pasteDataHere, RegionResolvingMode _regionResolvingMode, int _insertPosition, const QString& _documentLocation, documentFormat _format, bool _saveToNewFile, bool _mergeAnnotations, GTGlobals::UseMethod method, bool _wrongInput, bool recalculateQuals)
    : Filler(_os, "EditSequenceDialog"), pasteDataHere(_pasteDataHere), regionResolvingMode(_regionResolvingMode), insertPosition(_insertPosition),
      documentLocation(_documentLocation), format(_format), saveToNewFile(_saveToNewFile), mergeAnnotations(_mergeAnnotations),
      useMethod(method), wrongInput(_wrongInput), recalculateQuals(recalculateQuals) {
    if (!documentLocation.isEmpty()) {
        documentLocation = GTFileDialog::toAbsoluteNativePath(documentLocation);
    }
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
    mergeAnnotations = _mergeAnnotations;
}

#define GT_METHOD_NAME "commonScenario"
void InsertSequenceFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    auto plainText = GTWidget::findPlainTextEdit(os, "sequenceEdit", dialog);
    GTPlainTextEdit::setPlainText(os, plainText, pasteDataHere);

    QString radioButtonName;
    switch (regionResolvingMode) {
        case Resize:
            radioButtonName = "resizeRB";
            break;
        case Remove:
            radioButtonName = "removeRB";
            break;
        case SplitJoin:
            radioButtonName = "splitRB";
            break;
        case SplitSeparate:
            radioButtonName = "split_separateRB";
            break;
    }

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "recalculateQualsCheckBox"), recalculateQuals);

    auto regionResolvingMode = GTWidget::findRadioButton(os, radioButtonName, dialog);  //"regionResolvingMode");
    GTRadioButton::click(os, regionResolvingMode);

    auto insertPositionSpin = GTWidget::findSpinBox(os, "insertPositionSpin", dialog);
    GTSpinBox::setValue(os, insertPositionSpin, insertPosition, GTGlobals::UseKeyBoard);

    auto checkButton = GTWidget::findGroupBox(os, "saveToAnotherBox", dialog);

    if ((saveToNewFile && !checkButton->isChecked()) || (!saveToNewFile && checkButton->isChecked())) {
        QPoint checkPos;
        switch (useMethod) {
            case GTGlobals::UseMouse:
                checkPos = QPoint(checkButton->rect().left() + 12, checkButton->rect().top() + 12);
                GTMouseDriver::moveTo(checkButton->mapToGlobal(checkPos));
                GTMouseDriver::click();
                break;
            case GTGlobals::UseKey:
                GTWidget::setFocus(os, checkButton);
                GTKeyboardDriver::keyClick(Qt::Key_Space);
                break;
            default:
                break;
        }
    }

    GTGlobals::sleep(1000);

    if (saveToNewFile) {
        auto checkButton1 = GTWidget::findCheckBox(os, "mergeAnnotationsBox", dialog);
        GTCheckBox::setChecked(os, checkButton1, mergeAnnotations);

        GTLineEdit::setText(os, "filepathEdit", documentLocation, dialog);

        auto comboBox = GTWidget::findComboBox(os, "formatBox", dialog);

        int index = comboBox->findText(comboBoxItems[format]);
        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
        GTComboBox::selectItemByIndex(os, comboBox, index);
    }

    if (wrongInput) {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
