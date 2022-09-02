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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QLabel>

#include "DocumentFormatSelectorDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "DocumentFormatSelectorDialogFiller"

#define GT_METHOD_NAME "getButtonItem"
QRadioButton* DocumentFormatSelectorDialogFiller::getButton(HI::GUITestOpStatus& os) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    return GTWidget::findRadioButton(os, format, dialog, {false});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "run"
void DocumentFormatSelectorDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTGlobals::sleep(500);

    QRadioButton* radio = getButton(os);
    if (radio != nullptr) {
        if (score != -1) {
            GT_CHECK(formatLineLable != -1, "line is not defined");

            auto label = GTWidget::findLabel(os, QString("label_%1").arg(formatLineLable), dialog);
            QString sign = label->text();
            QRegExp regExp(QString("<b>%1</b> format. Score: (\\d+)").arg(format));
            regExp.indexIn(sign);
            int currentScore = regExp.cap(1).toInt();
            GT_CHECK(currentScore == score, QString("Unexpected similarity score, expected: %1, current: %2").arg(score).arg(currentScore));
        }

        GTRadioButton::click(os, radio);
    } else {
        auto chooseFormatManuallyRadio = GTWidget::findRadioButton(os, "chooseFormatManuallyRadio", dialog);
        GTRadioButton::click(os, chooseFormatManuallyRadio);
        GTGlobals::sleep();

        auto userSelectedFormat = GTWidget::findComboBox(os, "userSelectedFormat", dialog);
        GTComboBox::selectItemByText(os, userSelectedFormat, format, GTGlobals::UseMouse);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME
#undef GT_METHOD_NAME

}  // namespace U2
