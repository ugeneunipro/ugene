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

#ifndef _U2_WIZARDPAGECONTROLLER_H_
#define _U2_WIZARDPAGECONTROLLER_H_

#include <U2Lang/WizardPage.h>
#include <U2Lang/Schema.h>

#include <U2Designer/WizardController.h>

namespace U2 {

class WidgetController;
class WDWizardPage;

class WizardPageController {
public:
    WizardPageController(WizardController *wc, WizardPage *page);
    virtual ~WizardPageController();

    void setQtPage(WDWizardPage *value);
    WDWizardPage * getQtPage() const;
    WizardPage * getPage() const;
    void applyLayout();
    int nextId() const;

private:
    WDWizardPage *wPage;
    WizardController *wc;
    WizardPage *page;
    QList<WidgetController*> controllers;

private:
    void removeLayout(QLayout *l);
    void setError(WDWizardPage *wPage);
};

} // U2

#endif // _U2_WIZARDPAGECONTROLLER_H_
