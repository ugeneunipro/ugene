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

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/SelectorValue.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>

#include "SelectorActors.h"

namespace U2 {

SelectorActors::SelectorActors() {
    widget = NULL;
    srcActor = NULL;
}

SelectorActors::SelectorActors(ElementSelectorWidget *_widget, const QList<Actor*> &allActors, U2OpStatus &os)
: widget(_widget)
{
    srcActor = WorkflowUtils::actorById(allActors, widget->getActorId());
    if (NULL == srcActor) {
        os.setError(QObject::tr("Unknown actor id: %1").arg(widget->getActorId()));
        return;
    }

    foreach (const SelectorValue &value, widget->getValues()) {
        Actor *a = NULL;
        if (value.getProtoId() == srcActor->getProto()->getId()) {
            a = srcActor;
        } else {
            ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(value.getProtoId());
            a = proto->createInstance(widget->getActorId());
        }
        actors[value.getValue()] = a;
    }
}

SelectorActors::~SelectorActors() {

}

Actor * SelectorActors::getActor(const QString &value) const {
    return actors.value(value, NULL);
}

Actor * SelectorActors::getSourceActor() const {
    return srcActor;
}

QList<PortMapping> SelectorActors::getMappings(const QString &value) const {
    foreach (const SelectorValue &sv, widget->getValues()) {
        if (sv.getValue() == value) {
            return sv.getMappings();
        }
    }
    return QList<PortMapping>();
}

} // U2
