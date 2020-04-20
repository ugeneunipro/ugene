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

#include "ExportBlastResultDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportAnnotationsFiller"

ExportBlastResultDialogFiller::ExportBlastResultDialogFiller(HI::GUITestOpStatus &os, const QString &filePath, bool _addRefBoxChecked) :
    Filler(os, "ExportBlastResultDialog"),
    filePath(filePath),
    addRefBoxChecked(_addRefBoxChecked)
{

}

#define GT_METHOD_NAME "commonScenario"
void ExportBlastResultDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog, "Active modal widget is NULL");

    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "fileNameEdit", dialog), filePath);

    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox*>(os, "addRefBox", dialog), addRefBoxChecked);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
