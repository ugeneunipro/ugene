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

#include "ExportSelectedSequenceFromAlignmentDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::exportSelectedSequenceFromAlignment"

ExportSelectedSequenceFromAlignment::ExportSelectedSequenceFromAlignment(HI::GUITestOpStatus& _os, const QString& _path, documentFormat _format, bool _keepGaps, bool _addToProj)
    : Filler(_os, "U2__SaveSelectedSequenceFromMSADialog"),
      path(_path),
      format(_format),
      keepGaps(_keepGaps),
      addToProj(_addToProj) {
    comboBoxItems[EMBL] = "EMBL";
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[FASTQ] = "FASTQ";
    comboBoxItems[GFF] = "GFF";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxItems[Swiss_Prot] = "Swiss_Prot";
    comboBoxItems[Ugene_db] = "UGENE Database";
}

ExportSelectedSequenceFromAlignment::ExportSelectedSequenceFromAlignment(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "U2__SaveSelectedSequenceFromMSADialog", scenario),
      format(EMBL),
      keepGaps(false),
      addToProj(false) {
}

#define GT_METHOD_NAME "run"
void ExportSelectedSequenceFromAlignment::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QLineEdit* lineEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(lineEdit != nullptr, "line edit not found");
    GTLineEdit::setText(os, lineEdit, path);

    auto comboBox = GTWidget::findComboBox(os, "formatCombo", dialog);

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(os, comboBox, index);

    auto addToProjectBox = GTWidget::findCheckBox(os, "addToProjectBox", dialog);
    GTCheckBox::setChecked(os, addToProjectBox, addToProj);

    auto keepGapsBox = GTWidget::findCheckBox(os, "keepGapsBox", dialog);
    GTCheckBox::setChecked(os, keepGapsBox, keepGaps);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
