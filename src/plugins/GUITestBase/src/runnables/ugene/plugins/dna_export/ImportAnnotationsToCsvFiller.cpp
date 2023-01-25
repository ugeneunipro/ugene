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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QTableWidget>

#include "ImportAnnotationsToCsvFiller.h"
#include "system/GTClipboard.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ImportAnnotationsToCsvFiller"

ImportAnnotationsToCsvFiller::ImportAnnotationsToCsvFiller(HI::GUITestOpStatus& _os,
                                                           const QString& _fileToRead,
                                                           const QString& _resultFile,
                                                           ImportAnnotationsToCsvFiller::fileFormat _format,
                                                           bool _addResultFileToProject,
                                                           bool _columnSeparator,
                                                           const QString& _separator,
                                                           int _numberOfLines,
                                                           const QString& _skipAllLinesStartsWith,
                                                           bool _interpretMultipleAsSingle,
                                                           bool _removeQuotesButton,
                                                           const QString& _defaultAnnotationName,
                                                           const RoleParameters& _roleParameters,
                                                           GTGlobals::UseMethod method)
    : Filler(_os, "ImportAnnotationsFromCSVDialog"),
      fileToRead(_fileToRead),
      resultFile(QDir::toNativeSeparators(_resultFile)),
      format(_format),
      addResultFileToProject(_addResultFileToProject),
      columnSeparator(_columnSeparator),
      numberOfLines(_numberOfLines),
      separator(_separator),
      skipAllLinesStartsWith(_skipAllLinesStartsWith),
      interpretMultipleAsSingle(_interpretMultipleAsSingle),
      removeQuotesButton(_removeQuotesButton),
      defaultAnnotationName(_defaultAnnotationName),
      roleParameters(_roleParameters),
      useMethod(method) {
    comboBoxItems[BED] = "BED";
    comboBoxItems[EMBL] = "EMBL";
    comboBoxItems[FPKM] = "FPKM";
    comboBoxItems[GFF] = "GFF";
    comboBoxItems[GTF] = "GTF";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxItems[Swiss_Prot] = "Swiss_Prot";
}

ImportAnnotationsToCsvFiller::ImportAnnotationsToCsvFiller(GUITestOpStatus& _os, CustomScenario* c)
    : Filler(_os, "ImportAnnotationsFromCSVDialog", c),
      format(BED),
      addResultFileToProject(false),
      columnSeparator(false),
      numberOfLines(0),
      interpretMultipleAsSingle(false),
      removeQuotesButton(false),
      useMethod(GTGlobals::UseKey) {
}

#define GT_METHOD_NAME "run"
void RoleFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    auto addOffsetCheckBox = GTWidget::findCheckBox(os, "startOffsetCheck", dialog);
    auto addOffsetSpinBox = GTWidget::findSpinBox(os, "startOffsetValue", dialog);
    auto endPosCheckBox = GTWidget::findCheckBox(os, "endInclusiveCheck", dialog);
    auto strandMarkCheckBox = GTWidget::findCheckBox(os, "complValueCheck", dialog);
    auto markValueLineEdit = GTWidget::findLineEdit(os, "complValueEdit", dialog);
    auto qualifierLineEdit = GTWidget::findLineEdit(os, "qualifierNameEdit", dialog);

    GTGlobals::sleep(300);
    ImportAnnotationsToCsvFiller::StartParameter* startP = dynamic_cast<ImportAnnotationsToCsvFiller::StartParameter*>(parameter);
    if (startP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "startRB", dialog));
        GTCheckBox::setChecked(os, addOffsetCheckBox, startP->addOffset);
        if (startP->addOffset) {
            GTSpinBox::setValue(os, addOffsetSpinBox, startP->numberOfBp);
        }
    }

    ImportAnnotationsToCsvFiller::EndParameter* endP = dynamic_cast<ImportAnnotationsToCsvFiller::EndParameter*>(parameter);
    if (endP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "endRB", dialog));
        GTCheckBox::setChecked(os, endPosCheckBox, endP->endPos);
    }

    ImportAnnotationsToCsvFiller::LengthParameter* lenghtP = dynamic_cast<ImportAnnotationsToCsvFiller::LengthParameter*>(parameter);
    if (lenghtP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "lengthRB", dialog));
    }

    ImportAnnotationsToCsvFiller::StrandMarkParameter* strandMarkP = dynamic_cast<ImportAnnotationsToCsvFiller::StrandMarkParameter*>(parameter);
    if (strandMarkP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "complMarkRB", dialog));
        GTCheckBox::setChecked(os, strandMarkCheckBox, strandMarkP->markValue);
        GTLineEdit::setText(os, markValueLineEdit, strandMarkP->markValueName);
    }

    ImportAnnotationsToCsvFiller::NameParameter* nameP = dynamic_cast<ImportAnnotationsToCsvFiller::NameParameter*>(parameter);
    if (nameP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "nameRB", dialog));
    }

    ImportAnnotationsToCsvFiller::QualifierParameter* qualP = dynamic_cast<ImportAnnotationsToCsvFiller::QualifierParameter*>(parameter);
    if (qualP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "qualifierRB", dialog));
        GTLineEdit::setText(os, qualifierLineEdit, qualP->name);
    }

    ImportAnnotationsToCsvFiller::IgnoreParameter* ignoreP = dynamic_cast<ImportAnnotationsToCsvFiller::IgnoreParameter*>(parameter);
    if (ignoreP) {
        GTRadioButton::click(os, GTWidget::findRadioButton(os, "ignoreRB", dialog));
    }

    delete parameter;
    parameter = nullptr;

    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

class GTTableWidget {
public:
    static QPoint headerItemCenter(HI::GUITestOpStatus& os, QTableWidget* w, int pos) {
        CHECK_SET_ERR_RESULT(w != nullptr, "QTableWidget is NULL", QPoint());

        QTableWidgetItem* widgetItem = w->item(0, pos);
        CHECK_SET_ERR_RESULT(widgetItem != nullptr, "QTableWidgetItem is NULL", QPoint());

        QPoint itemPoint = w->visualItemRect(widgetItem).center();
        return w->mapToGlobal(itemPoint);
    }
};

#define GT_METHOD_NAME "run"
void ImportAnnotationsToCsvFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "readFileName", fileToRead, dialog);

    GTLineEdit::setText(os, "saveFileName", resultFile, dialog);

    QComboBox* comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != nullptr, "ComboBox not found");
    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::selectItemByIndex(os, comboBox, index, useMethod);

    QSpinBox* spinBox = dialog->findChild<QSpinBox*>();
    GT_CHECK(spinBox != nullptr, "SpinBox not found");
    GTSpinBox::setValue(os, spinBox, numberOfLines, useMethod);

    auto checkBox = GTWidget::findCheckBox(os, "addToProjectCheck", dialog);
    GTCheckBox::setChecked(os, checkBox, addResultFileToProject);

    if (columnSeparator) {
        auto columnSeparator = GTWidget::findRadioButton(os, "columnSeparatorRadioButton", dialog);
        GTRadioButton::click(os, columnSeparator);

        GTLineEdit::setText(os, "separatorEdit", separator, dialog, false, true);

        //        GTClipboard::setText(os, separator);
        //        GTWidget::click(os, separatorEdit);
        //        GTKeyboardDriver::keyClick(os, 'v', GTKeyboardDriver::key["ctrl"]);
    } else {
        GTRadioButton::click(os, "scriptRadioButton", dialog);
    }

    GTLineEdit::setText(os, "prefixToSkipEdit", skipAllLinesStartsWith, dialog);

    auto separatorsModeCheckBox = GTWidget::findCheckBox(os, "separatorsModeCheck", dialog);
    GTCheckBox::setChecked(os, separatorsModeCheckBox, interpretMultipleAsSingle);

    auto removeQuotesCheckBox = GTWidget::findCheckBox(os, "removeQuotesCheck", dialog);
    GTCheckBox::setChecked(os, removeQuotesCheckBox, removeQuotesButton);

    GTLineEdit::setText(os, "defaultNameEdit", defaultAnnotationName, dialog);

    auto previewButton = GTWidget::findPushButton(os, "previewButton", dialog);
    GTWidget::click(os, previewButton);
    GTGlobals::sleep();

    auto previewTable = GTWidget::findTableWidget(os, "previewTable", dialog);
    foreach (RoleColumnParameter r, roleParameters) {
        GTUtilsDialog::waitForDialog(os, new RoleFiller(os, r.parameter));
        GT_CHECK(previewTable->item(0, r.column) != nullptr, "Table item not found");
        previewTable->scrollToItem(previewTable->item(0, r.column));
        GTGlobals::sleep(200);
        GTMouseDriver::moveTo(GTTableWidget::headerItemCenter(os, previewTable, r.column));
        GTMouseDriver::click();
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
