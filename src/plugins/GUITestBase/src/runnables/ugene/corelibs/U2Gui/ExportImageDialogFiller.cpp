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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QTableWidget>

#include "ExportImageDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExportImage"

ExportImage::ExportImage(HI::GUITestOpStatus& os, const QString& filePath, const QString& comboValue, int spinValue)
    : Filler(os, "ImageExportForm"),
      filePath(QDir::toNativeSeparators(filePath)),
      comboValue(comboValue),
      spinValue(spinValue) {
}

ExportImage::ExportImage(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "ImageExportForm", scenario),
      spinValue(0) {
}

#define GT_METHOD_NAME "commonScenario"
void ExportImage::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTLineEdit::setText(os, "fileNameEdit", filePath, dialog);

    if (comboValue != "") {
        GTComboBox::selectItemByText(os, "formatsBox", dialog, comboValue);
    }

    if (spinValue) {
        GTSpinBox::setValue(os, "qualitySpinBox", spinValue, GTGlobals::UseKeyBoard, dialog);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::CircularViewExportImage"
#define GT_METHOD_NAME "commonScenario"
void CircularViewExportImage::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTLineEdit::setText(os, "fileNameEdit", filePath, dialog);

    if (!comboValue.isEmpty()) {
        GTComboBox::selectItemByText(os, "formatsBox", dialog, comboValue);
    }

    if (spinValue) {
        GTSpinBox::setValue(os, "qualitySpinBox", spinValue, GTGlobals::UseKeyBoard, dialog);
    }

    if (!exportedSequenceName.isEmpty()) {
        GTComboBox::selectItemByText(os, "Exported_sequence_combo", dialog, exportedSequenceName);
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportMsaImage"
#define GT_METHOD_NAME "commonScenario"
void ExportMsaImage::commonScenario() {
    GT_CHECK((exportWholeAlignment && exportCurrentSelection) != true, "Wrong filler parameters");

    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (!exportWholeAlignment) {
        if (!exportCurrentSelection) {
            GTUtilsDialog::waitForDialog(os, new SelectSubalignmentFiller(os, region));
        }
        GTComboBox::selectItemByText(os, "comboBox", dialog, "Custom region");
    }

    GTCheckBox::setChecked(os, "exportSeqNames", settings.includeNames, dialog);
    GTCheckBox::setChecked(os, "exportConsensus", settings.includeConsensus, dialog);
    GTCheckBox::setChecked(os, "exportRuler", settings.includeRuler, dialog);
    GTLineEdit::setText(os, "fileNameEdit", filePath, dialog);

    if (!comboValue.isEmpty()) {
        GTComboBox::selectItemByText(os, "formatsBox", dialog, comboValue);
    }

    if (spinValue) {
        GTSpinBox::setValue(os, "qualitySpinBox", spinValue, GTGlobals::UseKeyBoard, dialog);
    }
    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportSequenceImage"
#define GT_METHOD_NAME "commonScenario"
void ExportSequenceImage::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QString radioButtonName;
    switch (settings.type) {
        case CurrentView:
            radioButtonName = "currentViewButton";
            break;
        case ZoomedView:
            radioButtonName = "zoomButton";
            break;
        case DetailsView:
            radioButtonName = "detailsButton";
            break;
    }

    GTRadioButton::click(os, radioButtonName, dialog);

    if (settings.type != CurrentView) {
        // Set region.
        GTLineEdit::setText(os, "start_edit_line", QString::number(settings.region.startPos), dialog);
        GTLineEdit::setText(os, "end_edit_line", QString::number(settings.region.endPos()), dialog);
    }

    GTLineEdit::setText(os, "fileNameEdit", filePath, dialog);

    if (comboValue != "") {
        GTComboBox::selectItemByText(os, "formatsBox", dialog, comboValue);
    }

    if (spinValue) {
        GTSpinBox::setValue(os, "qualitySpinBox", spinValue, GTGlobals::UseKeyBoard, dialog);
    }

    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::SelectSubalignmentFiller"
#define GT_METHOD_NAME "commonScenario"
void SelectSubalignmentFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTSpinBox::setValue(os, "startLineEdit", msaRegion.region.startPos, GTGlobals::UseKeyBoard, dialog);
    GTSpinBox::setValue(os, "endLineEdit", msaRegion.region.endPos(), GTGlobals::UseKeyBoard, dialog);

    GTWidget::click(os, GTWidget::findWidget(os, "noneButton", dialog));

    auto table = GTWidget::findTableWidget(os, "sequencesTableWidget", dialog);

    QPoint p = table->geometry().topRight();
    p.setX(p.x() - 2);
    p.setY(p.y() + 2);
    p = dialog->mapToGlobal(p);

    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    for (int i = 0; i < table->rowCount(); i++) {
        for (const QString& sequence : qAsConst(msaRegion.sequences)) {
            auto box = qobject_cast<QCheckBox*>(table->cellWidget(i, 0));
            GT_CHECK(box != nullptr, "Not a QCheckBox cell");
            if (sequence == box->text()) {
                GTWidget::scrollToIndex(os, table, table->model()->index(i, 0));
                GT_CHECK(box->isEnabled(), QString("%1 box is disabled").arg(box->text()));
                GTCheckBox::setChecked(os, box, true);
            }
        }
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

ImageExportFormFiller::ImageExportFormFiller(HI::GUITestOpStatus& os, const Parameters& parameters)
    : Filler(os, "ImageExportForm"), parameters(parameters) {
}

#define GT_CLASS_NAME "GTUtilsDialog::ImageExportFormFiller"
#define GT_METHOD_NAME "commonScenario"

void ImageExportFormFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTLineEdit::setText(os, "fileNameEdit", QDir::toNativeSeparators(parameters.fileName), dialog);

    auto formatsBox = GTWidget::findComboBox(os, "formatsBox", dialog);
    GTComboBox::selectItemByText(os, formatsBox, parameters.format);

    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
