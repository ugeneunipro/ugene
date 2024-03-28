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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include "InsertRestrictionSiteDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::insertSequenceFiller"

InsertRestrictionSiteDialogFiller::InsertRestrictionSiteDialogFiller(const InsertRestrictionSiteDialogFillerSettings& _settings)
    : Filler("EditSequenceDialog"), settings(_settings) {
    if (!settings.documentLocation.isEmpty()) {
        settings.documentLocation = GTFileDialog::toAbsoluteNativePath(settings.documentLocation);
    }
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
}

void InsertRestrictionSiteDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTCheckBox::setChecked("cbShowEnzymesWithUndefinedShuppliers", settings.showUnknownSuppliers, dialog);

    GTComboBox::selectItemByText("cbChooseEnzyme", dialog, settings.enzymeName, GTGlobals::UseMouse);

    if (!settings.enzymeInfo.isEmpty()) {
        auto enzymeInfoText = GTTextEdit::getText(GTWidget::findTextBrowser("teChoosedEnzymeInfo", dialog));
        GT_CHECK(enzymeInfoText.contains(settings.enzymeInfo), QString("Unexpected enzyme info: %1").arg(enzymeInfoText));
    }

    QString radioButtonName;
    switch (settings.regionResolvingMode) {
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

    GTCheckBox::setChecked(GTWidget::findCheckBox("recalculateQualsCheckBox"), settings.recalculateQuals);

    auto regionResolvingMode = GTWidget::findRadioButton(radioButtonName, dialog);  //"regionResolvingMode");
    GTRadioButton::click(regionResolvingMode);

    if (settings.insertPosition > 0) {
        auto insertPositionLineEdit = GTWidget::findLineEdit("insertPositionLineEdit", dialog);
        GTLineEdit::setText(insertPositionLineEdit, QString::number(settings.insertPosition), GTGlobals::UseKeyBoard);
    }

    auto checkButton = GTWidget::findGroupBox("saveToAnotherBox", dialog);

    if ((settings.saveToNewFile && !checkButton->isChecked()) || (!settings.saveToNewFile && checkButton->isChecked())) {
        QPoint checkPos;
        switch (settings.useMethod) {
            case GTGlobals::UseMouse:
                checkPos = QPoint(checkButton->rect().left() + 12, checkButton->rect().top() + 12);
                GTMouseDriver::moveTo(checkButton->mapToGlobal(checkPos));
                GTMouseDriver::click();
                break;
            case GTGlobals::UseKey:
                GTWidget::setFocus(checkButton);
                GTKeyboardDriver::keyClick(Qt::Key_Space);
                break;
            default:
                break;
        }
    }

    if (settings.saveToNewFile) {
        auto checkButton1 = GTWidget::findCheckBox("mergeAnnotationsBox", dialog);
        GTCheckBox::setChecked(checkButton1, settings.mergeAnnotations);

        GTLineEdit::setText("filepathEdit", settings.documentLocation, dialog);

        auto comboBox = GTWidget::findComboBox("formatBox", dialog);

        int index = comboBox->findText(comboBoxItems[settings.format]);
        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[settings.format]));
        GTComboBox::selectItemByIndex(comboBox, index);
    }

    if (settings.wrongInput) {
        GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
    }
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

    if (settings.cancelIfWrongInput) {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
    }
}

#undef GT_CLASS_NAME



}  // namespace U2
