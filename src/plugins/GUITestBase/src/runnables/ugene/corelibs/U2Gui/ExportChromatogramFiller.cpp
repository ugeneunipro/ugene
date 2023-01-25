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

#include "ExportChromatogramFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExportChromatogramFiller"
ExportChromatogramFiller::ExportChromatogramFiller(HI::GUITestOpStatus& _os, const QString& _path, const QString& _name, ExportChromatogramFiller::FormatToUse _format, bool reversed, bool complement, bool addDocumentToProject, GTGlobals::UseMethod method)
    : Filler(_os, "ExportChromatogramDialog"), name(_name), useMethod(method), format(_format), reversed(reversed),
      complement(complement), addDocumentToProject(addDocumentToProject) {
    path = _path;
    comboBoxItems[SCF] = "SCF";
}

#define GT_METHOD_NAME "commonScenario"
void ExportChromatogramFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QLineEdit* lineEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(lineEdit != nullptr, "line edit not found");

    GTLineEdit::setText(os, lineEdit, path + name);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);

    if (addDocumentToProject) {
        auto checkBox = GTWidget::findCheckBox(os, "addToProjectBox", dialog);
        GTCheckBox::setChecked(os, checkBox, addDocumentToProject);
    }

    if (reversed) {
        auto checkBoxReversed = GTWidget::findCheckBox(os, "reverseBox", dialog);
        GTCheckBox::setChecked(os, checkBoxReversed, reversed);
    }

    if (complement) {
        auto checkBoxComplement = GTWidget::findCheckBox(os, "complementBox", dialog);
        GTCheckBox::setChecked(os, checkBoxComplement, complement);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
