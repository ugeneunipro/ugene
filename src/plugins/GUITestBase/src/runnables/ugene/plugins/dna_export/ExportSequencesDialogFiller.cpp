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
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>

#include "ExportSequencesDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportSelectedRegionFiller"
ExportSelectedRegionFiller::ExportSelectedRegionFiller(const QString& _path, const QString& _name, bool translate, const QString& seqName, bool saveAllAminoFrames)
    : Filler("U2__ExportSequencesDialog"), name(_name), seqName(seqName), translate(translate),
      saveAllAminoFrames(saveAllAminoFrames) {
    path = GTFileDialog::toAbsoluteNativePath(_path, true);
}

ExportSelectedRegionFiller::ExportSelectedRegionFiller(const QString& filePath)
    : Filler("U2__ExportSequencesDialog"),
      translate(false),
      saveAllAminoFrames(true) {
    path = QFileInfo(filePath).dir().path() + "/";
    name = QFileInfo(filePath).fileName();
}

ExportSelectedRegionFiller::ExportSelectedRegionFiller(CustomScenario* customScenario)
    : Filler("U2__ExportSequencesDialog", customScenario),
      translate(false), saveAllAminoFrames(true) {
}

void ExportSelectedRegionFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("fileNameEdit", path + name, dialog);

    if (!seqName.isEmpty()) {
        auto customSeqCheckBox = GTWidget::findCheckBox("customSeqNameBox", dialog);
        GTCheckBox::setChecked(customSeqCheckBox, true);
        GTLineEdit::setText("sequenceNameEdit", seqName, dialog);
    }

    auto translateButton = GTWidget::findCheckBox("translateButton");
    GTCheckBox::setChecked(translateButton, translate);

    if (translate) {
        auto allTFramesButton = GTWidget::findCheckBox("allTFramesButton");
        GTCheckBox::setChecked(allTFramesButton, saveAllAminoFrames);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void ExportSelectedRegionFiller::setPath(const QString& value) {
    path = value;
}

void ExportSelectedRegionFiller::setName(const QString& value) {
    name = value;
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
ExportSequenceOfSelectedAnnotationsFiller::ExportSequenceOfSelectedAnnotationsFiller(
    const QString& _path,
    FormatToUse _format,
    MergeOptions _options,
    int _gapLength,
    bool _addDocToProject,
    bool _exportWithAnnotations,
    GTGlobals::UseMethod method,
    bool _translate)
    : Filler("U2__ExportSequencesDialog"), gapLength(_gapLength), format(_format), addToProject(_addDocToProject),
      exportWithAnnotations(false), options(_options), useMethod(method), translate(_translate) {
    exportWithAnnotations = _exportWithAnnotations;
    path = GTFileDialog::toAbsoluteNativePath(_path);

    comboBoxItems[Fasta] = "FASTA";
    comboBoxItems[Fastq] = "FASTQ";
    comboBoxItems[Gff] = "GFF";
    comboBoxItems[Genbank] = "GenBank";

    mergeRadioButtons[SaveAsSeparate] = "separateButton";
    mergeRadioButtons[Merge] = "mergeButton";
}

void ExportSequenceOfSelectedAnnotationsFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("fileNameEdit", path, dialog);

    GTGlobals::sleep(200);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");
    int index = comboBox->findText(comboBoxItems[format]);

    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(comboBox, index, useMethod);

    GTGlobals::sleep(200);

    auto projectCheckBox = GTWidget::findCheckBox("addToProjectBox", dialog);
    GTCheckBox::setChecked(projectCheckBox, addToProject);

    GTGlobals::sleep(200);

    auto annotationsCheckBox = GTWidget::findCheckBox("withAnnotationsBox", dialog);
    if (annotationsCheckBox->isEnabled()) {
        GTCheckBox::setChecked(annotationsCheckBox, exportWithAnnotations);
    }

    GTGlobals::sleep(200);

    GTCheckBox::setChecked(GTWidget::findCheckBox("translateButton"), translate);

    auto mergeButton = GTWidget::findRadioButton(mergeRadioButtons[options], dialog);

    if (mergeButton->isEnabled()) {
        GTRadioButton::click(mergeButton);
    }

    GTGlobals::sleep(200);

    if (gapLength) {
        auto mergeSpinBox = GTWidget::findSpinBox("mergeSpinBox", dialog);
        GTSpinBox::setValue(mergeSpinBox, gapLength, useMethod);
    }

    GTGlobals::sleep(200);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

}  // namespace U2
