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

#include "BaseTempCalcWidget.h"

#include <U2Algorithm/BaseTempCalc.h>
#include <U2Algorithm/TempCalcRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QDialog>
#include <QDialogButtonBox>

namespace U2 {

TempCalcWidget::TempCalcWidget(QWidget* parent, TempCalcSettings* currentSettings) :
    QWidget(parent) {
    setupUi(this);
    
    const auto& factories = AppContext::getTempCalcRegistry()->getAllEntries();
    TempCalcFactory* restoreItem = nullptr;
    BaseTempCalcWidget* restoreWidget = nullptr;
    for (auto tempCalcMethodFactory : qAsConst(factories)) {
        const auto& id = tempCalcMethodFactory->getId();
        auto settingsWidget = tempCalcMethodFactory->createTempCalcSettingsWidget(this, id);
        cbAlgorithm->addItem(id);
        swSettings->addWidget(settingsWidget);
        if (id == currentSettings->id) {
            restoreItem = tempCalcMethodFactory;
            restoreWidget = settingsWidget;
        }
    }

    SAFE_POINT(restoreItem != nullptr, QString("Can't find %1").arg(currentSettings->id), );
    cbAlgorithm->setCurrentIndex(factories.indexOf(restoreItem));
    restoreWidget->restoreFromSettings(currentSettings);
}

TempCalcSettings* TempCalcWidget::getSettings() const {
    auto currentWidget = swSettings->widget(cbAlgorithm->currentIndex());
    SAFE_POINT(currentWidget != nullptr, L10N::nullPointerError("QWidget"), nullptr);

    auto curentBaseTempCalcWidget = qobject_cast<BaseTempCalcWidget*>(currentWidget);
    SAFE_POINT(curentBaseTempCalcWidget != nullptr, L10N::nullPointerError("BaseTempCalcWidget"), nullptr);

    return curentBaseTempCalcWidget->getSettings();
}

}