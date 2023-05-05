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

#include "TmCalculatorPropertyWidget.h"

#include <QLayout>

#include <U2Algorithm/TmCalculatorFactory.h>
#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/TmCalculatorSelectorDialog.h>

namespace U2 {

TmCalculatorPropertyWidget::TmCalculatorPropertyWidget(QWidget* parent, DelegateTags* tags)
    : PropertyWidget(parent, tags) {
    tempSettings = AppContext::getTmCalculatorRegistry()->getDefaultTmCalculatorFactory()->createDefaultSettings();

    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName("TmCalculatorPropertyLineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    lineEdit->setReadOnly(true);

    addMainWidget(lineEdit);

    toolButton = new QToolButton(this);
    toolButton->setObjectName("TmCalculatorPropertyToolButton");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, &QToolButton::clicked, this, &TmCalculatorPropertyWidget::sl_showDialog);
    layout()->addWidget(toolButton);
    updateUiState();
}

void TmCalculatorPropertyWidget::updateUiState() {
    auto factory = AppContext::getTmCalculatorRegistry()->getById(tempSettings.value(TmCalculator::KEY_ID).toString());
    lineEdit->setText(factory->visualName);
}

QVariant TmCalculatorPropertyWidget::value() {
    return QVariant(tempSettings);
}

void TmCalculatorPropertyWidget::setValue(const QVariant& value) {
    CHECK(value.isValid(), );

    auto settingsVariantMap = value.toMap();
    auto algorithmId = settingsVariantMap.value(TmCalculator::KEY_ID).toString();
    auto factory = AppContext::getTmCalculatorRegistry()->getById(algorithmId);
    tempSettings = factory->createDefaultSettings();
    updateUiState();
}

void TmCalculatorPropertyWidget::sl_showDialog() {
    QObjectScopedPointer<TmCalculatorSelectorDialog> dialog(new TmCalculatorSelectorDialog(this, tempSettings));
    int res = dialog->exec();
    CHECK(!dialog.isNull() && res == QDialog::Accepted, );

    tempSettings = dialog->getTemperatureCalculatorSettings();
    updateUiState();
    emit si_valueChanged(value());
}

}  // namespace U2
