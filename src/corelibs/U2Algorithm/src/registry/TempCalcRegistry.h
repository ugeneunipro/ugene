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
#pragma once

#include <QSharedPointer>

#include <U2Algorithm/BaseTempCalc.h>
#include <U2Algorithm/TempCalcFactory.h>

#include <U2Core/IdRegistry.h>
#include <U2Core/global.h>

namespace U2 {

/**
 * @TempCalcRegistry is a class to registering for all methods of temperature calculation
 */
class U2ALGORITHM_EXPORT TempCalcRegistry : public IdRegistry<TempCalcFactory> {
public:
    bool registerEntry(TempCalcFactory* factory) override;

    /** Returns the default temperature calculator factory. The returned factory is never null. */
    TempCalcFactory* getDefaultTempCalcFactory() const;

    /**
     * Creates a pre-configured 'BaseTempCalc' using saved settings ID.
     * If ID is not provided or settings are not found creates a default calculator with a default settings.
     */
    QSharedPointer<BaseTempCalc> createTempCalculator(const QString& settingsId = "") const;

    /**
     * Creates a temperature calculator instance preconfigured with the given settings.
     * If settings contains invalid algorithm ID returns a null value.
     * If settings are empty returns the default calculator.
     */
    QSharedPointer<BaseTempCalc> createTempCalculator(const QVariantMap& settingsMap) const;

    /**
     * Saves TM algorithms settings by the given 'settingsId' key.
     * The saved settings can be retrieved later within the same UGENE session using 'createCalculator' method.
     */
    void saveSettings(const QString& settingsId, const TempCalcSettings& settings);

    /** Loads previously saved settings. Returns an empty value if the settings was not found. */
    TempCalcSettings loadSettings(const QString& settingsId) const;

private:
    TempCalcFactory* defaultFactory = nullptr;
    QMap<QString, TempCalcSettings> savedSettings;
};

}  // namespace U2
