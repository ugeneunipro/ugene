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

TempCalcSettings* Primer3TempCalcWidget::getSettings() const {
    auto p3Settings = new Primer3TempCalcSettings;
    p3Settings->id = id;
    p3Settings->dnaConc = dsbDna->value();
    p3Settings->saltConc = dsbMonovalent->value();
    p3Settings->divalentConc = dsbDivalent->value();
    p3Settings->dntpConc = dsbDntp->value();
    p3Settings->dmsoConc = dsbDmso->value();
    p3Settings->dmsoFact = dsbDmsoFactor->value();
    p3Settings->formamideConc = dsbFormamide->value();
    p3Settings->nnMaxLen = sbNnMaxLength->value();
    p3Settings->tmMethod = static_cast<Primer3TempCalcSettings::TmMethodType>(cbTable->currentIndex());
    p3Settings->saltCorrections = static_cast<Primer3TempCalcSettings::SaltCorrectionType>(cbSalt->currentIndex());

    return p3Settings;
}

void Primer3TempCalcWidget::restoreFromSettings(TempCalcSettings* settings) {
    auto p3Settings = static_cast<Primer3TempCalcSettings*>(settings);
    SAFE_POINT(p3Settings != nullptr, L10N::nullPointerError("Primer3TempCalcSettings"), );

    dsbDna->setValue(p3Settings->dnaConc);
    dsbMonovalent->setValue(p3Settings->saltConc);
    dsbDivalent->setValue(p3Settings->divalentConc);
    dsbDntp->setValue(p3Settings->dntpConc);
    dsbDmso->setValue(p3Settings->dmsoConc);
    dsbDmsoFactor->setValue(p3Settings->dmsoFact);
    dsbFormamide->setValue(p3Settings->formamideConc);
    sbNnMaxLength->setValue(p3Settings->nnMaxLen);
    cbTable->setCurrentIndex(static_cast<int>(p3Settings->tmMethod));
    cbSalt->setCurrentIndex(static_cast<int>(p3Settings->saltCorrections));
}

}