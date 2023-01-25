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

#include "DotPlotDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"
#define GT_METHOD_NAME "commonScenario"

DotPlotFiller::DotPlotFiller(HI::GUITestOpStatus& _os, CustomScenario* customScenario)
    : Filler(_os, "DotPlotDialog", customScenario),
      minLen(100),
      identity(0),
      invertedRepeats(false),
      but1kpressed(false) {
}

void DotPlotFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    auto minLenBox = GTWidget::findSpinBox(os, "minLenBox", dialog);
    if (but1kpressed) {
        GTWidget::click(os, GTWidget::findWidget(os, "minLenHeuristicsButton", dialog));
        GTGlobals::sleep();
        GT_CHECK(minLenBox->value() == 2, "minLem not 2, 1k button works wrong");
    } else
        GTSpinBox::setValue(os, minLenBox, minLen, GTGlobals::UseKeyBoard);

    if (identity) {
        auto identityBox = GTWidget::findSpinBox(os, "identityBox", dialog);
        GTSpinBox::setValue(os, identityBox, identity, GTGlobals::UseKeyBoard);
    }

    auto invertedCheckBox = GTWidget::findCheckBox(os, "invertedCheckBox", dialog);
    GTCheckBox::setChecked(os, invertedCheckBox, invertedRepeats);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
