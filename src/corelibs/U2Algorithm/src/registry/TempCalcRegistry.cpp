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

bool TempCalcRegistry::registerEntry(TempCalcFactory* newFactory) {
    if (defaultFactory == nullptr || defaultFactory->defaultPriority < newFactory->defaultPriority) {
        defaultFactory = newFactory;
    }
    return IdRegistry::registerEntry(newFactory);
}

QSharedPointer<BaseTempCalc> TempCalcRegistry::createTempCalculator(const QString& settingsId) const {
    auto settings = loadSettings(settingsId);
    auto factoryId = settings.value(BaseTempCalc::KEY_ID).toString();
    auto factory = getById(factoryId);
    CHECK(factory != nullptr, defaultFactory->createCalculator(defaultFactory->createDefaultSettings()));
    return factory->createCalculator(savedSettings.value(settingsId));
}

TempCalcFactory* TempCalcRegistry::getDefaultTempCalcFactory() const {
    SAFE_POINT(defaultFactory != nullptr, "defaultFactory is null!", nullptr);
    return defaultFactory;
}

QSharedPointer<BaseTempCalc> TempCalcRegistry::createTempCalculator(const QVariantMap& settingsMap) const {
    if (settingsMap.isEmpty()) {
        auto factory = getDefaultTempCalcFactory();
        return factory->createCalculator(factory->createDefaultSettings());
    }
    CHECK(!settingsMap.isEmpty(), nullptr);
    auto factoryId = settingsMap.value(BaseTempCalc::KEY_ID).toString();
    TempCalcFactory* calcFactory = getById(factoryId);
    CHECK(calcFactory != nullptr, nullptr);
    return calcFactory->createCalculator(settingsMap);
}

void TempCalcRegistry::saveSettings(const QString& settingsId, const TempCalcSettings& settings) {
    savedSettings.insert(settingsId, settings);
}

TempCalcSettings TempCalcRegistry::loadSettings(const QString& settingsId) const {
    return savedSettings.value(settingsId);
}

}  // namespace U2
