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

#include "ExportReadsDialogFiller.h"

#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include <primitives/GTLineEdit.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTWidget.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportReadsDialogFiller"

ExportReadsDialogFiller::ExportReadsDialogFiller(HI::GUITestOpStatus &os, const QString &filePath, const QString format, bool addToProject)
    : Filler(os, "ExportReadsDialog"),
      filePath(filePath),
      format(format),
      addToProject(addToProject)
{
}

#define GT_METHOD_NAME "commonScenario"
void ExportReadsDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit* fileLineEdit = dialog->findChild<QLineEdit*>("filepathLineEdit");
    GT_CHECK(fileLineEdit != NULL, "File path lineEdit not found");
    GTLineEdit::setText(os, fileLineEdit, filePath);

    QComboBox *formatComboBox = dialog->findChild<QComboBox*>("documentFormatComboBox");
    GT_CHECK(formatComboBox != NULL, "Format comboBox not found");
    GTComboBox::setIndexWithText(os, formatComboBox, format);

    QCheckBox* addToPrj = dialog->findChild<QCheckBox*>("addToProjectCheckBox");
    GT_CHECK(addToPrj != NULL, "Add to project check box not found");
    GTCheckBox::setChecked(os, addToPrj, addToProject);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

} // namespace
