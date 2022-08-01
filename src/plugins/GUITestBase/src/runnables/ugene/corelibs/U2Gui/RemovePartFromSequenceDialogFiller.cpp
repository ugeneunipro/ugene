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
RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus& _os, QString _range, bool recalculateQuals)
    : Filler(_os, "RemovePartFromSequenceDialog"), range(_range), removeType(Resize), format(FASTA), saveNew(false),
      recalculateQuals(recalculateQuals) {
}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus& _os, RemoveType _removeType, bool _saveNew, const QString& _saveToFile, FormatToUse _format)
    : Filler(_os, "RemovePartFromSequenceDialog"), removeType(_removeType), format(_format), saveNew(_saveNew), recalculateQuals(false) {
    saveToFile = GTFileDialog::toAbsoluteNativePath(_saveToFile);
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus& _os, RemoveType _removeType)
    : Filler(_os, "RemovePartFromSequenceDialog"), removeType(_removeType), format(FASTA), recalculateQuals(false) {
}

#define GT_METHOD_NAME "commonScenario"
void RemovePartFromSequenceDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (!range.isEmpty()) {
        GTLineEdit::setText(os, "removeLocationEdit", range, dialog);
    }
    if (removeType == Resize) {
        auto resizeRB = GTWidget::findRadioButton(os, "resizeRB", dialog);
        GTRadioButton::click(os, resizeRB);
    } else {
        auto removeRB = GTWidget::findRadioButton(os, "removeRB", dialog);
        GTRadioButton::click(os, removeRB);
    }

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "recalculateQualsCheckBox"), recalculateQuals);

    GTGlobals::sleep(1000);
    if (saveNew) {
        auto saveToAnotherBox = GTWidget::findGroupBox(os, "saveToAnotherBox", dialog);
        GTGroupBox::setChecked(os, saveToAnotherBox, true);
        GTLineEdit::setText(os, "filepathEdit", saveToFile, dialog);
    }
    if (format != FASTA) {
        auto formatBox = GTWidget::findComboBox(os, "formatBox", dialog);
        int index = formatBox->findText(comboBoxItems[format]);
        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

        GTComboBox::selectItemByIndex(os, formatBox, index);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
