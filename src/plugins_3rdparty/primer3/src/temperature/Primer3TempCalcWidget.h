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

#include "ui_Primer3TempCalcWidget.h"

#include <U2Algorithm/BaseTempCalc.h>

#include <U2View/BaseTempCalcWidget.h>

namespace U2 {

/**
 * @Primer3TempCalcWidget is a widget to set up settings for @Primer3TempCalc
 */
class Primer3TempCalcWidget : public BaseTempCalcWidget, private Ui_Primer3TempCalcWidget {
    Q_OBJECT
public:
    Primer3TempCalcWidget(QWidget* parent, const QString& id);

    TempCalcSettings createSettings() const override;
    void restoreFromSettings(const TempCalcSettings& settings) override;

};

}
