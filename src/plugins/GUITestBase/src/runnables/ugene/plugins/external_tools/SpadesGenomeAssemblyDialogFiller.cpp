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

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDir>

#include "SpadesGenomeAssemblyDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "SpadesGenomeAssemblyDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void SpadesGenomeAssemblyDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(output, GTGlobals::UseMouse, GTFileDialogUtils::Choose));
    GTWidget::click(GTWidget::findWidget("setResultDirNameButton", dialog));

    auto libraryComboBox = GTWidget::findComboBox("libraryComboBox", dialog);
    GTComboBox::selectItemByText(libraryComboBox, library);

    foreach (QString s, leftReads) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(s));
        GTWidget::click(GTWidget::findWidget("addLeftButton", dialog));
    }

    foreach (QString s, rightReads) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(s));
        GTWidget::click(GTWidget::findWidget("addFightButton", dialog));
    }

    QComboBox* combo;
    if (!datasetType.isEmpty()) {
        combo = GTWidget::findComboBox("typeCombo", dialog);
        GTComboBox::selectItemByText(combo, datasetType);
    }

    if (!runningMode.isEmpty()) {
        combo = GTWidget::findComboBox("modeCombo", dialog);
        GTComboBox::selectItemByText(combo, runningMode);
    }

    if (!kmerSizes.isEmpty()) {
        GTLineEdit::setText("kmerEdit", kmerSizes, dialog);
    }

    if (numThreads != 0) {
        GTSpinBox::setValue("numThreadsSpinbox", numThreads, GTGlobals::UseKeyBoard);
    }

    if (memLimit != 0) {
        GTSpinBox::setValue("memlimitSpin", memLimit, GTGlobals::UseKeyBoard);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
