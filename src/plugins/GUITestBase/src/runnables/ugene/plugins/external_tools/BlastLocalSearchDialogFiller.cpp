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
#include <harness/UGUITestBase.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "BlastLocalSearchDialogFiller.h"
#include "GTUtilsTaskTreeView.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::BlastLocalSearchDialogFiller"

BlastLocalSearchDialogFiller::BlastLocalSearchDialogFiller(const Parameters& parameters)
    : Filler("BlastLocalSearchDialog"), parameters(parameters) {
}

BlastLocalSearchDialogFiller::BlastLocalSearchDialogFiller(CustomScenario* scenario)
    : Filler("BlastLocalSearchDialog", scenario) {
}

#define GT_METHOD_NAME "commonScenario"
void BlastLocalSearchDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget();
    if (!parameters.runBlast) {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        return;
    }

    auto programNameCombo = GTWidget::findComboBox("programNameComboBox", dialog);
    GTComboBox::selectItemByText(programNameCombo, parameters.programNameText);

    if (!parameters.dbPath.isEmpty()) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters.dbPath));
        GTWidget::click(GTWidget::findWidget("selectDatabasePushButton"));
    }

    if (parameters.withInputFile) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters.inputPath));
        GTWidget::click(GTWidget::findWidget("browseInput"));
    }

    if (!parameters.searchRegion.isEmpty()) {
        GTLineEdit::setText("start_edit_line", QString::number(parameters.searchRegion.startPos), dialog);
        GTLineEdit::setText("end_edit_line", QString::number(parameters.searchRegion.endPos()), dialog);
    }
    GTUtilsTaskTreeView::waitTaskFinished();
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
