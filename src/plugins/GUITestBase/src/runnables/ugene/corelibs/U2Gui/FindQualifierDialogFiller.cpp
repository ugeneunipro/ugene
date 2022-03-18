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

#include "FindQualifierDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::FindQualifierFiller"

FindQualifierFiller::FindQualifierFiller(HI::GUITestOpStatus& os, const FindQualifierFiller::FindQualifierFillerSettings& settings)
    : Filler(os, "SearchQualifierDialog"),
      settings(settings) {
}

FindQualifierFiller::FindQualifierFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "SearchQualifierDialog", scenario) {
}

#define GT_METHOD_NAME "commonScenario"
void FindQualifierFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QLineEdit* nameEdit = dialog->findChild<QLineEdit*>("nameEdit");
    GT_CHECK(nameEdit != nullptr, "nameEdit not found");
    GTLineEdit::setText(os, nameEdit, settings.name);

    QLineEdit* valueEdit = dialog->findChild<QLineEdit*>("valueEdit");
    GT_CHECK(valueEdit != nullptr, "valueEdit not found");
    GTLineEdit::setText(os, valueEdit, settings.value);

    if (settings.exactMatch) {
        QRadioButton* exactButton = dialog->findChild<QRadioButton*>("exactButton");
        GT_CHECK(exactButton != nullptr, "exactButton not found");
        GTRadioButton::click(os, exactButton);
    } else {
        QRadioButton* containsButton = dialog->findChild<QRadioButton*>("containsButton");
        GT_CHECK(containsButton != nullptr, "exactButton not found");
        GTRadioButton::click(os, containsButton);
    }

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != nullptr, "buttonBox is NULL");

    if (settings.expectTheEndOfTree) {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "The end of the annotation tree has been reached. Would you like to start the search from the beginning?"));
    }

    if (settings.expectNoResults) {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok, "No results found"));
    }

    if (settings.nextQualifier && settings.nextCount > 0) {
        QPushButton* nextButton = box->button(QDialogButtonBox::Ok);
        GT_CHECK(nextButton != nullptr, "\"Next\" button is NULL");
        for (unsigned int i = 0; i < settings.nextCount; i++) {
            GTWidget::click(os, nextButton);
        }
    }

    if (settings.selectAll) {
        QPushButton* selectAllButton = box->button(QDialogButtonBox::Yes);
        GT_CHECK(selectAllButton != nullptr, "\"Select all\" button is NULL");
        GTWidget::click(os, selectAllButton);
    }
    GTGlobals::sleep();

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Close);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
