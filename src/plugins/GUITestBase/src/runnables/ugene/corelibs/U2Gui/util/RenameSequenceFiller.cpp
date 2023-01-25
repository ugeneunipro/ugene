/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "RenameSequenceFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QPushButton>

#include "system/GTClipboard.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::RenamesequenceFiller"
#define GT_METHOD_NAME "run"

void RenameSequenceFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GT_CHECK(dialog->windowTitle() == "Rename Read" || dialog->windowTitle() == "Rename Sequence", "dialog not found");

    QLineEdit* nameEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(nameEdit != nullptr, "Line edit not found");

    if (!oldName.isEmpty()) {
        QString actualText = nameEdit->text();
        GT_CHECK(oldName == actualText, "edited sequence name not match with expected");
    }

    if (oldName != newName) {
        // if filler used not for checking sequence name
        GTLineEdit::setText(os, nameEdit, newName, false, useCopyPaste);
    }

    GTWidget::click(os, GTWidget::findButtonByText(os, "OK", dialog));
    GTGlobals::sleep(500);
    if (nullptr != QApplication::activeModalWidget()) {
        GTWidget::click(os, GTWidget::findButtonByText(os, "OK", QApplication::activeModalWidget()));
    }
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
