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

#include <U2Algorithm/TempCalcFactory.h>

#include <U2Core/global.h>

#include <QCoreApplication>
#include <QSharedPointer>

namespace U2 {

/**
 * @RoughTempCalcCmdFactory class could create instances of @RoughTempCalc
 * This implementation is intended to be used from ugenecl, because it doesn't have any dependencies from QtWidgets
 * and can't create @BaseTempCalcWidget object
 */
class U2ALGORITHM_EXPORT RoughTempCalcCmdFactory : public TempCalcFactory {
    Q_DECLARE_TR_FUNCTIONS(RoughTempCalcCmdFactory)
public:
    RoughTempCalcCmdFactory();

    QSharedPointer<BaseTempCalc> createTempCalculator(const TempCalcSettings& settings) const override;
    QSharedPointer<BaseTempCalc> createDefaultTempCalculator() const override;
    TempCalcSettings createDefaultTempCalcSettings() const override;
    BaseTempCalcWidget* createTempCalcSettingsWidget(QWidget* parent, const QString& id) const override;

};

}
