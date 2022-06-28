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

#include "ExportPrimersDialogFiller.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>

#include "GTUtilsTaskTreeView.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportPrimersDialogFiller"

ExportPrimersDialogFiller::ExportPrimersDialogFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "ExportPrimersDialog", scenario) {
}

#define GT_METHOD_NAME "commonScenario"
void ExportPrimersDialogFiller::commonScenario() {
    GTUtilsDialog::clickButtonBox(os, getDialog(os), QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDialog"
QWidget* ExportPrimersDialogFiller::getDialog(HI::GUITestOpStatus& os) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    return dialog;
}
#undef GT_METHOD_NAME


#define GT_METHOD_NAME "setFormat"
void ExportPrimersDialogFiller::setFormat(HI::GUITestOpStatus& os, const QString& format) {
    GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "cbFormat", getDialog(os)), format);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFilePath"
void ExportPrimersDialogFiller::setFilePath(HI::GUITestOpStatus& os, const QString& filePath) {
    GTLineEdit::setText(os, "leFilePath", filePath, getDialog(os));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
