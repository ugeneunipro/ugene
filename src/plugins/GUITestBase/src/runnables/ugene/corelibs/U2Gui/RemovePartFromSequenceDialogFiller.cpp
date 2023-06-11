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
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>

#include "RemovePartFromSequenceDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::RemovePartFromSequenceDialogFiller"
RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(QString _range, bool recalculateQuals)
    : Filler("RemovePartFromSequenceDialog"), range(_range), removeType(Resize), format(FASTA), saveNew(false),
      recalculateQuals(recalculateQuals) {
}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(RemoveType _removeType, bool _saveNew, const QString& _saveToFile, FormatToUse _format)
    : Filler("RemovePartFromSequenceDialog"), removeType(_removeType), format(_format), saveNew(_saveNew), recalculateQuals(false) {
    saveToFile = GTFileDialog::toAbsoluteNativePath(_saveToFile);
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(RemoveType _removeType)
    : Filler("RemovePartFromSequenceDialog"), removeType(_removeType), format(FASTA), recalculateQuals(false) {
}

#define GT_METHOD_NAME "commonScenario"
void RemovePartFromSequenceDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (!range.isEmpty()) {
        GTLineEdit::setText("removeLocationEdit", range, dialog);
    }
    if (removeType == Resize) {
        auto resizeRB = GTWidget::findRadioButton("resizeRB", dialog);
        GTRadioButton::click(resizeRB);
    } else {
        auto removeRB = GTWidget::findRadioButton("removeRB", dialog);
        GTRadioButton::click(removeRB);
    }

    GTCheckBox::setChecked(GTWidget::findCheckBox("recalculateQualsCheckBox"), recalculateQuals);

    GTGlobals::sleep(1000);
    if (saveNew) {
        auto saveToAnotherBox = GTWidget::findGroupBox("saveToAnotherBox", dialog);
        GTGroupBox::setChecked(saveToAnotherBox, true);
        GTLineEdit::setText("filepathEdit", saveToFile, dialog);
    }
    if (format != FASTA) {
        auto formatBox = GTWidget::findComboBox("formatBox", dialog);
        int index = formatBox->findText(comboBoxItems[format]);
        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

        GTComboBox::selectItemByIndex(formatBox, index);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
