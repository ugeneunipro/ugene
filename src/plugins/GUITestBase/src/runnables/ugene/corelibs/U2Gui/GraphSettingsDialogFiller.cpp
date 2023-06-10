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

#include <base_dialogs/ColorDialogFiller.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QGroupBox>

#include "GraphSettingsDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::GraphSettingsDialogFiller"
#define GT_METHOD_NAME "run"

GraphSettingsDialogFiller::GraphSettingsDialogFiller(
    int _window,
    int _steps,
    double _cutoff_min,
    double _cutoff_max,
    int _r,
    int _g,
    int _b)
    : Filler("GraphSettingsDialog"),
      window(_window),
      steps(_steps),
      cutoff_min(_cutoff_min),
      cutoff_max(_cutoff_max),
      r(_r),
      g(_g),
      b(_b) {
}

GraphSettingsDialogFiller::GraphSettingsDialogFiller(CustomScenario* c)
    : Filler("GraphSettingsDialog", c),
      window(0),
      steps(0),
      cutoff_min(0),
      cutoff_max(0),
      r(0),
      g(0),
      b(0) {
}

void GraphSettingsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (window != -1) {
        auto windowEdit = GTWidget::findSpinBox("windowEdit", dialog);
        GTSpinBox::setValue(windowEdit, window, GTGlobals::UseKeyBoard);
    }

    if (steps != -1) {
        auto stepsPerWindowEdit = GTWidget::findSpinBox("stepsPerWindowEdit", dialog);
        GTSpinBox::setValue(stepsPerWindowEdit, steps, GTGlobals::UseKeyBoard);
    }

    if (cutoff_max != 0 || cutoff_min != 0) {
        auto minmaxGroup = GTWidget::findGroupBox("minmaxGroup", dialog);
        GTGroupBox::setChecked(minmaxGroup);
    }

    if (cutoff_min != 0) {
        auto minBox = GTWidget::findDoubleSpinBox("minBox", dialog);
        GTDoubleSpinbox::setValue(minBox, cutoff_min, GTGlobals::UseKeyBoard);
    }

    if (cutoff_max != 0) {
        auto maxBox = GTWidget::findDoubleSpinBox("maxBox", dialog);
        GTDoubleSpinbox::setValue(maxBox, cutoff_max, GTGlobals::UseKeyBoard);
    }

    if (r != -1 && g != -1 && b != -1) {
        GTUtilsDialog::waitForDialog(new ColorDialogFiller(r, g, b));
        GTWidget::click(GTWidget::findWidget("Default color", dialog));
    }

    GTGlobals::sleep(200);
    GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
