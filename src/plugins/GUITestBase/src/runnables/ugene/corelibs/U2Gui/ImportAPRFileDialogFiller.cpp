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

#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>

#include "ImportAPRFileDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ImportBAMFileFiller"
#define GT_METHOD_NAME "run"
ImportAPRFileFiller::ImportAPRFileFiller(HI::GUITestOpStatus& os,
                                         bool _isReadOnly,
                                         QString dstUrl,
                                         QString formatToConvertTo,
                                         int timeoutMs)
    : Filler(os, "Select Document Format"),
      isReadOnly(_isReadOnly),
      destinationUrl(dstUrl),
      format(formatToConvertTo) {
    settings.timeout = timeoutMs;
}

ImportAPRFileFiller::ImportAPRFileFiller(HI::GUITestOpStatus& os, CustomScenario* _c)
    : Filler(os, "Select Document Format", _c),
      isReadOnly(false) {
    settings.timeout = 120000;
}

void ImportAPRFileFiller::commonScenario() {
    GTGlobals::sleep(500);
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (isReadOnly) {
        auto rb = GTWidget::findRadioButton(os, "0_radio", dialog);
        GTRadioButton::click(os, rb);
        GTGlobals::sleep();
    } else {
        auto rb = GTWidget::findRadioButton(os, "1_radio", dialog);
        GTRadioButton::click(os, rb);
        GTGlobals::sleep();

        if (!destinationUrl.isEmpty()) {
            GTLineEdit::setText(os, "fileNameEdit", destinationUrl, QApplication::activeModalWidget());
        }

        if (!format.isEmpty()) {
            auto cb = GTWidget::findComboBox(os, "formatCombo", dialog);
            GTComboBox::selectItemByText(os, cb, format);
        }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
