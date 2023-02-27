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

#include "TempCalcWidget.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

#include <U2Algorithm/BaseTempCalc.h>
#include <U2Algorithm/TempCalcRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "BaseTempCalcWidget.h"

namespace U2 {

TempCalcWidget::TempCalcWidget(QWidget* parent)
    : QWidget(parent),
      cbAlgorithm(new QComboBox(this)),
      swSettings(new QStackedWidget(this)) {
    setObjectName("tempCalcWidget");
    cbAlgorithm->setObjectName("cbAlgorithm");
    auto label = new QLabel(tr("Choose temperature calculation algorithm:"), this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(cbAlgorithm);
    layout->addWidget(swSettings);
    auto factories = AppContext::getTempCalcRegistry()->getAllEntries();
    for (auto tempCalcMethodFactory : qAsConst(factories)) {
        auto settingsWidget = tempCalcMethodFactory->createSettingsWidget(this);
        cbAlgorithm->addItem(tempCalcMethodFactory->visualName, tempCalcMethodFactory->getId());
        swSettings->addWidget(settingsWidget);
        settingsWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        connect(settingsWidget, &BaseTempCalcWidget::si_settingsChanged, this, &TempCalcWidget::si_settingsChanged);
    }
    swSettings->currentWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(cbAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged), swSettings, &QStackedWidget::setCurrentIndex);
    connect(cbAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TempCalcWidget::si_settingsChanged);
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

void TempCalcWidget::init(const TempCalcSettings& currentSettings) {
    auto index = cbAlgorithm->findData(currentSettings.value(BaseTempCalc::KEY_ID));
    CHECK(index != -1, );

    cbAlgorithm->setCurrentIndex(index);
    auto baseCalcWidget = qobject_cast<BaseTempCalcWidget*>(swSettings->widget(index));
    SAFE_POINT(baseCalcWidget != nullptr, "Not a BaseTempCalcWidget", );
    baseCalcWidget->restoreFromSettings(currentSettings);
}

TempCalcSettings TempCalcWidget::getSettings() const {
    auto currentWidget = swSettings->widget(cbAlgorithm->currentIndex());
    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), {});

    auto currentBaseTempCalcWidget = qobject_cast<BaseTempCalcWidget*>(currentWidget);
    SAFE_POINT(currentBaseTempCalcWidget != nullptr, L10N::nullPointerError("BaseTempCalcWidget"), {});

    return currentBaseTempCalcWidget->createSettings();
}

}  // namespace U2
