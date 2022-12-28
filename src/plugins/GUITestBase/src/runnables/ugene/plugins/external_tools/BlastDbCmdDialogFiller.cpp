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

#include "BlastDbCmdDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include "base_dialogs/GTFileDialog.h"

namespace U2 {

BlastDbCmdDialogFiller::BlastDbCmdDialogFiller(HI::GUITestOpStatus& os,
                                               const QString& dbPath,
                                               const QString& outputPath)
    : Filler(os, "BlastDBCmdDialog"), dbPath(dbPath), outputPath(outputPath) {
}

BlastDbCmdDialogFiller::BlastDbCmdDialogFiller(HI::GUITestOpStatus& os, HI::CustomScenario* customScenario)
    : Filler(os, "BlastDBCmdDialog", customScenario) {
}

void BlastDbCmdDialogFiller::commonScenario() {
    using namespace HI;
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dbPath));
    GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton", dialog));

    GTLineEdit::setText(os, "outputPathLineEdit", outputPath, dialog);
    GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
}

}  // namespace U2
