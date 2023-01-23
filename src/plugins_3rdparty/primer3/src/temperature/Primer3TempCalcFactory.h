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

#ifndef U2_PRIMER_3_MELT_TEMP_CALC_FACTORY_
#define U2_PRIMER_3_MELT_TEMP_CALC_FACTORY_

#include <U2Algorithm/TempCalcFactory.h>

namespace U2 {

/**
 * @Primer3TempCalcFactory class could create instances of @Primer3TempCalc and @Primer3TempCalcWidget
 */
class Primer3TempCalcFactory : public TempCalcFactory {
public:
    Primer3TempCalcFactory();

    BaseTempCalc* createTempCalculator(TempCalcSettings* settings) const override;
    BaseTempCalc* createTempCalculator(QMap<QString, QVariant> mapSettings) const override;
    BaseTempCalc* createDefaultTempCalculator() const override;
    TempCalcSettings* createDefaultTempCalcSettings() const override;
    BaseTempCalcWidget* createTempCalcSettingsWidget(QWidget* parent, const QString& id) const override;

private:

    static const QString ID;

};

}

#endif