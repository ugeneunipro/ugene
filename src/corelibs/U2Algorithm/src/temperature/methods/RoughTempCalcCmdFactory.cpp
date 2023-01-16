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

#include "RoughTempCalc.h"
#include "RoughTempCalcCmdFactory.h"

namespace U2 {

const QString RoughTempCalcCmdFactory::ID = "Rough";

RoughTempCalcCmdFactory::RoughTempCalcCmdFactory()
    : TempCalcFactory(ID) {}

BaseTempCalc* RoughTempCalcCmdFactory::createTempCalculator(TempCalcSettings* settings) const {
    return new RoughTempCalc(settings);
}

BaseTempCalc* RoughTempCalcCmdFactory::createTempCalculator(QMap<QString, QVariant> mapSettings) const {
    auto settings = new TempCalcSettings;
    settings->fromVariantMap(mapSettings);
    return new RoughTempCalc(settings);
}

BaseTempCalc* RoughTempCalcCmdFactory::createDefaultTempCalculator() const {
    return createTempCalculator(createDefaultTempCalcSettings());
}

TempCalcSettings* RoughTempCalcCmdFactory::createDefaultTempCalcSettings() const {
    auto settings = new TempCalcSettings;
    settings->id = ID;
    return settings;
}

BaseTempCalcWidget* RoughTempCalcCmdFactory::createTempCalcSettingsWidget(QWidget* parent, const QString& id) const {
    return nullptr;
}

}