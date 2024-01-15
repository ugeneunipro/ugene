/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "ImportACEFileDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ImportBAMFileFiller"
ImportACEFileFiller::ImportACEFileFiller(
    bool _isReadOnly,
    QString dstUrl,
    QString srcUrl,
    int timeoutMs)
    : Filler("Select Document Format"),
      isReadOnly(_isReadOnly),
      sourceUrl(srcUrl),
      destinationUrl(dstUrl) {
    settings.timeout = timeoutMs;
}

ImportACEFileFiller::ImportACEFileFiller(CustomScenario* _c)
    : Filler("Select Document Format", _c),
      isReadOnly(false) {
    settings.timeout = 120000;
}

void ImportACEFileFiller::commonScenario() {
    GTGlobals::sleep(500);
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (isReadOnly) {
        auto rb = GTWidget::findRadioButton("0_radio", dialog);
        GTRadioButton::click(rb);
        GTGlobals::sleep();
    } else {
        auto rb = GTWidget::findRadioButton("1_radio", dialog);
        GTRadioButton::click(rb);
        GTGlobals::sleep();

        if (!sourceUrl.isEmpty()) {
            GTLineEdit::setText("sourseLineEdit", sourceUrl, QApplication::activeModalWidget());
        }

        if (!destinationUrl.isEmpty()) {
            GTLineEdit::setText("fileNameEdit", destinationUrl, QApplication::activeModalWidget(), dialog);
        }
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME

}  // namespace U2
