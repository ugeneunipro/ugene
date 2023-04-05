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

#include "TmCalculatorRegistry.h"

#include <U2Core/U2SafePoints.h>

#include "TmCalculator.h"
#include "TmCalculatorFactory.h"

namespace U2 {

bool TmCalculatorRegistry::registerEntry(TmCalculatorFactory* newFactory) {
    if (defaultFactory == nullptr || defaultFactory->defaultPriority < newFactory->defaultPriority) {
        defaultFactory = newFactory;
    }
    return IdRegistry::registerEntry(newFactory);
}

QSharedPointer<TmCalculator> TmCalculatorRegistry::createTmCalculator(const QString& settingsId) const {
    auto settings = loadSettings(settingsId);
    auto factoryId = settings.value(TmCalculator::KEY_ID).toString();
    auto factory = getById(factoryId);
    CHECK(factory != nullptr, defaultFactory->createCalculator(defaultFactory->createDefaultSettings()));
    return factory->createCalculator(savedSettings.value(settingsId));
}

TmCalculatorFactory* TmCalculatorRegistry::getDefaultTmCalculatorFactory() const {
    SAFE_POINT(defaultFactory != nullptr, "defaultFactory is null!", nullptr);
    return defaultFactory;
}

QSharedPointer<TmCalculator> TmCalculatorRegistry::createTmCalculator(const QVariantMap& settingsMap) const {
    if (settingsMap.isEmpty()) {
        auto factory = getDefaultTmCalculatorFactory();
        return factory->createCalculator(factory->createDefaultSettings());
    }
    CHECK(!settingsMap.isEmpty(), nullptr);
    auto factoryId = settingsMap.value(TmCalculator::KEY_ID).toString();
    TmCalculatorFactory* calcFactory = getById(factoryId);
    CHECK(calcFactory != nullptr, nullptr);
    return calcFactory->createCalculator(settingsMap);
}

void TmCalculatorRegistry::saveSettings(const QString& settingsId, const QVariantMap& settings) {
    savedSettings.insert(settingsId, settings);
}

QVariantMap TmCalculatorRegistry::loadSettings(const QString& settingsId) const {
    return savedSettings.value(settingsId);
}

}  // namespace U2
