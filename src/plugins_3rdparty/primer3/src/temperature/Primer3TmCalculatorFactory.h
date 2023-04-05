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

#include <QCoreApplication>

#include <U2Algorithm/TmCalculatorFactory.h>

namespace U2 {

/**
 * @Primer3TmCalculatorFactory class could create instances of @Primer3TmCalculator and @Primer3TmCalculatorSettingsWidget
 */
class Primer3TmCalculatorFactory : public TmCalculatorFactory {
    Q_DECLARE_TR_FUNCTIONS(Primer3TmCalculatorFactory)
public:
    Primer3TmCalculatorFactory();

    QSharedPointer<TmCalculator> createCalculator(const QVariantMap& settings) const override;
    QVariantMap createDefaultSettings() const override;
    TmCalculatorSettingsWidget* createSettingsWidget(QWidget* parent) const override;
};

}  // namespace U2
