#include "Primer3TmCalculatorFactory.h"
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

#include "Primer3TmCalculator.h"
#include "Primer3TmCalculatorSettingsWidget.h"

namespace U2 {

Primer3TmCalculatorFactory::Primer3TmCalculatorFactory()
    : TmCalculatorFactory("primer3-tm-algorithm", tr("Primer 3"), 1) {  // Weight = 1 adds more priority to Primer3 algorithm than the default one.
}

QSharedPointer<TmCalculator> Primer3TmCalculatorFactory::createCalculator(const QVariantMap& settings) const {
    return QSharedPointer<TmCalculator>(new Primer3TmCalculator(settings));
}

QVariantMap Primer3TmCalculatorFactory::createDefaultSettings() const {
    QVariantMap settings;
    settings.insert(TmCalculator::KEY_ID, id);
    settings.insert(Primer3TmCalculator::KEY_DNA_CONC, Primer3TmCalculator::DNA_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_SALT_CONC, Primer3TmCalculator::SALT_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_DIVALENT_CONC, Primer3TmCalculator::DIVALENT_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_DNTP_CONC, Primer3TmCalculator::DNTP_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_DMSO_CONC, Primer3TmCalculator::DMSO_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_DMSO_FACT, Primer3TmCalculator::DMSO_FACT_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_FORMAMIDE_CONC, Primer3TmCalculator::FORMAMIDE_CONC_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_MAX_LEN, Primer3TmCalculator::NN_MAX_LEN_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_TM_METHOD, Primer3TmCalculator::TM_METHOD_DEFAULT);
    settings.insert(Primer3TmCalculator::KEY_SALT_CORRECTION, Primer3TmCalculator::SALT_CORRECTIONS_DEFAULT);

    return settings;
}

TmCalculatorSettingsWidget* Primer3TmCalculatorFactory::createSettingsWidget(QWidget* parent) const {
    return new Primer3TmCalculatorSettingsWidget(parent, id);
}

}  // namespace U2
