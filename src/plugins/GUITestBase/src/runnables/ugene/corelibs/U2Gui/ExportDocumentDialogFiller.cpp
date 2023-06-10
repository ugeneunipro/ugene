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
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

#include "ExportDocumentDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "ExportDocumentDialogFiller"
ExportDocumentDialogFiller::ExportDocumentDialogFiller(const QString& _path, const QString& _name, ExportDocumentDialogFiller::FormatToUse _format, bool compressFile, bool addToProject, GTGlobals::UseMethod method)
    : Filler("ExportDocumentDialog"),
      path(_path), name(_name), useMethod(method), format(_format), compressFile(compressFile), addToProject(addToProject) {
    if (!path.isEmpty()) {
        path = GTFileDialog::toAbsoluteNativePath(_path, true);
    }

    comboBoxItems[BAM] = "BAM";
    comboBoxItems[CLUSTALW] = "CLUSTALW";
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[GFF] = "GFF";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxItems[MEGA] = "Mega";
    comboBoxItems[MSF] = "MSF";
    comboBoxItems[NWK] = "Newick Standard";
    comboBoxItems[TEXT] = "Plain text";
    comboBoxItems[UGENEDB] = "UGENE Database";
    comboBoxItems[VCF] = "VCFv4";
    comboBoxItems[VectorNTI] = "Vector NTI sequence";
}

#define GT_METHOD_NAME "commonScenario"
void ExportDocumentDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (!path.isEmpty()) {
        GTLineEdit::setText("fileNameEdit", path + name, dialog);

        auto comboBox = GTWidget::findComboBox("formatCombo", dialog);
        int index = comboBox->findText(comboBoxItems[format]);

        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
        GTComboBox::selectItemByIndex(comboBox, index, useMethod);

        auto compressCheckBox = GTWidget::findCheckBox("compressCheck", dialog);
        GTCheckBox::setChecked(compressCheckBox, compressFile);

        auto addCheckBox = GTWidget::findCheckBox("addToProjCheck", dialog);
        GTCheckBox::setChecked(addCheckBox, addToProject);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
