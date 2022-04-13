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

#include "MuscleDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::MuscleDialogFiller"

MuscleDialogFiller::MuscleDialogFiller(HI::GUITestOpStatus& os, Mode _mode, bool _doNotReArr, bool translateToAmino)
    : Filler(os, "MuscleAlignmentDialog"), mode(_mode), doNotReArr(_doNotReArr), translateToAmino(translateToAmino) {
}

#define GT_METHOD_NAME "commonScenario"
void MuscleDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    auto modeBox = GTWidget::findComboBox(os, "confBox", dialog);
    GTComboBox::selectItemByIndex(os, modeBox, mode);

    auto stableCB = GTWidget::findCheckBox(os, "stableCB", dialog);
    GTCheckBox::setChecked(os, stableCB, doNotReArr);

    auto translate2AminoCb = GTWidget::findCheckBox(os, "translateCheckBox", dialog);
    GTCheckBox::setChecked(os, translate2AminoCb, translateToAmino);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
