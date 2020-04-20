/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "FontDialogFiller.h"
#include <primitives/GTWidget.h>
#include <drivers/GTKeyboardDriver.h>

#include <QApplication>
#include <QAbstractButton>

namespace HI {

#define GT_CLASS_NAME "GTUtilsDialog::FontDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void FontDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog is NULL");
    GTGlobals::sleep();

    GTKeyboardDriver::keyClick( Qt::Key_Enter);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}
