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

#ifndef _U2_ELEMENTSELECTORWIDGET_H_
#define _U2_ELEMENTSELECTORWIDGET_H_

#include <U2Lang/Schema.h>
#include <U2Lang/WizardWidget.h>

#include "SelectorValue.h"

namespace U2 {

class U2LANG_EXPORT ElementSelectorWidget : public WizardWidget {
public:
    ElementSelectorWidget();

    virtual void accept(WizardWidgetVisitor *visitor);
    virtual void validate(const QList<Actor*> &actors, U2OpStatus &os) const;

    void setActorId(const QString &value);
    void addValue(const SelectorValue &value);
    void setLabel(const QString &value);

    const QString & getActorId() const;
    const QList<SelectorValue> & getValues() const;
    const QString & getLabel() const;

    static const QString ID;

private:
    QString actorId;
    QString label;
    QList<SelectorValue> values;

    /** Returns found actor or NULL */
    Workflow::Actor * validateActorId(QList<Workflow::Actor*> actors, U2OpStatus &os) const;
    void validateDuplicates(const SelectorValue &value,
        const QSet<QString> &valueSet, U2OpStatus &os) const;
};

} // U2

#endif // _U2_ELEMENTSELECTORWIDGET_H_
