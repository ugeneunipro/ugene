/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <utils/GTUtilsDialog.h>

namespace U2 {

class GTUtilsWizard {
public:
    enum WizardButton {
        Next,
        Back,
        Apply,
        Run,
        Cancel,
        Defaults,
        Setup,
        Finish
    };

    static void setInputFiles(const QList<QStringList>& list);
    static void setAllParameters(QMap<QString, QVariant> map);
    static void setParameter(const QString& parameterName, const QVariant& parameterValue);
    static QVariant getParameter(const QString& parameterName);
    static void setValue(QWidget* w, QVariant value);
    static void clickButton(WizardButton button);
    static QString getPageTitle();

private:
    static const QMap<QString, WizardButton> buttonMap;
    static QMap<QString, WizardButton> initButtonMap();
};

}  // namespace U2
