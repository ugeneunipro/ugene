#include "Primer3TempCalcFactory.h"
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

#include "Primer3TempCalc.h"
#include "Primer3TempCalcWidget.h"

namespace U2 {

Primer3TempCalcFactory::Primer3TempCalcFactory()
    : TempCalcFactory("primer3-tm-algorithm", tr("Primer 3"), -1) {  // TODO: Make Primer3 default once issues are fixed.
}

QSharedPointer<BaseTempCalc> Primer3TempCalcFactory::createCalculator(const TempCalcSettings& settings) const {
    return QSharedPointer<BaseTempCalc>(new Primer3TempCalc(settings));
}

TempCalcSettings Primer3TempCalcFactory::createDefaultSettings() const {
    TempCalcSettings settings;
    settings.insert(BaseTempCalc::KEY_ID, id);
    settings.insert(Primer3TempCalc::KEY_DNA_CONC, Primer3TempCalc::DNA_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_SALT_CONC, Primer3TempCalc::SALT_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_DIVALENT_CONC, Primer3TempCalc::DIVALENT_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_DNTP_CONC, Primer3TempCalc::DNTP_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_DMSO_CONC, Primer3TempCalc::DMSO_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_DMSO_FACT, Primer3TempCalc::DMSO_FACT_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_FORMAMIDE_CONC, Primer3TempCalc::FORMAMIDE_CONC_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_MAX_LEN, Primer3TempCalc::NN_MAX_LEN_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_TM_METHOD, Primer3TempCalc::TM_METHOD_DEFAULT);
    settings.insert(Primer3TempCalc::KEY_SALT_CORRECTION, Primer3TempCalc::SALT_CORRECTIONS_DEFAULT);

    return settings;
}

BaseTempCalcWidget* Primer3TempCalcFactory::createSettingsWidget(QWidget* parent) const {
    return new Primer3TempCalcWidget(parent, id);
}

}  // namespace U2
