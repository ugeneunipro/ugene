/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "StartupDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>

#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::StartupDialogFiller"

StartupDialogFiller::StartupDialogFiller(const QString& path, bool isPathValid)
    : Filler("StartupDialog"),
      path(path),
      isPathValid(isPathValid) {
}

void StartupDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    if (path != UGUITest::sandBoxDir) {
        QString originalPath = GTLineEdit::getText("pathEdit", dialog);
        GTLineEdit::setText("pathEdit", path, dialog);

        if (!isPathValid) {
            GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);

            GTLineEdit::setText("pathEdit", originalPath, dialog);
        }
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME

}  // namespace U2
