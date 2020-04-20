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

#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include "DatasetNameEditDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "DatasetNameEditDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void DatasetNameEditDialogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if(!datasetName.isEmpty()){
        QLineEdit* line = dialog->findChild<QLineEdit*>();
        GT_CHECK(line, "lineEdit not found");
        GTLineEdit::setText(os, line, datasetName);
    }
    GTWidget::click(os, GTWidget::findButtonByText(os, "OK", dialog));
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
