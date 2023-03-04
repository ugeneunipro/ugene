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
#pragma once

#include <QWidget>

#include "ui_Primer3TmCalculatorSettingsWidget.h"

#include <U2Algorithm/TmCalculator.h>

#include <U2View/TmCalculatorSettingsWidget.h>

namespace U2 {

/**
 * @Primer3TmCalculatorSettingsWidget is a widget to set up settings for @Primer3TmCalculator
 */
class Primer3TmCalculatorSettingsWidget : public TmCalculatorSettingsWidget, private Ui_Primer3TmCalculatorSettingsWidget {
    Q_OBJECT
public:
    Primer3TmCalculatorSettingsWidget(QWidget* parent, const QString& id);

    QVariantMap createSettings() const override;
    void restoreFromSettings(const QVariantMap& settings) override;

};

}
