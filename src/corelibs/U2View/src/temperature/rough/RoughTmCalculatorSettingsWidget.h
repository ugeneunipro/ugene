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

#include <U2View/TmCalculatorSettingsWidget.h>

namespace U2 {

/**
 * @RoughTmCalculatorSettingsWidget is a widget to set up settings for @RoughTmCalculator
 */
class RoughTmCalculatorSettingsWidget : public TmCalculatorSettingsWidget {
    Q_OBJECT
public:
    RoughTmCalculatorSettingsWidget(QWidget* parent, const QString& id);

    QVariantMap createSettings() const override;
    void restoreFromSettings(const QVariantMap& settings) override;
};

}  // namespace U2
