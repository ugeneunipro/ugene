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

#include "MeltingTemperatureSettingsDialogFiller.h"

#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include "GTGlobals.h"

namespace U2 {

#define GT_CLASS_NAME "MeltingTemperatureSettingsDialogFiller"
MeltingTemperatureSettingsDialogFiller::MeltingTemperatureSettingsDialogFiller(HI::GUITestOpStatus& os, const QMap<Parameter, QString>& _parameters)
    : Filler(os, "MeltingTemperatureCalculationDialog"),
      parameters(_parameters) {}

#define GT_METHOD_NAME "commonScenario"
void MeltingTemperatureSettingsDialogFiller::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget(os);
    GT_CHECK(parameters.keys().contains(Parameter::Algorithm), "Algorithm wasn't set up");

    auto algorithm = parameters.value(Parameter::Algorithm);
    auto cbAlgorithm = GTWidget::findComboBox(os, "cbAlgorithm", dialog);
    const auto& parameterKeys = parameters.keys();
    GT_CHECK(parameterKeys.contains(Parameter::Algorithm), "Algorithm wasn't set up");

    auto algId = parameters.value(Parameter::Algorithm);
    auto allAlgorithms = GTComboBox::getValues(os, cbAlgorithm);
    GT_CHECK(allAlgorithms.contains(algId), QString("Unexpected algorithm ID: %1").arg(algId));

    GTComboBox::selectItemByText(os, cbAlgorithm, algId);
    if (algId == "Primer 3") {
        if (parameterKeys.contains(DnaConc)) {
            GTDoubleSpinbox::setValue(os, "dsbDna", parameters.value(DnaConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(MonovalentConc)) {
            GTDoubleSpinbox::setValue(os, "dsbMonovalent", parameters.value(MonovalentConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(DivalentConc)) {
            GTDoubleSpinbox::setValue(os, "dsbDivalent", parameters.value(DivalentConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(DntpConc)) {
            GTDoubleSpinbox::setValue(os, "dsbDntp", parameters.value(DntpConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(DmsoConc)) {
            GTDoubleSpinbox::setValue(os, "dsbDmso", parameters.value(DmsoConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(DmsoFactor)) {
            GTDoubleSpinbox::setValue(os, "dsbDmsoFactor", parameters.value(DmsoFactor).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(FormamideConc)) {
            GTDoubleSpinbox::setValue(os, "dsbFormamide", parameters.value(FormamideConc).toDouble(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(MaxLen)) {
            GTSpinBox::setValue(os, "sbNnMaxLength", parameters.value(MaxLen).toInt(), GTGlobals::UseKeyBoard, dialog);
        }
        if (parameterKeys.contains(ThermodynamicTable)) {
            GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbTable", dialog), parameters.value(ThermodynamicTable).toInt());
        }
        if (parameterKeys.contains(SaltCorrectionFormula)) {
            GTComboBox::selectItemByIndex(os, GTWidget::findComboBox(os, "cbSalt", dialog), parameters.value(SaltCorrectionFormula).toInt());
        }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME


}