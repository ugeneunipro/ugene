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

#include "TmCalculatorSelectorDialog.h"

#include <QDialogButtonBox>
#include <QLayout>

#include <U2Algorithm/TmCalculatorFactory.h>
#include <U2Algorithm/TmCalculatorRegistry.h>

#include <U2Core/AppContext.h>

#include <U2Gui/HelpButton.h>

#include "TmCalculatorSelectorWidget.h"

namespace U2 {

TmCalculatorSelectorDialog::TmCalculatorSelectorDialog(QWidget* parent, const QVariantMap& currentSettings)
    : QDialog(parent) {
    setObjectName("MeltingTemperatureCalculationDialog");
    setWindowTitle(tr("Melting temperature"));
    setLayout(new QVBoxLayout);
    selectorWidget = new TmCalculatorSelectorWidget(this);
    selectorWidget->init(currentSettings);
    layout()->addWidget(selectorWidget);
    auto dbb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    dbb->setObjectName("buttonBox");
    new HelpButton(this, dbb, "88080505");
    connect(dbb, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(dbb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout()->addWidget(dbb);
}

QSharedPointer<TmCalculator> TmCalculatorSelectorDialog::createTemperatureCalculator() const {
    auto settings = selectorWidget->getSettings();
    QString id = settings.value(TmCalculator::KEY_ID).toString();
    return AppContext::getTmCalculatorRegistry()->getById(id)->createCalculator(settings);
}

QVariantMap TmCalculatorSelectorDialog::getTemperatureCalculatorSettings() const {
    return selectorWidget->getSettings();
}

}  // namespace U2
