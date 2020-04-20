/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>

#include <primitives/GTCheckBox.h>
#include "system/GTClipboard.h"
#include <primitives/GTComboBox.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTPlainTextEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include "ImportAnnotationsToCsvFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ImportAnnotationsToCsvFiller"

ImportAnnotationsToCsvFiller::ImportAnnotationsToCsvFiller(HI::GUITestOpStatus &_os,
                                                           const QString &_fileToRead,
                                                           const QString &_resultFile,
                                                           ImportAnnotationsToCsvFiller::fileFormat _format,
                                                           bool _addResultFileToProject,
                                                           bool _columnSeparator,
                                                           const QString &_separator,
                                                           int _numberOfLines,
                                                           const QString &_skipAllLinesStartsWith,
                                                           bool _interpretMultipleAsSingle,
                                                           bool _removeQuotesButton,
                                                           const QString &_defaultAnnotationName,
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
      useMethod(method)
{
    comboBoxItems[BED] = "BED";
    comboBoxItems[EMBL] = "EMBL";
    comboBoxItems[FPKM] = "FPKM";
    comboBoxItems[GFF] = "GFF";
    comboBoxItems[GTF] = "GTF";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxItems[Swiss_Prot] = "Swiss_Prot";
}

ImportAnnotationsToCsvFiller::ImportAnnotationsToCsvFiller(GUITestOpStatus &_os, CustomScenario *c)
    : Filler(_os, "ImportAnnotationsFromCSVDialog", c),
      format(BED),
      addResultFileToProject(false),
      columnSeparator(false),
      numberOfLines(0),
      interpretMultipleAsSingle(false),
      removeQuotesButton(false),
      useMethod(GTGlobals::UseKey)
{

}

#define GT_METHOD_NAME "run"
void RoleFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    QCheckBox *addOffsetCheckBox = dialog->findChild<QCheckBox*>("startOffsetCheck");
    QSpinBox *addOffsetSpinBox = dialog->findChild<QSpinBox*>("startOffsetValue");
    QCheckBox *endPosCheckBox = dialog->findChild<QCheckBox*>("endInclusiveCheck");
    QCheckBox *strandMarkCheckBox = dialog->findChild<QCheckBox*>("complValueCheck");
    QLineEdit *markValueLineEdit = dialog->findChild<QLineEdit*>("complValueEdit");
    QLineEdit *qualifierLineEdit = dialog->findChild<QLineEdit*>("qualifierNameEdit");

    GTGlobals::sleep(300);
    ImportAnnotationsToCsvFiller::StartParameter* startP = dynamic_cast<ImportAnnotationsToCsvFiller::StartParameter*>(parameter);
    if (startP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("startRB"));
        GTCheckBox::setChecked(os, addOffsetCheckBox, startP->addOffset);
        if (startP->addOffset) {
            GTSpinBox::setValue(os, addOffsetSpinBox, startP->numberOfBp);
        }
    }

    ImportAnnotationsToCsvFiller::EndParameter* endP = dynamic_cast<ImportAnnotationsToCsvFiller::EndParameter*>(parameter);
    if (endP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("endRB"));
        GTCheckBox::setChecked(os, endPosCheckBox, endP->endPos);
    }

    ImportAnnotationsToCsvFiller::LengthParameter* lenghtP = dynamic_cast<ImportAnnotationsToCsvFiller::LengthParameter*>(parameter);
    if (lenghtP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("lengthRB"));
    }

    ImportAnnotationsToCsvFiller::StrandMarkParameter* strandMarkP = dynamic_cast<ImportAnnotationsToCsvFiller::StrandMarkParameter*>(parameter);
    if (strandMarkP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("complMarkRB"));
        GTCheckBox::setChecked(os, strandMarkCheckBox, strandMarkP->markValue);
        GTLineEdit::setText(os, markValueLineEdit, strandMarkP->markValueName);
    }

    ImportAnnotationsToCsvFiller::NameParameter* nameP = dynamic_cast<ImportAnnotationsToCsvFiller::NameParameter*>(parameter);
    if (nameP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("nameRB"));
    }

    ImportAnnotationsToCsvFiller::QualifierParameter* qualP = dynamic_cast<ImportAnnotationsToCsvFiller::QualifierParameter*>(parameter);
    if (qualP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("qualifierRB"));
        GTLineEdit::setText(os, qualifierLineEdit, qualP->name);
    }

    ImportAnnotationsToCsvFiller::IgnoreParameter* ignoreP = dynamic_cast<ImportAnnotationsToCsvFiller::IgnoreParameter*>(parameter);
    if (ignoreP) {
        GTRadioButton::click(os, dialog->findChild<QRadioButton*>("ignoreRB"));
    }

    delete parameter;
    parameter = NULL;

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

class GTTableWidget {
public:
    static QPoint headerItemCenter(HI::GUITestOpStatus&os, QTableWidget* w, int pos) {
        CHECK_SET_ERR_RESULT(w != NULL, "QTableWidget is NULL", QPoint());

        QTableWidgetItem *widgetItem =  w->item(0, pos);
        CHECK_SET_ERR_RESULT(widgetItem != NULL, "QTableWidgetItem is NULL", QPoint());

        QPoint itemPoint = w->visualItemRect(widgetItem).center();
        return w->mapToGlobal(itemPoint);
    }
};

#define GT_METHOD_NAME "run"
void ImportAnnotationsToCsvFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *readFileLineEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("readFileName"));
    GT_CHECK(readFileLineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, readFileLineEdit, fileToRead);

    QLineEdit *resultFileLineEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("saveFileName"));
    GT_CHECK(resultFileLineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, dialog->findChild<QLineEdit*>(QString::fromUtf8("saveFileName")), resultFile);

    QComboBox *comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != NULL, "ComboBox not found");
    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));
    GTComboBox::setCurrentIndex(os, comboBox, index, useMethod);

    QSpinBox *spinBox = dialog->findChild<QSpinBox *>();
    GT_CHECK(spinBox != NULL, "SpinBox not found");
    GTSpinBox::setValue(os, spinBox, numberOfLines, useMethod);

    QCheckBox *checkBox = dialog->findChild<QCheckBox*>(QString::fromUtf8("addToProjectCheck"));
    GTCheckBox::setChecked(os, checkBox, addResultFileToProject);

    if (columnSeparator) {
        QRadioButton* columnSeparator = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "columnSeparatorRadioButton", dialog));
        GTRadioButton::click(os, columnSeparator);

        QLineEdit *separatorEdit = GTWidget::findExactWidget<QLineEdit *>(os, "separatorEdit", dialog);
        GTLineEdit::setText(os, separatorEdit, separator, false, true);

//        GTClipboard::setText(os, separator);
//        GTWidget::click(os, separatorEdit);
//        GTKeyboardDriver::keyClick(os, 'v', GTKeyboardDriver::key["ctrl"]);
    } else {
        QRadioButton* scriptRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "scriptRadioButton", dialog));
        scriptRadioButton->setChecked(true);
    }

    QLineEdit *firstLinesLineEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("prefixToSkipEdit"));
    GT_CHECK(firstLinesLineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, firstLinesLineEdit, skipAllLinesStartsWith);

    QCheckBox *separatorsModeCheckBox = dialog->findChild<QCheckBox*>(QString::fromUtf8("separatorsModeCheck"));
    GTCheckBox::setChecked(os, separatorsModeCheckBox, interpretMultipleAsSingle);

    QCheckBox *removeQuotesCheckBox = dialog->findChild<QCheckBox*>(QString::fromUtf8("removeQuotesCheck"));
    GTCheckBox::setChecked(os, removeQuotesCheckBox, removeQuotesButton);

    QLineEdit *defaultAnnotationNameLineEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("defaultNameEdit"));
    GT_CHECK(defaultAnnotationNameLineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, defaultAnnotationNameLineEdit, defaultAnnotationName);

    QPushButton *previewButton = dialog->findChild<QPushButton*>("previewButton");
    GT_CHECK(previewButton != NULL, "DA button not found");
    GTWidget::click(os, previewButton);
    GTGlobals::sleep();

    QTableWidget *previewTable = dialog->findChild<QTableWidget*>("previewTable");
    foreach (RoleColumnParameter r, roleParameters) {
        GTUtilsDialog::waitForDialog(os, new RoleFiller(os, r.parameter));
        GT_CHECK(previewTable->item(0, r.column) != NULL, "Table item not found");
        previewTable->scrollToItem(previewTable->item(0, r.column));
        GTGlobals::sleep(200);
        GTMouseDriver::moveTo(GTTableWidget::headerItemCenter(os, previewTable, r.column));
        GTMouseDriver::click();
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
