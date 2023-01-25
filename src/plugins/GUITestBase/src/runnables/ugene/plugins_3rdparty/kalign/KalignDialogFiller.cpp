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

#include "KalignDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"

KalignDialogFiller::KalignDialogFiller(HI::GUITestOpStatus& _os, int _gapOpenVal, bool _toAmino)
    : Filler(_os, "KalignDialog"),
      gapOpenVal(_gapOpenVal), toAmino(_toAmino) {
}

#define GT_METHOD_NAME "commonScenario"
void KalignDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (gapOpenVal) {
        auto gapOpenCheckBox = GTWidget::findCheckBox(os, "gapOpenCheckBox", dialog);
        GTCheckBox::setChecked(os, gapOpenCheckBox, true);

        auto gapOpenSpinBox = GTWidget::findDoubleSpinBox(os, "gapOpenSpinBox", dialog);
        GTDoubleSpinbox::setValue(os, gapOpenSpinBox, gapOpenVal, GTGlobals::UseKeyBoard);
    }

    auto translateCheckBox = GTWidget::findCheckBox(os, "translateCheckBox", dialog);
    GTCheckBox::setChecked(os, translateCheckBox, toAmino);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
