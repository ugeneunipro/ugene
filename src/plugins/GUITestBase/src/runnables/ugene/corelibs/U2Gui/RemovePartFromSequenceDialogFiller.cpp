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
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include "RemovePartFromSequenceDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::RemovePartFromSequenceDialogFiller"
RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus &_os, QString _range, bool recalculateQuals)
    : Filler(_os, "RemovePartFromSequenceDialog"), range(_range), removeType(Resize), format(FASTA), saveNew(false),
    recalculateQuals(recalculateQuals)
{

}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus &_os,RemoveType _removeType, bool _saveNew,
    const QString &_saveToFile, FormatToUse _format)
    : Filler(_os, "RemovePartFromSequenceDialog"), removeType(_removeType), format(_format), saveNew(_saveNew), recalculateQuals(false)
{
    QString __saveToFile = QDir::cleanPath(QDir::currentPath() + "/" + _saveToFile);
    saveToFile = QDir::toNativeSeparators(__saveToFile);
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[Genbank] = "GenBank";
}

RemovePartFromSequenceDialogFiller::RemovePartFromSequenceDialogFiller(HI::GUITestOpStatus &_os, RemoveType _removeType)
    : Filler(_os, "RemovePartFromSequenceDialog"), removeType(_removeType), format(FASTA), recalculateQuals(false)
{

}

#define GT_METHOD_NAME "commonScenario"
void RemovePartFromSequenceDialogFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if (!range.isEmpty()){
        QLineEdit *removeLocationEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("removeLocationEdit"));
        GT_CHECK(removeLocationEdit != NULL, "QLineEdit \"removeLocationEdit\" not found");
        GTLineEdit::setText(os, removeLocationEdit, range);
    }
    if (removeType == Resize) {
        QRadioButton *resizeRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("resizeRB"));
        GT_CHECK(resizeRB != NULL, "radio button not found");
        GTRadioButton::click(os, resizeRB);
    } else {
        QRadioButton *removeRB = dialog->findChild<QRadioButton*>(QString::fromUtf8("removeRB"));
        GT_CHECK(removeRB != NULL, "radio button not found");
        GTRadioButton::click(os, removeRB);
    }

    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "recalculateQualsCheckBox"), recalculateQuals);

    GTGlobals::sleep(1000);
    if (saveNew) {
        QGroupBox *saveToAnotherBox =  dialog->findChild<QGroupBox*>(QString::fromUtf8("saveToAnotherBox"));
        GTGroupBox::setChecked(os, saveToAnotherBox, true);
        QLineEdit *filepathEdit = dialog->findChild<QLineEdit*>(QString::fromUtf8("filepathEdit"));
        GT_CHECK(filepathEdit != NULL, "QLineEdit \"filepathEdit\" not found");

        GTLineEdit::setText(os, filepathEdit, saveToFile);
    }
    if (format != FASTA){
        QComboBox *formatBox = dialog->findChild<QComboBox*>(QString::fromUtf8("formatBox"));
        GT_CHECK(formatBox != NULL, "format box not found");
        int index = formatBox->findText(comboBoxItems[format]);
        GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

        GTComboBox::setCurrentIndex(os, formatBox, index);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
