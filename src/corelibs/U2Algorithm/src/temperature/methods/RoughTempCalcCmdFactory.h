/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ROUGHT_MELT_TEMP_CALC_CMD_FACTORY_
#define _U2_ROUGHT_MELT_TEMP_CALC_CMD_FACTORY_

#include <U2Algorithm/TempCalcFactory.h>

#include <U2Core/global.h>

namespace U2 {

/**
 * This implementation is intended to be used from ugenecl, because it doesn't have any dependencies from QtWidgets
 */
class U2ALGORITHM_EXPORT RoughTempCalcCmdFactory : public TempCalcFactory {
public:
    RoughTempCalcCmdFactory();

    BaseTempCalc* createTempCalculator(TempCalcSettings* settings) const override;
    BaseTempCalc* createTempCalculator(QMap<QString, QVariant> mapSettings) const override;
    BaseTempCalc* createDefaultTempCalculator() const override;
    TempCalcSettings* createDefaultTempCalcSettings() const override;
    BaseTempCalcWidget* createTempCalcSettingsWidget(QWidget* parent, const QString& id) const override;

    static const QString ID;

};

}

#endif
