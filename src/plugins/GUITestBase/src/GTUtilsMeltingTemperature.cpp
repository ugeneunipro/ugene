#include "GTUtilsMeltingTemperature.h"
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

#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include "GTGlobals.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsMeltingTemperature"

#define GT_METHOD_NAME "setParameters"
void GTUtilsMeltingTemperature::setParameters(HI::GUITestOpStatus& os, const QMap<Parameter, QString>& parameters, QWidget* parent) {
    QList<Parameter> parameterKeys = parameters.keys();
    if (parameterKeys.contains(Parameter::Algorithm)) {
        auto cbAlgorithm = GTWidget::findComboBox(os, "cbAlgorithm", parent);
        GTComboBox::selectItemByText(os, cbAlgorithm, parameters.value(Parameter::Algorithm));
    }
    if (parameterKeys.contains(Parameter::DnaConc)) {
        GTDoubleSpinbox::setValue(os, "dsbDna", parameters.value(Parameter::DnaConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::MonovalentConc)) {
        GTDoubleSpinbox::setValue(os, "dsbMonovalent", parameters.value(Parameter::MonovalentConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::DivalentConc)) {
        GTDoubleSpinbox::setValue(os, "dsbDivalent", parameters.value(Parameter::DivalentConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::DntpConc)) {
        GTDoubleSpinbox::setValue(os, "dsbDntp", parameters.value(Parameter::DntpConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::DmsoConc)) {
        GTDoubleSpinbox::setValue(os, "dsbDmso", parameters.value(Parameter::DmsoConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::DmsoFactor)) {
        GTDoubleSpinbox::setValue(os, "dsbDmsoFactor", parameters.value(Parameter::DmsoFactor).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::FormamideConc)) {
        GTDoubleSpinbox::setValue(os, "dsbFormamide", parameters.value(Parameter::FormamideConc).toDouble(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::MaxLen)) {
        GTSpinBox::setValue(os, "sbNnMaxLength", parameters.value(Parameter::MaxLen).toInt(), GTGlobals::UseKeyBoard, parent);
    }
    if (parameterKeys.contains(Parameter::ThermodynamicTable)) {
        GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbTable", parent), parameters.value(Parameter::ThermodynamicTable).toInt());
    }
    if (parameterKeys.contains(Parameter::SaltCorrectionFormula)) {
        GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbSalt", parent), parameters.value(Parameter::SaltCorrectionFormula).toInt());
    }
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
