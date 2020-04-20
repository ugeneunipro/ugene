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

#include "MuscleDialogFiller.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTWidget.h>
#include <primitives/GTCheckBox.h>

#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::MuscleDialogFiller"

MuscleDialogFiller::MuscleDialogFiller(HI::GUITestOpStatus &os, Mode _mode, bool _doNotReArr, bool translateToAmino)
    : Filler(os, "MuscleAlignmentDialog"), mode(_mode), doNotReArr(_doNotReArr), translateToAmino(translateToAmino)
{

}

#define GT_METHOD_NAME "commonScenario"
void MuscleDialogFiller::commonScenario(){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QComboBox *modeBox = dialog->findChild<QComboBox*>("confBox");
    GT_CHECK(modeBox != NULL, "combobox not found");
    GTComboBox::setCurrentIndex(os, modeBox, mode);

    QCheckBox *stableCB = dialog->findChild<QCheckBox*>("stableCB");
    GTCheckBox::setChecked(os, stableCB, doNotReArr);

    QCheckBox *translate2AminoCb = dialog->findChild<QCheckBox*>("translateCheckBox");
    GTCheckBox::setChecked(os, translate2AminoCb, translateToAmino);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}


#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} //ns
