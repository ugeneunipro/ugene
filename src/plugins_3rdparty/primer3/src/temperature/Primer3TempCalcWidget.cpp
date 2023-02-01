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

#include "Primer3TempCalcWidget.h"
#include "Primer3TempCalc.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

Primer3TempCalcWidget::Primer3TempCalcWidget(QWidget* parent, const QString& id) :
    BaseTempCalcWidget(parent, id) {
    setupUi(this);
    connect(dsbDna, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbMonovalent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbDivalent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbDntp, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbDmso, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbDmsoFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(dsbFormamide, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(sbNnMaxLength, QOverload<int>::of(&QSpinBox::valueChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(cbTable, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
    connect(cbSalt, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Primer3TempCalcWidget::si_settingsChanged);
}

TempCalcSettings Primer3TempCalcWidget::createSettings() const {
    TempCalcSettings settings;
    settings.insert(BaseTempCalc::KEY_ID, id);
    settings.insert(Primer3TempCalc::KEY_DNA_CONC, dsbDna->value());
    settings.insert(Primer3TempCalc::KEY_SALT_CONC, dsbMonovalent->value());
    settings.insert(Primer3TempCalc::KEY_DIVALENT_CONC, dsbDivalent->value());
    settings.insert(Primer3TempCalc::KEY_DNTP_CONC, dsbDntp->value());
    settings.insert(Primer3TempCalc::KEY_DMSO_CONC, dsbDmso->value());
    settings.insert(Primer3TempCalc::KEY_DMSO_FACT, dsbDmsoFactor->value());
    settings.insert(Primer3TempCalc::KEY_FORMAMIDE_CONC, dsbFormamide->value());
    settings.insert(Primer3TempCalc::KEY_MAX_LEN, sbNnMaxLength->value());
    settings.insert(Primer3TempCalc::KEY_TM_METHOD, cbTable->currentIndex());
    settings.insert(Primer3TempCalc::KEY_SALT_CORRECTION, cbSalt->currentIndex());

    return settings;
}

void Primer3TempCalcWidget::restoreFromSettings(const TempCalcSettings& settings) {
    dsbDna->setValue(settings.value(Primer3TempCalc::KEY_DNA_CONC, Primer3TempCalc::DNA_CONC_DEFAULT).toDouble());
    dsbMonovalent->setValue(settings.value(Primer3TempCalc::KEY_SALT_CONC, Primer3TempCalc::SALT_CONC_DEFAULT).toDouble());
    dsbDivalent->setValue(settings.value(Primer3TempCalc::KEY_DIVALENT_CONC, Primer3TempCalc::DIVALENT_CONC_DEFAULT).toDouble());
    dsbDntp->setValue(settings.value(Primer3TempCalc::KEY_DNTP_CONC, Primer3TempCalc::DNTP_CONC_DEFAULT).toDouble());
    dsbDmso->setValue(settings.value(Primer3TempCalc::KEY_DMSO_CONC, Primer3TempCalc::DMSO_CONC_DEFAULT).toDouble());
    dsbDmsoFactor->setValue(settings.value(Primer3TempCalc::KEY_DMSO_FACT, Primer3TempCalc::DMSO_FACT_DEFAULT).toDouble());
    dsbFormamide->setValue(settings.value(Primer3TempCalc::KEY_FORMAMIDE_CONC, Primer3TempCalc::FORMAMIDE_CONC_DEFAULT).toDouble());
    sbNnMaxLength->setValue(settings.value(Primer3TempCalc::KEY_MAX_LEN, Primer3TempCalc::NN_MAX_LEN_DEFAULT).toInt());
    cbTable->setCurrentIndex(settings.value(Primer3TempCalc::KEY_TM_METHOD, Primer3TempCalc::TM_METHOD_DEFAULT).toInt());
    cbSalt->setCurrentIndex(settings.value(Primer3TempCalc::KEY_SALT_CORRECTION, Primer3TempCalc::SALT_CORRECTIONS_DEFAULT).toInt());
}

}
