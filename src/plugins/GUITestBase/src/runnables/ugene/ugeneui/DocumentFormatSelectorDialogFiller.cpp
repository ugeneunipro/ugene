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

#include <QApplication>
#include <QLabel>

#include <primitives/GTComboBox.h>
#include <primitives/GTRadioButton.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include "DocumentFormatSelectorDialogFiller.h"

namespace U2{

#define GT_CLASS_NAME "DocumentFormatSelectorDialogFiller"

#define GT_METHOD_NAME "getButton"
QRadioButton* DocumentFormatSelectorDialogFiller::getButton(HI::GUITestOpStatus &os){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", NULL);
    QRadioButton* result = GTWidget::findExactWidget<QRadioButton*>(os, format, dialog, GTGlobals::FindOptions(false));

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "run"
void DocumentFormatSelectorDialogFiller::commonScenario()
{
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");
    GTGlobals::sleep(500);

    QRadioButton* radio = getButton(os);
    if (NULL != radio) {
        if (-1 != score) {
            GT_CHECK(formatLineLable != -1, "line is not defined");

            QLabel* label = GTWidget::findExactWidget<QLabel*>(os, QString("label_%1").arg(formatLineLable), dialog, GTGlobals::FindOptions(false));
            GT_CHECK(label, "label is NULL");

            const QString sign = label->text();
            QRegExp regExp(QString("<b>%1</b> format. Score: (\\d+)").arg(format));
            regExp.indexIn(sign);
            int currentScore = regExp.cap(1).toInt();
            GT_CHECK(currentScore == score, QString("Unexpected similarity score, expected: %1, current: %2").arg(score).arg(currentScore));
        }

        GTRadioButton::click(os, radio);
    } else {
        QRadioButton* chooseFormatManuallyRadio = GTWidget::findExactWidget<QRadioButton*>(os, "chooseFormatManuallyRadio", dialog);
        GTRadioButton::click(os, chooseFormatManuallyRadio);
        GTGlobals::sleep();

        QComboBox* userSelectedFormat = GTWidget::findExactWidget<QComboBox*>(os, "userSelectedFormat", dialog);
        GTComboBox::setIndexWithText(os, userSelectedFormat, format, true, GTGlobals::UseMouse);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME
#undef GT_METHOD_NAME

}
