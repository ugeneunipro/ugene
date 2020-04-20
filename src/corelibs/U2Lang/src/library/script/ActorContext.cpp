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

#include <U2Lang/IntegralBusType.h>
#include <U2Lang/ScriptEngineUtils.h>

#include "ActorContext.h"

namespace U2 {
namespace LocalWorkflow {

const QString ActorContext::OUTPUT("output");
const QString ActorContext::INPUT("input");
const QString ActorContext::PARAMETERS("params");

QScriptValue ActorContext::createContext(BaseWorker *worker, QScriptEngine *engine) {
    QScriptValue in = engine->newArray();
    QScriptValue out = engine->newArray();
    foreach (const QString &portId, worker->getPorts().keys()) {
        Port *port = worker->getActor()->getPort(portId);
        IntegralBus *bus = worker->getPorts()[portId];
        if (port->isInput()) {
            QScriptValue busArray = createInBus(bus, engine);
            in.setProperty(portId, busArray, QScriptValue::ReadOnly);
        } else {
            QScriptValue busArray = createOutBus(port, engine);
            out.setProperty(portId, busArray, QScriptValue::ReadOnly);
        }
    }

    QScriptValue params = engine->newArray();
    foreach (Attribute *attr, worker->getActor()->getParameters()) {
        QScriptValue value = engine->newVariant(attr->getAttributePureValue());
        params.setProperty(attr->getId(), value, QScriptValue::ReadOnly);
    }

    QScriptValue ctx = engine->newArray();
    ctx.setProperty(INPUT, in, QScriptValue::ReadOnly);
    ctx.setProperty(OUTPUT, out, QScriptValue::ReadOnly);
    ctx.setProperty(PARAMETERS, params, QScriptValue::ReadOnly);
    return ctx;
}

QScriptValue ActorContext::createInBus(IntegralBus *bus, QScriptEngine *engine) {
    QVariantMap busData;
    if (bus->hasMessage()) {
        busData = bus->lookMessage().getData().toMap();
    }
    QScriptValue busArray = engine->newArray();
    QMap<Descriptor, DataTypePtr> types = bus->getBusType()->getDatatypesMap();
    foreach (const Descriptor &slotDesc, types.keys()) {
        QString slotId = slotDesc.getId();
        QScriptValue value = QScriptValue::NullValue;
        if (busData.contains(slotId)) {
            value = ScriptEngineUtils::toScriptValue(engine, busData.value(slotId), types[slotId]);
        }
        busArray.setProperty(slotId, value, QScriptValue::ReadOnly);
    }
    return busArray;
}

QScriptValue ActorContext::createOutBus(Port *port, QScriptEngine *engine) {
    QScriptValue busArray = engine->newArray();
    foreach (const Descriptor &slotDesc, port->getOutputType()->getDatatypesMap().keys()) {
        busArray.setProperty(slotDesc.getId(), QScriptValue::NullValue);
    }
    return busArray;
}

} // LocalWorkflow
} // U2
