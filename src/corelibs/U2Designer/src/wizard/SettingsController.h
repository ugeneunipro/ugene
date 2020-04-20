/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_SETTINGS_CONTROLLER_H_
#define _U2_SETTINGS_CONTROLLER_H_

#include <U2Designer/WizardController.h>

#include <U2Lang/WizardWidget.h>

#include "WidgetController.h"

namespace U2 {

class SettingsController : public WidgetController {
    Q_OBJECT
public:
    SettingsController(WizardController *wc, SettingsWidget *sw);
    virtual ~SettingsController();

    virtual QWidget * createGUI(U2OpStatus &os);

    static const QString TYPE_URL;
private slots:
    void sl_valueChanged();
    void sl_fileSelect();

private:
    SettingsWidget *sw;
    QLineEdit* lineEdit;

private:
    QWidget* createUrlSettingsWidget(U2OpStatus &os);
    QString getSettingValue();
};

} // U2

#endif // _U2_SETTINGS_CONTROLLER_H_
