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

#include "TmCalculatorSelectorWidget.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

#include <U2Algorithm/TmCalculator.h>
#include <U2Algorithm/TmCalculatorFactory.h>
#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "TmCalculatorSettingsWidget.h"

namespace U2 {

TmCalculatorSelectorWidget::TmCalculatorSelectorWidget(QWidget* parent)
    : QWidget(parent),
      cbAlgorithm(new QComboBox(this)),
      swSettings(new QStackedWidget(this)) {
    setObjectName("TmCalculatorSettingsWidget");
    cbAlgorithm->setObjectName("cbAlgorithm");
    auto label = new QLabel(tr("Choose temperature calculation algorithm:"), this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(cbAlgorithm);
    layout->addWidget(swSettings);
    auto factories = AppContext::getTmCalculatorRegistry()->getAllEntries();
    for (auto factory : qAsConst(factories)) {
        auto settingsWidget = factory->createSettingsWidget(this);
        cbAlgorithm->addItem(factory->visualName, factory->getId());
        swSettings->addWidget(settingsWidget);
        settingsWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        connect(settingsWidget, &TmCalculatorSettingsWidget::si_settingsChanged, this, &TmCalculatorSelectorWidget::si_settingsChanged);
    }
    swSettings->currentWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(cbAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged), swSettings, &QStackedWidget::setCurrentIndex);
    connect(cbAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TmCalculatorSelectorWidget::si_settingsChanged);
    connect(swSettings, &QStackedWidget::currentChanged, this, [this](int index) {
        // setSizePolicy() and adjustSize() are required for widget resizing on @settingsWidget widget changed
        for (int i = 0; i < swSettings->count(); i++) {
            CHECK_CONTINUE(i != index);

            swSettings->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        }
        auto currentWidget = swSettings->widget(index);
        SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), );

        currentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        swSettings->adjustSize();
        adjustSize();
        parentWidget()->adjustSize();
    });
}

void TmCalculatorSelectorWidget::init(const QVariantMap& currentSettings) {
    auto index = cbAlgorithm->findData(currentSettings.value(TmCalculator::KEY_ID));
    CHECK(index != -1, );

    cbAlgorithm->setCurrentIndex(index);
    auto settingsWidget = qobject_cast<TmCalculatorSettingsWidget*>(swSettings->widget(index));
    SAFE_POINT(settingsWidget != nullptr, "Not a TmCalculatorSettingsWidget", );
    settingsWidget->restoreFromSettings(currentSettings);
}

QVariantMap TmCalculatorSelectorWidget::getSettings() const {
    auto currentWidget = swSettings->widget(cbAlgorithm->currentIndex());
    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), {});

    auto settingsWidget = qobject_cast<TmCalculatorSettingsWidget*>(currentWidget);
    SAFE_POINT(settingsWidget != nullptr, L10N::nullPointerError("TmCalculatorSettingsWidget"), {});

    return settingsWidget->createSettings();
}

}  // namespace U2
