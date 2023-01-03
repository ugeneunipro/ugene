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

#include "ImportBAMFileDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ImportBAMFileFiller"
#define GT_METHOD_NAME "run"
ImportBAMFileFiller::ImportBAMFileFiller(HI::GUITestOpStatus& os, const QString destinationUrl, const QString referenceFolderPath, const QString referenceFileName, bool importUnmappedReads, bool deselectAll, int timeoutMs)
    : Filler(os, "Import BAM File"),
      referenceFolderPath(referenceFolderPath),
      referenceFileName(referenceFileName),
      destinationUrl(destinationUrl),
      importUnmappedReads(importUnmappedReads),
      deselectAll(deselectAll) {
    settings.timeout = timeoutMs;
}

ImportBAMFileFiller::ImportBAMFileFiller(HI::GUITestOpStatus& os, CustomScenario* _c)
    : Filler(os, "Import BAM File", _c),
      referenceFolderPath(""),
      referenceFileName(""),
      destinationUrl(""),
      importUnmappedReads(false),
      deselectAll(false) {
    settings.timeout = 120000;
}

void ImportBAMFileFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    if (!referenceFolderPath.isEmpty()) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, referenceFolderPath, referenceFileName));
        GTWidget::click(os, GTWidget::findWidget(os, "refUrlButton", dialog));
    }

    if (!destinationUrl.isEmpty()) {
        GTLineEdit::setText(os, "destinationUrlEdit", destinationUrl, dialog);
    }

    auto importUnmapped = GTWidget::findCheckBox(os, "importUnmappedBox", dialog);
    if (importUnmapped->isChecked() != importUnmappedReads) {
        GTCheckBox::setChecked(os, importUnmapped, importUnmapped);
    }

    if (deselectAll) {
        auto deselectAllButton = GTWidget::findToolButton(os, "selectNoneToolButton", dialog);
        GTWidget::click(os, deselectAllButton);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
