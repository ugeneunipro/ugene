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

#include <U2Algorithm/TmCalculator.h>

#include <U2Core/global.h>

namespace U2 {

/**
 * @TmCalculatorSettingsWidget class is a widget to set up settings for temperature calculations
 * User can set up all settings manually using this widget
 */
class U2VIEW_EXPORT TmCalculatorSettingsWidget : public QWidget {
    Q_OBJECT
public:
    TmCalculatorSettingsWidget(QWidget* parent, const QString& id);

    /**
     * Get settings, which were set up.
     * Use this function when settings up is done.
     * @return the settings object
     */
    virtual QVariantMap createSettings() const = 0;
    /**
     * Restore widget from the settings object.
     * @settings the settings object.
     */
    virtual void restoreFromSettings(const QVariantMap& settings) = 0;

signals:
    void si_settingsChanged();

protected:
    QString id;


};

}
