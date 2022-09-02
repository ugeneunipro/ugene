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

#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>

#include <U2Core/global.h>

#include "Primer3DialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"
Primer3DialogFiller::Primer3DialogFiller(HI::GUITestOpStatus& os, const Primer3Settings& settings)
    : Filler(os, "Primer3Dialog"),
      settings(settings) {
}

#define GT_METHOD_NAME "run"
void Primer3DialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (settings.resultsCount != -1) {
        auto resultsCountSpinBox = GTWidget::findSpinBox(os, "edit_PRIMER_NUM_RETURN", dialog);
        GTSpinBox::setValue(os, resultsCountSpinBox, settings.resultsCount);
    }

    auto tabWidget = GTWidget::findTabWidget(os, "tabWidget", dialog);
    if (!settings.primersGroupName.isEmpty() || !settings.primersName.isEmpty()) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 6);

        if (!settings.primersGroupName.isEmpty()) {
            GTLineEdit::setText(os, "leGroupName", settings.primersGroupName, dialog);
        }

        if (!settings.primersName.isEmpty()) {
            GTLineEdit::setText(os, "leAnnotationName", settings.primersName, dialog);
        }
    }

    if (settings.start != -1) {
        GTLineEdit::setText(os, "start_edit_line", QString::number(settings.start), dialog);
    }

    if (settings.end != -1) {
        GTLineEdit::setText(os, "end_edit_line", QString::number(settings.end), dialog);
    }
    auto leftCheckbox = GTWidget::findCheckBox(os, "checkbox_PICK_LEFT", dialog);
    GTCheckBox::setChecked(os, leftCheckbox, settings.pickLeft);

    auto rightCheckbox = GTWidget::findCheckBox(os, "checkbox_PICK_RIGHT", dialog);
    GTCheckBox::setChecked(os, rightCheckbox, settings.pickRight);

    if (settings.rtPcrDesign) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 4);

        auto groupBox = GTWidget::findGroupBox(os, "spanIntronExonBox", dialog);
        GTGroupBox::setChecked(os, groupBox);
    }

    if (!settings.shortRegion) {
        auto button = GTWidget::findPushButton(os, "pickPrimersButton", dialog);
        GTWidget::click(os, button);
    } else {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        auto button = GTWidget::findPushButton(os, "pickPrimersButton", dialog);
        GTWidget::click(os, button);
        if (isOsMac()) {
            dialog->close();
        } else {
            GTKeyboardDriver::keyClick(Qt::Key_Escape);
        }
    }
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
