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
#pragma once

#include <core/GUITestOpStatus.h>

#include <QWidget>

namespace U2 {

using namespace HI;

/**
 * @GTUtilsMeltingTemperature class has utilit functions about melting temperature calculation
 */
class GTUtilsMeltingTemperature {
public:
    GTUtilsMeltingTemperature() = delete;

    enum class Parameter {
        Algorithm,
        DnaConc,
        MonovalentConc,
        DivalentConc,
        DntpConc,
        DmsoConc,
        DmsoFactor,
        FormamideConc,
        MaxLen,
        ThermodynamicTable,
        SaltCorrectionFormula
    };

    /**
     * This function sets melting temperature parameters.
     * Settings dialog or widget should be on the screen
     * @os operation status
     * @parameters parameters to set
     * @parent parent
     */
    static void setParameters(HI::GUITestOpStatus& os, const QMap<Parameter, QString>& parameters, QWidget* parent);

};

}
