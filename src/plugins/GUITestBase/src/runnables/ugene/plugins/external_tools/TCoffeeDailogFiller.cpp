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

#include "TCoffeeDailogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"
#define GT_METHOD_NAME "commonScenario"

TCoffeeDailogFiller::TCoffeeDailogFiller(HI::GUITestOpStatus& os, int gapOpen, int gapExt, int numOfIters)
    : Filler(os, "TCoffeeSupportRunDialog"),
      gapOpen(gapOpen),
      gapExt(gapExt),
      numOfIters(numOfIters) {
}

void TCoffeeDailogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (gapOpen != INT_MAX) {
        auto gapOpenCheckBox = GTWidget::findCheckBox(os, "gapOpenCheckBox", dialog);
        GTCheckBox::setChecked(os, gapOpenCheckBox, true);

        auto gapOpenSpinBox = GTWidget::findSpinBox(os, "gapOpenSpinBox", dialog);
        GTSpinBox::setValue(os, gapOpenSpinBox, gapOpen);
    }

    if (gapExt != INT_MAX) {
        auto gapExtCheckBox = GTWidget::findCheckBox(os, "gapExtCheckBox", dialog);
        GTCheckBox::setChecked(os, gapExtCheckBox, true);

        auto gapExtSpinBox = GTWidget::findSpinBox(os, "gapExtSpinBox", dialog);
        GTSpinBox::setValue(os, gapExtSpinBox, gapExt);
    }

    if (numOfIters != INT_MAX) {
        auto maxNumberIterRefinementCheckBox = GTWidget::findCheckBox(os, "maxNumberIterRefinementCheckBox", dialog);
        GTCheckBox::setChecked(os, maxNumberIterRefinementCheckBox, true);

        auto maxNumberIterRefinementSpinBox = GTWidget::findSpinBox(os, "maxNumberIterRefinementSpinBox", dialog);
        GTSpinBox::setValue(os, maxNumberIterRefinementSpinBox, numOfIters);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
}  // namespace U2
