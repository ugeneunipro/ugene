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

BlastLocalSearchDialogFiller::BlastLocalSearchDialogFiller(const Parameters& parameters, HI::GUITestOpStatus& os)
    : Filler(os, "BlastLocalSearchDialog"), parameters(parameters) {
}

BlastLocalSearchDialogFiller::BlastLocalSearchDialogFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "BlastLocalSearchDialog", scenario) {
}

#define GT_METHOD_NAME "commonScenario"
void BlastLocalSearchDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget(os);
    if (!parameters.runBlast) {
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        return;
    }

    auto programNameCombo = GTWidget::findComboBox(os, "programNameComboBox", dialog);
    GTComboBox::selectItemByText(os, programNameCombo, parameters.programNameText);

    if (!parameters.dbPath.isEmpty()) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.dbPath));
        GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton"));
    }

    if (parameters.withInputFile) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters.inputPath));
        GTWidget::click(os, GTWidget::findWidget(os, "browseInput"));
    }

    if (!parameters.searchRegion.isEmpty()) {
        GTLineEdit::setText(os, "start_edit_line", QString::number(parameters.searchRegion.startPos), dialog);
        GTLineEdit::setText(os, "end_edit_line", QString::number(parameters.searchRegion.endPos()), dialog);
    }
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
