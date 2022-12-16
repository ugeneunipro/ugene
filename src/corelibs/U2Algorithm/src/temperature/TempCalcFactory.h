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

#ifndef _U2_TEMP_CALC_FACTORY_
#define _U2_TEMP_CALC_FACTORY_

#include <U2Core/global.h>

#include <QString>

namespace U2 {

class BaseTempCalc;
class BaseTempCalcWidget;
struct TempCalcSettings;

class U2ALGORITHM_EXPORT TempCalcFactory {
public:
    TempCalcFactory(const QString& id);

    virtual BaseTempCalc* createTempCalculator(TempCalcSettings* settings) const = 0;
    virtual BaseTempCalc* createDefaultTempCalculator() const = 0;
    virtual BaseTempCalcWidget* createTempCalcSettingsWidget(QWidget* parent, const QString& id) const = 0;

    const QString& getId() const;

private:
    const QString id;
};

}

#endif