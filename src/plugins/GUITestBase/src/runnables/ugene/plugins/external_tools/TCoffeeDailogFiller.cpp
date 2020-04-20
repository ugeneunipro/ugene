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

#include "TCoffeeDailogFiller.h"

#include <QApplication>
#include <QDialogButtonBox>

#include <primitives/GTWidget.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTSpinBox.h>

namespace U2{

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"
#define GT_METHOD_NAME "commonScenario"

TCoffeeDailogFiller::TCoffeeDailogFiller(HI::GUITestOpStatus &os, int gapOpen, int gapExt, int numOfIters) :
    Filler(os, "TCoffeeSupportRunDialog"),
    gapOpen(gapOpen),
    gapExt(gapExt),
    numOfIters(numOfIters)
{

}

void TCoffeeDailogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (gapOpen!=INT_MAX){
        QCheckBox* gapOpenCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "gapOpenCheckBox", dialog));
        GTCheckBox::setChecked(os,gapOpenCheckBox,true);

        QSpinBox* gapOpenSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "gapOpenSpinBox", dialog));
        GTSpinBox::setValue(os, gapOpenSpinBox, gapOpen);
    }

    if (gapExt!=INT_MAX){
        QCheckBox* gapExtCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "gapExtCheckBox", dialog));
        GTCheckBox::setChecked(os,gapExtCheckBox,true);

        QSpinBox* gapExtSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "gapExtSpinBox", dialog));
        GTSpinBox::setValue(os, gapExtSpinBox, gapExt);
    }

    if (numOfIters!=INT_MAX){
        QCheckBox* maxNumberIterRefinementCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "maxNumberIterRefinementCheckBox", dialog));
        GTCheckBox::setChecked(os,maxNumberIterRefinementCheckBox, true);

        QSpinBox* maxNumberIterRefinementSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "maxNumberIterRefinementSpinBox", dialog));
        GTSpinBox::setValue(os, maxNumberIterRefinementSpinBox, numOfIters);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}
