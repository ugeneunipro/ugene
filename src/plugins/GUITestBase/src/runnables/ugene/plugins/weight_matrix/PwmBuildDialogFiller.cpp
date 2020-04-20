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

#include "PwmBuildDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"

PwmBuildDialogFiller::PwmBuildDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions)
    : Filler(os, "PWMBuildDialog"),
      dialog(NULL),
      actions(actions)
{

}

PwmBuildDialogFiller::PwmBuildDialogFiller(HI::GUITestOpStatus &os, CustomScenario *c)
    : Filler(os, "PWMBuildDialog", c),
      dialog(NULL)
{

}

#define GT_METHOD_NAME "run"
void PwmBuildDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    foreach (const Action &action, actions) {
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
            os.setError("An unimplemented action");
            FAIL(false, );
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterInput"
void PwmBuildDialogFiller::enterInput(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the file path from the action data");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "inputEdit", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectInput"
void PwmBuildDialogFiller::selectInput(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the file path from the action data");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, actionData.toString()));
    GTWidget::click(os, GTWidget::findWidget(os, "inputButton", dialog));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expectInvalidFile"
void PwmBuildDialogFiller::expectInvalidFile() {
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "There are no sequences in the file."));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCancel"
void PwmBuildDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
