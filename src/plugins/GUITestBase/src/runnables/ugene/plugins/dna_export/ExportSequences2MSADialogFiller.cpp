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

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include "ExportSequences2MSADialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportSequenceAsAlignmentFiller"
ExportSequenceAsAlignmentFiller::ExportSequenceAsAlignmentFiller(HI::GUITestOpStatus &_os, const QString &_path, const QString &_name,
                                ExportSequenceAsAlignmentFiller::FormatToUse _format, bool addDocumentToProject, GTGlobals::UseMethod method):
Filler(_os, "U2__ExportSequences2MSADialog"), name(_name), useMethod(method), format(_format), addToProject(addDocumentToProject) {
    QString __path = QDir::cleanPath(QDir::currentPath() + "/" + _path);
    if (__path.at(__path.count() - 1) != QDir::separator()) {
        __path += QDir::separator();
    }

    path = QDir::toNativeSeparators(__path);

    comboBoxItems[Clustalw] = "CLUSTALW";
    comboBoxItems[Fasta] = "FASTA";
    comboBoxItems[Msf] = "MSF";
    comboBoxItems[Mega] = "Mega";
    comboBoxItems[Nexus] = "NEXUS";
    comboBoxItems[Sam] = "SAM";
    comboBoxItems[Stockholm] = "Stockholm";
}

ExportSequenceAsAlignmentFiller::ExportSequenceAsAlignmentFiller(HI::GUITestOpStatus &_os, CustomScenario *scenario)
    : Filler(_os, "U2__ExportSequences2MSADialog", scenario),
      format(Clustalw),
      addToProject(false)
{

}

#define GT_METHOD_NAME "run"
void ExportSequenceAsAlignmentFiller::commonScenario()
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
    GTComboBox::setCurrentIndex(os, comboBox, index, useMethod);

    QCheckBox *checkBox = dialog->findChild<QCheckBox*>(QString::fromUtf8("addToProjectBox"));
    GTCheckBox::setChecked(os, checkBox, addToProject);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
