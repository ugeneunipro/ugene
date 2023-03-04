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

#include <U2Algorithm/TmCalculator.h>

#include <U2Lang/ConfigurationEditor.h>

#include <QLineEdit>
#include <QToolButton>

namespace U2 {

class TmCalculatorPropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    TmCalculatorPropertyWidget(QWidget* parent = nullptr, DelegateTags* tags = nullptr);

    QVariant value() override;

public slots:
    void setValue(const QVariant& value) override;

private slots:
    void sl_showDialog();

private:
    /** Updates state of UI based in the current settings. */
    void updateUiState();

    QLineEdit* lineEdit = nullptr;
    QToolButton* toolButton = nullptr;
    QVariantMap tempSettings;
};


}
