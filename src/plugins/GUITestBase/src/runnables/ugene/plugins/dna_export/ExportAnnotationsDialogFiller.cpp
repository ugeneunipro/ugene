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
ExportAnnotationsFiller::ExportAnnotationsFiller(const QString& exportToFile, fileFormat format, HI::GUITestOpStatus& os)
    : Filler(os, "U2__ExportAnnotationsDialog"),
      softMode(true),
      format(format),
      addToProject(false),
      saveSequencesUnderAnnotations(false),
      saveSequenceNames(false),
      useMethod(GTGlobals::UseMouse) {
    init(exportToFile);
}

ExportAnnotationsFiller::ExportAnnotationsFiller(HI::GUITestOpStatus& _os,
                                                 const QString& _exportToFile,
                                                 fileFormat _format,
                                                 bool addToProject,
                                                 bool saveSequencesUnderAnnotations,
                                                 bool saveSequenceNames,
                                                 GTGlobals::UseMethod method)
    : Filler(_os, "U2__ExportAnnotationsDialog"),
      softMode(false),
      format(_format),
      addToProject(addToProject),
      saveSequencesUnderAnnotations(saveSequencesUnderAnnotations),
      saveSequenceNames(saveSequenceNames),
      useMethod(method) {
    init(_exportToFile);
}

ExportAnnotationsFiller::ExportAnnotationsFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "U2__ExportAnnotationsDialog", scenario),
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

#define GT_METHOD_NAME "commonScenario"
void ExportAnnotationsFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "fileNameEdit", exportToFile, dialog);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);
    if (!addToProject) {
        auto addToProjectButton = GTWidget::findCheckBox(os, "addToProjectCheck", dialog);
        if (addToProjectButton->isEnabled()) {
            GTCheckBox::setChecked(os, addToProjectButton, false);
        }
    }

    if (!softMode) {
        auto checkButton = GTWidget::findCheckBox(os, "exportSequenceCheck", dialog);
        GTCheckBox::setChecked(os, checkButton, saveSequencesUnderAnnotations);

        checkButton = GTWidget::findCheckBox(os, "exportSequenceNameCheck", dialog);
        GTCheckBox::setChecked(os, checkButton, saveSequenceNames);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
