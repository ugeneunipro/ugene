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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/WizardWidgetVisitor.h>

#include "ElementSelectorWidget.h"

namespace U2 {

const QString ElementSelectorWidget::ID("element-selector");

ElementSelectorWidget::ElementSelectorWidget()
: WizardWidget()
{

}

void ElementSelectorWidget::accept(WizardWidgetVisitor *visitor) {
    visitor->visit(this);
}

void ElementSelectorWidget::setActorId(const QString &value) {
    actorId = value;
}

void ElementSelectorWidget::addValue(const SelectorValue &value) {
    values << value;
}

void ElementSelectorWidget::setLabel(const QString &value) {
    label = value;
}

const QString & ElementSelectorWidget::getActorId() const {
    return actorId;
}

const QList<SelectorValue> & ElementSelectorWidget::getValues() const {
    return values;
}

const QString & ElementSelectorWidget::getLabel() const {
    return label;
}

void ElementSelectorWidget::validate(const QList<Actor*> &actors, U2OpStatus &os) const {
    Workflow::Actor *actor = validateActorId(actors, os);
    CHECK_OP(os, );

    QSet<QString> valueSet;
    foreach (const SelectorValue &value, values) {
        validateDuplicates(value, valueSet, os);
        CHECK_OP(os, );
        valueSet << value.getValue();

        value.validate(actor, os);
        CHECK_OP(os, );
    }
}

Workflow::Actor * ElementSelectorWidget::validateActorId(QList<Workflow::Actor*> actors, U2OpStatus &os) const {
    Workflow::Actor *result = NULL;
    foreach (Workflow::Actor *actor, actors) {
        if (actor->getId() == actorId) {
            result = actor;
            break;
        }
    }
    if (NULL == result) {
        os.setError(QObject::tr("The workflow does not contain an element with this id: %1").arg(actorId));
    }
    return result;
}

void ElementSelectorWidget::validateDuplicates(const SelectorValue &value,
    const QSet<QString> &valueSet, U2OpStatus &os) const {
    if (valueSet.contains(value.getValue())) {
        os.setError(QObject::tr("Duplicated selector value: %1").arg(value.getValue()));
    }
}

} // U2
