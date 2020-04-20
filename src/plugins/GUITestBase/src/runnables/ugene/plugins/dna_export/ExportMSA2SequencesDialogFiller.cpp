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

#include "ExportMSA2SequencesDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTCheckBox.h>

#include <QDir>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportToSequenceFormatFiller"

ExportToSequenceFormatFiller::ExportToSequenceFormatFiller(HI::GUITestOpStatus &_os, const QString &_path, const QString &_name, documentFormat _format,
                                                           bool saveFile, bool keepCharacters, GTGlobals::UseMethod method):
Filler(_os, "U2__ExportMSA2SequencesDialog"), name(_name), format(_format), saveFile(saveFile), keepCharacters(keepCharacters),
useMethod(method) {
    QString __path = QDir::cleanPath(QDir::currentPath() + "/" + _path);
    if (__path.at(__path.count() - 1) != '/') {
        __path += '/';
    }
    path = __path;
    comboBoxItems[EMBL] = "EMBL";
    comboBoxItems[FASTA] = "FASTA";
    comboBoxItems[FASTQ] = "FASTQ";
    comboBoxItems[GFF] = "GFF";
    comboBoxItems[Genbank] = "GenBank";
    comboBoxItems[Swiss_Prot] = "Swiss_Prot";

}

ExportToSequenceFormatFiller::ExportToSequenceFormatFiller(HI::GUITestOpStatus &_os, CustomScenario *scenario)
    : Filler(_os, "U2__ExportMSA2SequencesDialog", scenario),
      format(EMBL),
      saveFile(false),
      keepCharacters(false)
{

}

#define GT_METHOD_NAME "commonScenario"
void ExportToSequenceFormatFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(lineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, lineEdit, path + name);

    QComboBox *comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != NULL, "ComboBox not found");

    int index = comboBox->findText(comboBoxItems[format]);
    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    if (comboBox->currentIndex() != index){
        GTComboBox::setCurrentIndex(os, comboBox, index, true, useMethod);
        }

    if (saveFile){
        QCheckBox* saveFileCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "addToProjectBox", dialog));
        GTCheckBox::setChecked(os, saveFileCheckBox);
        }

    if (keepCharacters){
        QRadioButton* keepButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "keepGapsRB", dialog));
        keepButton->setChecked(true);
        }
    else {
        QRadioButton* trimButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "trimGapsRB", dialog));
        trimButton->setChecked(true);
        }

    GTGlobals::sleep(100);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME


}
