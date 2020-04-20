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

#ifndef _U2_WIZARDWIDGETVISITOR_H_
#define _U2_WIZARDWIDGETVISITOR_H_

#include <U2Lang/ElementSelectorWidget.h>
#include <U2Lang/WizardWidget.h>

namespace U2 {

class U2LANG_EXPORT WizardWidgetVisitor {
public:
    virtual void visit(AttributeWidget *) = 0;
    virtual void visit(WidgetsArea *) = 0;
    virtual void visit(GroupWidget *) = 0;
    virtual void visit(LogoWidget *) = 0;
    virtual void visit(ElementSelectorWidget *) = 0;
    virtual void visit(PairedReadsWidget *) = 0;
    virtual void visit(UrlAndDatasetWidget *) = 0;
    virtual void visit(RadioWidget *) = 0;
    virtual void visit(SettingsWidget *) = 0;
    virtual void visit(BowtieWidget *) = 0;
    virtual void visit(TophatSamplesWidget *) = 0;
    virtual void visit(LabelWidget *) = 0;
};

} // U2

#endif // _U2_WIZARDWIDGETVISITOR_H_
