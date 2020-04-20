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

#include <QToolButton>
#include <QApplication>
#include <QGroupBox>
#include <QComboBox>

#include <primitives/GTWidget.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTRadioButton.h>

#include "FormatDBDialogFiller.h"

namespace U2 {

FormatDBSupportRunDialogFiller::FormatDBSupportRunDialogFiller(HI::GUITestOpStatus &os, const Parameters &parameters)
: Filler(os, "FormatDBSupportRunDialog"), parameters(parameters)
{

}

#define GT_CLASS_NAME "GTUtilsDialog::FormatDBSupportRunDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void FormatDBSupportRunDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QRadioButton *inputFilesRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "inputFilesRadioButton", dialog));
    GT_CHECK(inputFilesRadioButton, "inputFilesRadioButton not found");
    QLineEdit *inputFilesLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "inputFilesLineEdit", dialog));
    GT_CHECK(inputFilesLineEdit, "inputFilesLineEdit is NULL");
    QToolButton *inputFilesToolButton = qobject_cast<QToolButton*>(GTWidget::findWidget(os, "inputFilesToolButton", dialog));
    GT_CHECK(inputFilesToolButton, "inputFilesToolButton is NULL");

    QRadioButton *inputDirRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "inputDirRadioButton", dialog));
    GT_CHECK(inputDirRadioButton, "inputDirRadioButton not found");
    QLineEdit *inputDirLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "inputDirLineEdit", dialog));
    GT_CHECK(inputDirLineEdit, "inputDirLineEdit is NULL");
    QToolButton *inputDirToolButton = qobject_cast<QToolButton*>(GTWidget::findWidget(os, "inputDirToolButton", dialog));
    GT_CHECK(inputDirLineEdit, "inputDirToolButton is NULL");

    QRadioButton *proteinTypeRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "proteinTypeRadioButton", dialog));
    GT_CHECK(proteinTypeRadioButton, "proteinTypeRadioButton not found");
    QRadioButton *nucleotideTypeRadioButton = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "nucleotideTypeRadioButton", dialog));
    GT_CHECK(nucleotideTypeRadioButton, "nucleotideTypeRadioButton not found");

    if (parameters.customFiller_3551) {
        GTRadioButton::click(os, inputFilesRadioButton);
        CHECK_SET_ERR(inputFilesRadioButton->isChecked(), "Files radio button is unchecked");
        CHECK_SET_ERR(inputFilesLineEdit->isEnabled(), "Files lineedit is disabled");
        CHECK_SET_ERR(inputFilesToolButton->isEnabled(), "Files button is disabled");

        GTGlobals::sleep(500);

        GTRadioButton::click(os, inputDirRadioButton);
        CHECK_SET_ERR(inputDirRadioButton->isChecked(), "Dir radio button is unchecked");
        CHECK_SET_ERR(inputDirLineEdit->isEnabled(), "Dir lineedit is disabled");
        CHECK_SET_ERR(inputDirToolButton->isEnabled(), "Dir button is disabled");
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    if(parameters.checkAlphabetType) {
        bool isProtein = Parameters::Protein == parameters.alphabetType;
        CHECK_SET_ERR(isProtein == proteinTypeRadioButton->isChecked(), "Incorrect alphabet is set");
    }

    if (parameters.justCancel) {
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (!parameters.inputFilePath.isEmpty()) {
        GTRadioButton::click(os, inputFilesRadioButton);
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.inputFilePath));
        GTWidget::click(os, GTWidget::findWidget(os, "inputFilesToolButton"));
    }

    if (Parameters::Protein == parameters.alphabetType) {
        GTRadioButton::click(os, proteinTypeRadioButton);
    } else {
        GTRadioButton::click(os, nucleotideTypeRadioButton);
    }

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.outputDirPath, "", GTFileDialogUtils::Choose));
    GTWidget::click(os, GTWidget::findWidget(os, "databasePathToolButton"));
    GTGlobals::sleep(3000);

    GTWidget::click(os, GTWidget::findButtonByText(os, "Format", GTUtilsDialog::buttonBox(os, dialog)));
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} // U2
