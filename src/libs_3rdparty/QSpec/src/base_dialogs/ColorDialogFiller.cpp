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

#include "base_dialogs/ColorDialogFiller.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QColor>
#include <QSpinBox>
#include <QWidget>

namespace HI {

#define GT_CLASS_NAME "GTUtilsDialog::ColorDialogFiller"

#define GT_METHOD_NAME "commonScenario"
void ColorDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    QList<QSpinBox*> spinList = dialog->findChildren<QSpinBox*>();
    GTSpinBox::setValue(spinList.at(3), r, GTGlobals::UseKeyBoard);
    GTSpinBox::setValue(spinList.at(4), g, GTGlobals::UseKeyBoard);
    GTSpinBox::setValue(spinList.at(5), b, GTGlobals::UseKeyBoard);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace HI
