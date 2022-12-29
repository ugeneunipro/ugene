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

#ifndef _U2_TEMP_CALC_REGISTRY_
#define _U2_TEMP_CALC_REGISTRY_

#include <U2Core/IdRegistry.h>
#include <U2Core/global.h>

#include <U2Algorithm/BaseTempCalc.h>
#include <U2Algorithm/TempCalcFactory.h>

namespace U2 {

/**
 * Registry for all methods of temperature calculation    
 */
class U2ALGORITHM_EXPORT TempCalcRegistry : public IdRegistry<TempCalcFactory> {
public:
    bool registerEntry(TempCalcFactory* t) override;
    /**
     * Get the default temperature calculator
     * Get the factory with the corresponding ID @saveId
     * If @saveId is empty - get the first registred factory
     * @return pointer to the temperature calculator
     */
    BaseTempCalc* getDefaultTempCalculator(const QString& saveId = "") const;
    /**
     * Get the default temperature calculator settings 
     * @return pointer to the temperature calculator settings
     */
    TempCalcSettings* getDefaultTempCalcSettings() const;
    /**
     * Save calculation settings to this static object
     * This is required if, for example, we need to store settings,
     * which are stored in some other object, but this object should be destroyed
     * (for example, if this object is some widget and should be closed).
     * @saveId ID of the saved settings. If it is settings of some Sequence View you can use its AnnotatedDNAView name
     * If @saveId is empty - get the first registred factory
     * @return pointer to the temperature calculator
     */
    void saveSettings(const QString& saveId, TempCalcSettings* settings);

private:
    TempCalcFactory* defaultFactory = nullptr;
    QMap<QString, QMap<QString, QVariant>> savedSettings;
};

}

#endif
