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

#include <QString>

#include <U2Core/global.h>

#include "BaseTempCalc.h"

namespace U2 {

class BaseTempCalc;
class BaseTempCalcWidget;

/**
 * Factory, which creates temperature calculator and widget with the set of corresponding settings
 */
class U2ALGORITHM_EXPORT TempCalcFactory {
public:
    TempCalcFactory(const QString& id, const QString& visualName);
    virtual ~TempCalcFactory() = default;

    /**
     * Creates temperature calculator.
     * @settings settings of the calculator, which should be created.
     * @return pointer to the temperature calculator.
     */
    virtual QSharedPointer<BaseTempCalc> createTempCalculator(const TempCalcSettings& settings) const = 0;
    /**
     * Creates temperature calculator with the default settings.
     * @return pointer to the temperature calculator.
     */
    virtual QSharedPointer<BaseTempCalc> createDefaultTempCalculator() const = 0;
    /**
     * Creates default settings of the default temperature calculator.
     * @return temperature calculator settings.
     */
    virtual TempCalcSettings createDefaultTempCalcSettings() const = 0;
    /**
     * Creates widget to set manually settings and get TempCalcSettings from this widget.
     * @return pointer to the temperature calculator.
     */
    virtual BaseTempCalcWidget* createTempCalcSettingsWidget(QWidget* parent) const = 0;

    /** Returns id of the factory. The method is required by IdRegistry. */
    const QString& getId() const;

    const QString id;

    const QString visualName;
};

}  // namespace U2
