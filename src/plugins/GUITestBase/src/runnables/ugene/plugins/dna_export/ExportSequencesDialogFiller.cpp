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
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>

#include "ExportSequencesDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportSelectedRegionFiller"
ExportSelectedRegionFiller::ExportSelectedRegionFiller(HI::GUITestOpStatus& _os, const QString& _path, const QString& _name, bool translate, const QString& seqName, bool saveAllAminoFrames)
    : Filler(_os, "U2__ExportSequencesDialog"), name(_name), seqName(seqName), translate(translate),
      saveAllAminoFrames(saveAllAminoFrames) {
    path = GTFileDialog::toAbsoluteNativePath(_path, true);
}

ExportSelectedRegionFiller::ExportSelectedRegionFiller(GUITestOpStatus& os, const QString& filePath)
    : Filler(os, "U2__ExportSequencesDialog"),
      translate(false),
      saveAllAminoFrames(true) {
    path = QFileInfo(filePath).dir().path() + "/";
    name = QFileInfo(filePath).fileName();
}

ExportSelectedRegionFiller::ExportSelectedRegionFiller(HI::GUITestOpStatus& os, CustomScenario* customScenario)
    : Filler(os, "U2__ExportSequencesDialog", customScenario),
      translate(false), saveAllAminoFrames(true) {
}

#define GT_METHOD_NAME "commonScenario"
void ExportSelectedRegionFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "fileNameEdit", path + name, dialog);

    if (!seqName.isEmpty()) {
        auto customSeqCheckBox = GTWidget::findCheckBox(os, "customSeqNameBox", dialog);
        GTCheckBox::setChecked(os, customSeqCheckBox, true);
        GTLineEdit::setText(os, "sequenceNameEdit", seqName, dialog);
    }

    auto translateButton = GTWidget::findCheckBox(os, "translateButton");
    GTCheckBox::setChecked(os, translateButton, translate);

    if (translate) {
        auto allTFramesButton = GTWidget::findCheckBox(os, "allTFramesButton");
        GTCheckBox::setChecked(os, allTFramesButton, saveAllAminoFrames);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

void ExportSelectedRegionFiller::setPath(const QString& value) {
    path = value;
}

void ExportSelectedRegionFiller::setName(const QString& value) {
    name = value;
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
ExportSequenceOfSelectedAnnotationsFiller::ExportSequenceOfSelectedAnnotationsFiller(HI::GUITestOpStatus& _os,
                                                                                     const QString& _path,
                                                                                     FormatToUse _format,
                                                                                     MergeOptions _options,
                                                                                     int _gapLength,
                                                                                     bool _addDocToProject,
                                                                                     bool _exportWithAnnotations,
                                                                                     GTGlobals::UseMethod method,
                                                                                     bool _translate)
    : Filler(_os, "U2__ExportSequencesDialog"), gapLength(_gapLength), format(_format), addToProject(_addDocToProject),
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

#define GT_METHOD_NAME "commonScenario"
void ExportSequenceOfSelectedAnnotationsFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "fileNameEdit", path, dialog);

    GTGlobals::sleep(200);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");
    int index = comboBox->findText(comboBoxItems[format]);

    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);

    GTGlobals::sleep(200);

    auto projectCheckBox = GTWidget::findCheckBox(os, "addToProjectBox", dialog);
    GTCheckBox::setChecked(os, projectCheckBox, addToProject);

    GTGlobals::sleep(200);

    auto annotationsCheckBox = GTWidget::findCheckBox(os, "withAnnotationsBox", dialog);
    if (annotationsCheckBox->isEnabled()) {
        GTCheckBox::setChecked(os, annotationsCheckBox, exportWithAnnotations);
    }

    GTGlobals::sleep(200);

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "translateButton"), translate);

    auto mergeButton = GTWidget::findRadioButton(os, mergeRadioButtons[options], dialog);

    if (mergeButton->isEnabled()) {
        GTRadioButton::click(os, mergeButton);
    }

    GTGlobals::sleep(200);

    if (gapLength) {
        auto mergeSpinBox = GTWidget::findSpinBox(os, "mergeSpinBox", dialog);
        GTSpinBox::setValue(os, mergeSpinBox, gapLength, useMethod);
    }

    GTGlobals::sleep(200);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
