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

#include "BranchSettingsDialogFiller.h"
#include <QApplication>
#include <QPushButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QDialogButtonBox>

#include <primitives/GTWidget.h>
#include <primitives/GTSpinBox.h>
#include <base_dialogs/ColorDialogFiller.h>

namespace U2{

#define GT_CLASS_NAME "GTUtilsDialog::BranchSettingsDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void BranchSettingsDialogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog!=NULL, "Active modal widget not found");

    GTGlobals::sleep(500);
    GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os,0,0,255));
    QPushButton* colorButton = dialog->findChild<QPushButton*>("colorButton");
    GTWidget::click(os, colorButton);

    QSpinBox* thicknessSpinBox = dialog->findChild<QSpinBox*>("thicknessSpinBox");
    GTSpinBox::setValue(os,thicknessSpinBox,10);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}
