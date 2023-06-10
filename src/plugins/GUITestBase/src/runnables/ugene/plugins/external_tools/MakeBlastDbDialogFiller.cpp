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

#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QGroupBox>

#include "GTUtilsTaskTreeView.h"
#include "MakeBlastDbDialogFiller.h"

namespace U2 {

MakeBlastDbDialogFiller::MakeBlastDbDialogFiller(const Parameters& parameters)
    : Filler("MakeBlastDbDialog"), parameters(parameters) {
}

#define GT_CLASS_NAME "GTUtilsDialog::MakeBlastDbDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void MakeBlastDbDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto inputFilesRadioButton = GTWidget::findRadioButton("inputFilesRadioButton", dialog);
    GTWidget::findLineEdit("inputFilesLineEdit", dialog);

    auto proteinTypeRadioButton = GTWidget::findRadioButton("proteinTypeRadioButton", dialog);
    auto nucleotideTypeRadioButton = GTWidget::findRadioButton("nucleotideTypeRadioButton", dialog);

    bool isProtein = parameters.alphabetType == Parameters::Protein;
    CHECK_SET_ERR(!parameters.checkAlphabetType || isProtein == proteinTypeRadioButton->isChecked(), "Incorrect alphabet");

    if (parameters.justCancel) {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (!parameters.inputFilePath.isEmpty()) {
        GTRadioButton::click(inputFilesRadioButton);
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters.inputFilePath));
        GTWidget::click(GTWidget::findWidget("inputFilesToolButton"));
    }

    GTRadioButton::click(isProtein ? proteinTypeRadioButton : nucleotideTypeRadioButton);

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters.outputDirPath, "", GTFileDialogUtils::Choose));
    GTWidget::click(GTWidget::findWidget("databasePathToolButton"));
    GTUtilsTaskTreeView::waitTaskFinished();

    GTWidget::click(GTWidget::findButtonByText("Build", GTUtilsDialog::buttonBox(dialog)));
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
