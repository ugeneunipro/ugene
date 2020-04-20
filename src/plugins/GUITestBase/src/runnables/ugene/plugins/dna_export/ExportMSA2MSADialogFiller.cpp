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

#include "ExportMSA2MSADialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTComboBox.h>

#include <QDir>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportToSequenceFormatFiller"

ExportMSA2MSADialogFiller::ExportMSA2MSADialogFiller(HI::GUITestOpStatus &_os, int _formatVal, QString _path) : Filler(_os, "U2__ExportMSA2MSADialog"),
    formatVal(_formatVal),
    path(_path){}

#define GT_METHOD_NAME "commonScenario"
void ExportMSA2MSADialogFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if(!path.isEmpty()){
        QLineEdit* fileNameEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
        GTLineEdit::setText(os, fileNameEdit,path);
    }
    if(formatVal>=0){
        QComboBox* formatCombo = dialog->findChild<QComboBox*>("formatCombo");
        GTComboBox::setCurrentIndex(os, formatCombo, formatVal);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
