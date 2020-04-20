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
#include <QLineEdit>
#include <QPushButton>

#include "StartupDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::StartupDialogFiller"

StartupDialogFiller::StartupDialogFiller(HI::GUITestOpStatus &os, const QString &path, bool isPathValid) :
    Filler(os, "StartupDialog"),
    path(path),
    isPathValid(isPathValid)
{

}

#define GT_METHOD_NAME "commonScenario"
void StartupDialogFiller::commonScenario() {
    GTGlobals::sleep(1000);
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (path != UGUITest::sandBoxDir) {
        QLineEdit *pathEdit = GTWidget::findExactWidget<QLineEdit *>(os, "pathEdit", dialog);
        CHECK(NULL != pathEdit, );

        const QString rightPath = GTLineEdit::getText(os, pathEdit);
        GTLineEdit::setText(os, pathEdit, path);

        if (!isPathValid) {
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            CHECK_OP(os, );

            GTLineEdit::setText(os, pathEdit, rightPath);
        }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}    // namespace U2
