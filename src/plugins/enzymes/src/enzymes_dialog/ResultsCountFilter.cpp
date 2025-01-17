/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "ResultsCountFilter.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include "EnzymesIO.h"

namespace U2 {

ResultsCountFilter::ResultsCountFilter(QWidget* parent)
    : QWidget(parent) {

    setupUi(this);

    minHitSB->setMinimum(MIN_HIT_DEFAULT_VALUE);
    minHitSB->setMaximum(MAX_HIT_DEFAULT_VALUE);

    maxHitSB->setMinimum(MAX_HIT_DEFAULT_VALUE - 1);
    maxHitSB->setMaximum(INT_MAX);

    connect(minHitSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int newMinValue) {
        maxHitSB->setMinimum(newMinValue);
    });
    connect(maxHitSB, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int newMaxValue) {
        minHitSB->setMaximum(newMaxValue);
    });

    initSettings();
}

void ResultsCountFilter::saveSettings() {
    auto settings = AppContext::getSettings();
    settings->setValue(EnzymeSettings::ENABLE_HIT_COUNT, filterGroupBox->isChecked());
    if (filterGroupBox->isChecked()) {
        settings->setValue(EnzymeSettings::MIN_HIT_VALUE, minHitSB->value());
        settings->setValue(EnzymeSettings::MAX_HIT_VALUE, maxHitSB->value());
    } else {
        settings->setValue(EnzymeSettings::MIN_HIT_VALUE, MIN_HIT_DEFAULT_VALUE);
        settings->setValue(EnzymeSettings::MAX_HIT_VALUE, INT_MAX);
    }
}

void ResultsCountFilter::initSettings() {
    auto settings = AppContext::getSettings();
    bool useHitCountControl = settings->getValue(EnzymeSettings::ENABLE_HIT_COUNT, false).toBool();
    int minHitValue = settings->getValue(EnzymeSettings::MIN_HIT_VALUE, MIN_HIT_DEFAULT_VALUE).toInt();
    int maxHitValue = settings->getValue(EnzymeSettings::MAX_HIT_VALUE, MAX_HIT_DEFAULT_VALUE).toInt();
    filterGroupBox->setChecked(useHitCountControl);
    if (useHitCountControl) {
        minHitSB->setValue(minHitValue);
        maxHitSB->setValue(maxHitValue);
    } else {
        minHitSB->setValue(MIN_HIT_DEFAULT_VALUE);
        maxHitSB->setValue(MAX_HIT_DEFAULT_VALUE);
    }
}

}  // namespace U2
