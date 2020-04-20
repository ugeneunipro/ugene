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

#include "KalignDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTDoubleSpinBox.h>

#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"

KalignDialogFiller::KalignDialogFiller(HI::GUITestOpStatus &_os, int _gapOpenVal, bool _toAmino) : Filler(_os, "KalignDialog"),
    gapOpenVal(_gapOpenVal), toAmino(_toAmino) {}

#define GT_METHOD_NAME "commonScenario"
void KalignDialogFiller::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if(gapOpenVal){
        QCheckBox* gapOpenCheckBox = dialog->findChild<QCheckBox*>("gapOpenCheckBox");
        GTCheckBox::setChecked(os,gapOpenCheckBox, true);

        QDoubleSpinBox* gapOpenSpinBox = dialog->findChild<QDoubleSpinBox*>("gapOpenSpinBox");
        GTDoubleSpinbox::setValue(os,gapOpenSpinBox,gapOpenVal,GTGlobals::UseKeyBoard);
    }

    QCheckBox* translateCheckBox = GTWidget::findExactWidget<QCheckBox*>(os, "translateCheckBox", dialog);
    GTCheckBox::setChecked(os, translateCheckBox, toAmino);
    
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
