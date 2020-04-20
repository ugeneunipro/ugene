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

#include <U2Lang/BaseTypes.h>

#include "SequencePrototype.h"
#include <U2Lang/WorkflowScriptEngine.h>

#include "ScriptEngineUtils.h"

namespace U2 {

WorkflowScriptEngine * ScriptEngineUtils::workflowEngine(QScriptEngine *engine) {
    return dynamic_cast<WorkflowScriptEngine*>(engine);
}

DbiDataStorage * ScriptEngineUtils::dataStorage(QScriptEngine *engine) {
    WorkflowScriptEngine *we = workflowEngine(engine);
    CHECK(NULL != we, NULL);
    WorkflowContext *wc = we->getWorkflowContext();
    CHECK(NULL != wc, NULL);
    return wc->getDataStorage();
}

SequenceScriptClass * ScriptEngineUtils::getSequenceClass(QScriptEngine *engine) {
    QScriptValue cls = engine->globalObject().property(SequenceScriptClass::CLASS_NAME);
    CHECK(!cls.isNull(), NULL);
    return qscriptvalue_cast<SequenceScriptClass*>(cls.data());
}

SharedDbiDataHandler ScriptEngineUtils::getDbiId(QScriptEngine *engine, const QScriptValue &value, const QString &className) {
    QScriptValue sClass = engine->globalObject().property(className);

    QScriptValue idValue = value;
    if (value.instanceOf(sClass)) {
        QScriptValue getIdFun = value.property("getId");
        if (getIdFun.isFunction()) {
            idValue = getIdFun.call(value);
        }
    }

    if (idValue.isVariant()) {
        QVariant var = idValue.toVariant();
        if (var.canConvert<SharedDbiDataHandler>()) {
            return var.value<SharedDbiDataHandler>();
        }
    }
    return SharedDbiDataHandler();
}

SharedDbiDataHandler ScriptEngineUtils::getDbiId(QScriptEngine * /*engine*/, const QScriptValue &value) {
    if (value.isVariant()) {
        QVariant var = value.toVariant();
        if (var.canConvert<SharedDbiDataHandler>()) {
            return var.value<SharedDbiDataHandler>();
        }
    }
    return SharedDbiDataHandler();
}

QScriptValue ScriptEngineUtils::toScriptValue(QScriptEngine *engine, const QVariant &value, DataTypePtr type) {
    if (BaseTypes::DNA_SEQUENCE_TYPE() == type) {
        SequenceScriptClass *sClass = getSequenceClass(engine);
        CHECK(NULL != sClass, engine->newVariant(value));

        if (value.canConvert<SharedDbiDataHandler>()) {
            SharedDbiDataHandler seqId = value.value<SharedDbiDataHandler>();
            return sClass->newInstance(seqId, false);
        }
    }
    return engine->newVariant(value);
}

QVariant ScriptEngineUtils::fromScriptValue(QScriptEngine *engine, const QScriptValue &value, DataTypePtr type) {
    if (BaseTypes::DNA_SEQUENCE_TYPE() == type) {
        SharedDbiDataHandler seqId = ScriptEngineUtils::getDbiId(engine, value,
            SequenceScriptClass::CLASS_NAME);
        return qVariantFromValue(seqId);
    }
    return value.toVariant();
}

} // U2
