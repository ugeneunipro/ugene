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

#include "PwmBuildDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QLineEdit>

#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"

PwmBuildDialogFiller::PwmBuildDialogFiller(const QList<Action>& actions)
    : Filler("PWMBuildDialog"),
      dialog(nullptr),
      actions(actions) {
}

PwmBuildDialogFiller::PwmBuildDialogFiller(CustomScenario* c)
    : Filler("PWMBuildDialog", c),
      dialog(nullptr) {
}

void PwmBuildDialogFiller::commonScenario() {
    dialog = GTWidget::getActiveModalWidget();

    foreach (const Action& action, actions) {
        switch (action.first) {
            case EnterInput:
                enterInput(action.second);
                break;
            case SelectInput:
                selectInput(action.second);
                break;
            case ExpectInvalidFile:
                expectInvalidFile();
                break;
            case ClickCancel:
                clickCancel();
                break;
            default:
                FAIL("Not implemented action", );
        }
    }
}

void PwmBuildDialogFiller::enterInput(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the file path from the action data");
    GTLineEdit::setText("inputEdit", actionData.toString(), dialog);
}

void PwmBuildDialogFiller::selectInput(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the file path from the action data");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(actionData.toString()));
    GTWidget::click(GTWidget::findWidget("inputButton", dialog));
    GTGlobals::sleep(500);
}

void PwmBuildDialogFiller::expectInvalidFile() {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok, "There are no sequences in the file."));
}

void PwmBuildDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

#undef GT_CLASS_NAME

}  // namespace U2
