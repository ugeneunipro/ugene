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

#include "TempCalcRegistry.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

bool TempCalcRegistry::registerEntry(TempCalcFactory* t) {
    if (defaultFactory == nullptr) {
        defaultFactory = t;
    }
    return IdRegistry::registerEntry(t);
}

QSharedPointer<BaseTempCalc> TempCalcRegistry::createDefaultTempCalculator(const QString& saveId) const {
    CHECK(!saveId.isEmpty(), defaultFactory->createDefaultTempCalculator());

    auto savedFactory = getById(savedSettings.value(saveId).value(BaseTempCalc::KEY_ID).toString());
    CHECK(savedFactory != nullptr, defaultFactory->createDefaultTempCalculator());
    
    return savedFactory->createTempCalculator(savedSettings.value(saveId));
}

TempCalcSettings TempCalcRegistry::createDefaultTempCalcSettings() const {
    return defaultFactory->createDefaultTempCalcSettings();
}

QSharedPointer<BaseTempCalc> TempCalcRegistry::createTempCalculatorBySettingsMap(const QVariantMap& settingsMap) const {
    QSharedPointer<BaseTempCalc> result;
    if (settingsMap.isEmpty()) {
        result = createDefaultTempCalculator();
    } else {
        auto settingsId = settingsMap.value(BaseTempCalc::KEY_ID).toString();
        result = getById(settingsId)->createTempCalculator(settingsMap);
    }

    return result;
}

void TempCalcRegistry::saveSettings(const QString& saveId, const TempCalcSettings& settings) {
    savedSettings.insert(saveId, settings);
}

}
