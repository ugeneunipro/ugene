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

#include "GenerateAlignmentProfileDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTGroupBox.h>

#include <QApplication>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::GenerateAlignmentProfileDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void GenerateAlignmentProfileDialogFiller::commonScenario() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    GTGlobals::sleep(500);
    if(counts){
        QRadioButton* countsRB = dialog->findChild<QRadioButton*>("countsRB");
        GTRadioButton::click(os,countsRB);
    }
    else{
        QRadioButton* percentsRB = dialog->findChild<QRadioButton*>("percentsRB");
        GTRadioButton::click(os,percentsRB);
    }

    if (format != NONE) {
        GTGroupBox::setChecked(os, "saveBox", dialog);
        GTGlobals::sleep(500);

        QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileEdit");
        GTLineEdit::setText(os,fileEdit, filePath);

        QRadioButton* formatRB = dialog->findChild<QRadioButton*>(checkBoxItems[format]);
        GTRadioButton::click(os,formatRB);
    }

    QCheckBox* gapCB = dialog->findChild<QCheckBox*>("gapCB");
    GTCheckBox::setChecked(os,gapCB,gapScore);

    QCheckBox* unusedCB = dialog->findChild<QCheckBox*>("unusedCB");
    GTCheckBox::setChecked(os,unusedCB,symdolScore);

    QCheckBox* skipGapPositionsCB = dialog->findChild<QCheckBox*>("skipGapPositionsCB");
    GTCheckBox::setChecked(os,skipGapPositionsCB,skipGaps);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
