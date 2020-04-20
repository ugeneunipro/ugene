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

#ifndef _U2_GT_UTILS_WIZARD_H_
#define _U2_GT_UTILS_WIZARD_H_

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

    static void setInputFiles(HI::GUITestOpStatus &os, const QList<QStringList> &list);
    static void setAllParameters(HI::GUITestOpStatus &os, QMap<QString, QVariant> map);
    static void setParameter(HI::GUITestOpStatus &os, QString parName, QVariant parValue);
    static QVariant getParameter(HI::GUITestOpStatus &os, QString parName);
    static void setValue(HI::GUITestOpStatus &os, QWidget* w, QVariant value);
    static void clickButton(HI::GUITestOpStatus &os, WizardButton button);
    static QString getPageTitle(HI::GUITestOpStatus &os);

private:
    static const QMap<QString, WizardButton> buttonMap;
    static QMap<QString, WizardButton> initButtonMap();
};

}   // namespace U2

#endif // _U2_GT_UTILS_WIZARD_H_
