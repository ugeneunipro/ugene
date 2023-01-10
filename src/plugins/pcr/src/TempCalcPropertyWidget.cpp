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

#include "TempCalcPropertyWidget.h"

#include <U2Algorithm/TempCalcRegistry.h>

#include <U2Core/AppContext.h>

#include <U2View/TempCalcDialog.h>

#include <QLayout>

namespace U2 {

TempCalcPropertyWidget::TempCalcPropertyWidget(QWidget* parent, DelegateTags* tags) 
    : PropertyWidget(parent, tags) {
    lineEdit = new QLineEdit(this);
    tempSettings = AppContext::getTempCalcRegistry()->getDefaultTempCalcSettings();
    lineEdit->setText(tempSettings->id);
    lineEdit->setPlaceholderText(tempSettings->id);
    lineEdit->setObjectName("tempCalcPropertyLineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setReadOnly(true);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("tempCalcPropertyToolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, &QToolButton::clicked, this, &TempCalcPropertyWidget::sl_showDialog);
    layout()->addWidget(toolButton);
}

TempCalcPropertyWidget::~TempCalcPropertyWidget() {
    delete tempSettings;
}

QVariant TempCalcPropertyWidget::value() {
    return QVariant(tempSettings->toVariantMap());
}

void TempCalcPropertyWidget::setValue(const QVariant& value) {
    CHECK(value.isValid(), );
    
    auto settingsVariantMap = value.toMap();
    auto settingsId = settingsVariantMap.value(TempCalcSettings::KEY_ID).toString();
    lineEdit->setText(settingsId);
    auto newSettigns = AppContext::getTempCalcRegistry()->getById(settingsId)->createDefaultTempCalcSettings();
    newSettigns->fromVariantMap(settingsVariantMap);
    delete tempSettings;
    tempSettings = newSettigns;
}

void TempCalcPropertyWidget::sl_showDialog() {
    auto dialog = new TempCalcDialog(this, tempSettings);
    dialog->open();
    connect(dialog, &QDialog::finished, this, [this](int result) {
        auto senderDialog = qobject_cast<TempCalcDialog*>(sender());
        if (result == QDialog::DialogCode::Accepted) {
            delete tempSettings;
            tempSettings = senderDialog->getTemperatureCalculatorSettings();
            lineEdit->setText(tempSettings->id);
            emit si_valueChanged(value());
        }
        senderDialog->deleteLater();
    });

}

}