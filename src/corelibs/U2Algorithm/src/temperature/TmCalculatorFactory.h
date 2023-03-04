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

namespace U2 {

class TmCalculator;
class TmCalculatorSettingsWidget;

/**
 * Factory, which creates temperature calculator and widget with the set of corresponding settings
 */
class U2ALGORITHM_EXPORT TmCalculatorFactory {
public:
    TmCalculatorFactory(const QString& id, const QString& visualName, int defaultPriority);

    virtual ~TmCalculatorFactory() = default;

    /** Creates temperature calculator pre-configured with the given settings. */
    virtual QSharedPointer<TmCalculator> createCalculator(const QVariantMap& settings) const = 0;

    /** Creates default settings of the default temperature calculator. */
    virtual QVariantMap createDefaultSettings() const = 0;

    /** Creates a widget with a settings for the given TmCalculator. */
    virtual TmCalculatorSettingsWidget* createSettingsWidget(QWidget* parent) const = 0;

    /** Returns id of the factory. The method is required by IdRegistry. */
    const QString& getId() const;

    const QString id;

    const QString visualName;

    /**
     * Priority of this algorithm to be selected as the default in UGENE.
     * The built-in 'Rough' algorithm has 0 priority. Any higher value will be preferable over '0'.
     */
    const int defaultPriority;
};

}  // namespace U2
