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

#include <QDialog>
#include <QSharedPointer>

#include <U2Algorithm/BaseTempCalc.h>

#include <U2Core/global.h>

namespace U2 {

class TempCalcWidget;

/**
 * @TempCalcDialog class is a dialog wrapper around @TempCalcWidget
 * Use this class if you need to use this widget in a separate dialog
 */
class U2VIEW_EXPORT TempCalcDialog : public QDialog {
    Q_OBJECT
public:
    TempCalcDialog(QWidget* parent, const TempCalcSettings& currentSettings);

    /**
     * Create temperature calculator according to settings from @tempCalcWidget
     * @return pointed to the created temperature calculator
     */
    QSharedPointer<BaseTempCalc> createTemperatureCalculator() const;

    /**
     * Get settings from the inner widget
     * @return the temperature calculator settings
     */
    TempCalcSettings getTemperatureCalculatorSettings() const;

private:
    TempCalcWidget* tempCalcWidget = nullptr;
};

}
