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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTWidget.h>

#include "CAP3SupportDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "StructuralAlignmentDialogFiller"
#define GT_METHOD_NAME "run"

void CAP3SupportDialogFiller::commonScenario(){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    foreach (QString file, input) {
        int num = file.lastIndexOf('/');
        if (num == -1){
            num = file.lastIndexOf('\\');
            GT_CHECK(num != -1, QString("String %1 not looks like file path").arg(file))
        }
        QString path = file.left(num);
        QString name = file.right(file.length() - num - 1);

        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, path, name));
        GTWidget::click(os, GTWidget::findWidget(os, "addButton", dialog));
    }

    int num = output.lastIndexOf('/');
    if (num == -1){
        num = output.lastIndexOf('\\');
        GT_CHECK(num != -1, QString("String %1 not looks like file path").arg(output))
    }
    QString outputPath = output.left(num);
    QString outputName = output.right(output.length() - num - 1);
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, outputPath, outputName, GTFileDialogUtils::Save));
    GTWidget::click(os, GTWidget::findWidget(os, "specifyOutputPathButton", dialog));

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME


}

