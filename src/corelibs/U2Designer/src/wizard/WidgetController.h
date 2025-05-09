/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QLabel>
#include <QWidget>

#include <U2Core/U2OpStatus.h>

#include <U2Lang/ConfigurationEditor.h>

namespace U2 {

class WizardController;

class WidgetController : public QObject {
public:
    WidgetController(WizardController* wc);
    virtual ~WidgetController();

    virtual QWidget* createGUI(U2OpStatus& os) = 0;

protected:
    WizardController* wc;
};

class LabeledPropertyWidget : public QWidget {
public:
    LabeledPropertyWidget(const QString& labelText, PropertyWidget* widget, QWidget* parent = nullptr);

    void setLabelWidth(int width);

private:
    QLabel* label;
};

}  // namespace U2
