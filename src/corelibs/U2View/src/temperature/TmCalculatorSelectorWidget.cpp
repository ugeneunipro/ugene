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

#include <U2View/DNAStatisticsTask.h>

#include "TmCalculatorSettingsWidget.h"

namespace U2 {

TmCalculatorSelectorWidget::TmCalculatorSelectorWidget(QWidget* parent, bool showLimitsHint)
    : QWidget(parent) {
    setObjectName("TmCalculatorSettingsWidget");
    swSettings = new QStackedWidget(this);

    cbAlgorithm = new QComboBox(this);
    cbAlgorithm->setObjectName("cbAlgorithm");

    QLabel* lengthHintLabel = new QLabel(tr("Hint: UGENE computes Tm for sequence regions from %1 up to %2 bp")
                                             .arg(DNAStatisticsTask::TM_MIN_LENGTH_LIMIT)
                                             .arg(DNAStatisticsTask::TM_MAX_LENGTH_LIMIT));
    lengthHintLabel->setStyleSheet("QLabel{font-size: 12px; padding-top: 5px; padding-bottom: 5px; color: #333333;}");
    lengthHintLabel->setVisible(showLimitsHint);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Choose temperature calculation algorithm:"), this));
    layout->addWidget(cbAlgorithm);
    layout->addWidget(swSettings);
    layout->addWidget(lengthHintLabel);
    auto factories = AppContext::getTmCalculatorRegistry()->getAllEntries();
    for (auto factory : qAsConst(factories)) {
        auto settingsWidget = factory->createSettingsWidget(this);
        cbAlgorithm->addItem(factory->visualName, factory->getId());
        swSettings->addWidget(settingsWidget);
        connect(settingsWidget, &TmCalculatorSettingsWidget::si_settingsChanged, this, &TmCalculatorSelectorWidget::si_settingsChanged);
    }

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
        currentWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
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
