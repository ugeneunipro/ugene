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

#include "RangeSelectorFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ZoomToRangeDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ZoomToRangeDialogFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *from = dialog->findChild<QLineEdit*>("start_edit_line");
    QLineEdit *to = dialog->findChild<QLineEdit*>("end_edit_line");
    GT_CHECK(from != NULL, "\"start_edit_line\" to  not found");
    GT_CHECK(to != NULL, "\"end_edit_line\" to  not found");

    if(minVal != -1){
        GTLineEdit::setText(os, from, QString::number(minVal));
    }
    if(maxVal != -1){
        GTLineEdit::setText(os, to, QString::number(maxVal));
    }

    GTWidget::click(os, GTWidget::findButtonByText(os, "Ok", dialog));
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
