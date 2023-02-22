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
#include <QComboBox>
#include <QStackedWidget>

#include <U2Algorithm/BaseTempCalc.h>

#include <U2Core/global.h>

namespace U2 {

/**
 * @TempCalcWidget class is a Widget wich contains all @BaseTempCalcWidget's
 * Has combo box to switch settings and stacked widget, 
 * wich contains all @BaseTempCalcWidget's
 */
class U2VIEW_EXPORT TempCalcWidget : public QWidget {
    Q_OBJECT
public:
    TempCalcWidget(QWidget* parent);

    /**
     * Initialize widget with @currentSettings
     * @currentSettings settings to initialize widget
     */
    void init(const TempCalcSettings& currentSettings);
    /**
     * Get sequence object to the set settings
     * Settings object will be got from the active BaseTempCalcWidget
     * @return settings object
     */
    TempCalcSettings getSettings() const;

signals:
    void si_settingsChanged();

private:
    QComboBox* cbAlgorithm = nullptr;
    QStackedWidget* swSettings = nullptr;

};

}
