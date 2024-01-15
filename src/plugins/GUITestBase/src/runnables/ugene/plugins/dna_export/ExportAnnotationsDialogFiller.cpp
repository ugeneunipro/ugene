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

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>

#include "ExportAnnotationsDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportAnnotationsFiller"
ExportAnnotationsFiller::ExportAnnotationsFiller(const QString& _exportToFile, const FileFormat& _format)
    : Filler("U2__ExportAnnotationsDialog"),
      softMode(true),
      format(_format),
      addToProject(false),
      saveSequencesUnderAnnotations(false),
      saveSequenceNames(false),
      useMethod(GTGlobals::UseMouse) {
    init(_exportToFile);
}

ExportAnnotationsFiller::ExportAnnotationsFiller(
    const QString& _exportToFile,
    const FileFormat& _format,
    bool _addToProject,
    bool _saveSequencesUnderAnnotations,
    bool _saveSequenceNames,
    GTGlobals::UseMethod method)
    : Filler("U2__ExportAnnotationsDialog"),
      softMode(false),
      format(_format),
      addToProject(_addToProject),
      saveSequencesUnderAnnotations(_saveSequencesUnderAnnotations),
      saveSequenceNames(_saveSequenceNames),
      useMethod(method) {
    init(_exportToFile);
}

ExportAnnotationsFiller::ExportAnnotationsFiller(CustomScenario* scenario)
    : Filler("U2__ExportAnnotationsDialog", scenario),
      softMode(false),
      format(genbank),
      addToProject(false),
      saveSequencesUnderAnnotations(false),
      saveSequenceNames(false),
      useMethod(GTGlobals::UseMouse) {
}

void ExportAnnotationsFiller::init(const QString& exportToFileParam) {
    exportToFile = QDir::toNativeSeparators(QDir::cleanPath(exportToFileParam));

    comboBoxItems[bed] = "BED";
    comboBoxItems[genbank] = "GenBank";
    comboBoxItems[gff] = "GFF";
    comboBoxItems[gtf] = "GTF";
    comboBoxItems[csv] = "CSV";
    comboBoxItems[ugenedb] = "UGENE Database";
}

void ExportAnnotationsFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("fileNameEdit", exportToFile, dialog);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(comboBox, index, useMethod);
    if (!addToProject) {
        auto addToProjectButton = GTWidget::findCheckBox("addToProjectCheck", dialog);
        if (addToProjectButton->isEnabled()) {
            GTCheckBox::setChecked(addToProjectButton, false);
        }
    }

    if (!softMode) {
        GTCheckBox::setChecked("exportSequenceCheck", saveSequencesUnderAnnotations, dialog);
        GTCheckBox::setChecked("exportSequenceNameCheck", saveSequenceNames, dialog);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

}  // namespace U2
