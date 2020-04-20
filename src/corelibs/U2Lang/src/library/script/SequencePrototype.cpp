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

#include <U2Core/AppContext.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "ScriptEngineUtils.h"
#include <U2Lang/WorkflowScriptEngine.h>

#include "SequencePrototype.h"

namespace U2 {

const QString SequenceScriptClass::CLASS_NAME("Sequence");

/************************************************************************/
/* SequencePrototype */
/************************************************************************/
SequencePrototype::SequencePrototype(QObject *parent)
: DbiClassPrototype(parent)
{

}

U2SequenceObject * SequencePrototype::getSequenceObject() const {
    CHECK(NULL != thisData(), NULL);
    Workflow::DbiDataStorage *storage = dataStorage();
    CHECK(NULL != storage, NULL);
    return Workflow::StorageUtils::getSequenceObject(storage, thisData()->getId());
}

U2SequenceObject * SequencePrototype::getValidSequenceObject() const {
    U2SequenceObject *result = getSequenceObject();
    SCRIPT_CHECK(NULL != result, context(), "Invalid sequence object", NULL);
    return result;
}

U2Region SequencePrototype::getRegion(int startPosArg, int lengthPosArg) {
    assert(startPosArg < lengthPosArg);

    U2Region r = U2_REGION_MAX;
    if (startPosArg < context()->argumentCount()) {
        r.startPos = context()->argument(startPosArg).toNumber();
        SCRIPT_CHECK(r.startPos >= 0, context(), "Negative start position", U2Region());
        if (lengthPosArg < context()->argumentCount()) {
            r.length = context()->argument(lengthPosArg).toNumber();
        } else {
            r.length -= r.startPos;
        }
        SCRIPT_CHECK(r.length >= 0, context(), "Negative length", U2Region());
    }
    return r;
}

qint64 SequencePrototype::length() {
    QScopedPointer<U2SequenceObject> seqObj(getValidSequenceObject());
    CHECK(!seqObj.isNull(), 0);
    return seqObj->getSequenceLength();
}

QString SequencePrototype::name() {
    QScopedPointer<U2SequenceObject> seqObj(getValidSequenceObject());
    CHECK(!seqObj.isNull(), 0);
    return seqObj->getSequenceName();
}

QString SequencePrototype::string() {
    QScopedPointer<U2SequenceObject> seqObj(getValidSequenceObject());
    CHECK(!seqObj.isNull(), "");

    return seqObj->getSequenceData(getRegion(0, 1));
}

void SequencePrototype::splice() {
    QScopedPointer<U2SequenceObject> seqObj(getValidSequenceObject());
    CHECK(!seqObj.isNull(), );
    if (0 == context()->argumentCount()) {
        return;
    }
    QString data = context()->argument(0).toString();
    DNASequence seq(data.toLatin1());
    U2OpStatus2Log os;
    seqObj->replaceRegion(getRegion(1, 2), seq, os);
    SCRIPT_CHECK(!os.isCoR(), context(), "Can not replace a substring", );
}

/************************************************************************/
/* SequenceScriptClass */
/************************************************************************/
SequenceScriptClass::SequenceScriptClass(QScriptEngine *engine)
: DbiScriptClass(engine)
{
    qScriptRegisterMetaType<ScriptDbiData>(engine, toScriptValue<SequenceScriptClass>, fromScriptValue);
    proto = engine->newQObject(new SequencePrototype(this));
}

QScriptValue SequenceScriptClass::constructor(QScriptContext *ctx, QScriptEngine * /*engine*/) {
    SequenceScriptClass *sClass = qscriptvalue_cast<SequenceScriptClass*>(ctx->callee().data());
    if (!sClass) {
        return QScriptValue();
    }

    if (1 == ctx->argumentCount()) {
        QScriptValue arg = ctx->argument(0);
        if (arg.instanceOf(ctx->callee())) {
            return sClass->newInstance(qscriptvalue_cast<ScriptDbiData>(arg), true /* deepCopy */);
        } else if (arg.isVariant()) {
            QVariant var = arg.toVariant();
            if (var.canConvert<Workflow::SharedDbiDataHandler>()) {
                return sClass->newInstance(var.value<Workflow::SharedDbiDataHandler>());
            }
        }
    } else if (2 == ctx->argumentCount()) {
        QString data = ctx->argument(0).toString();
        QString name = ctx->argument(1).toString();
        return sClass->newInstance(data, name);
    }
    return QScriptValue();
}

Workflow::SharedDbiDataHandler SequenceScriptClass::copySequence(const ScriptDbiData &id, QScriptEngine *engine) {
    Workflow::SharedDbiDataHandler result;
    Workflow::DbiDataStorage *storage = ScriptEngineUtils::dataStorage(engine);
    SCRIPT_CHECK(NULL != storage, engine->currentContext(), "Data storage error", result);

    QScopedPointer<U2SequenceObject> seqObj(Workflow::StorageUtils::getSequenceObject(storage, id.getId()));
    SCRIPT_CHECK(!seqObj.isNull(), engine->currentContext(), "Invalid sequence id", result);

    U2OpStatusImpl os;
    U2Sequence clonedSeq = U2SequenceUtils::copySequence(seqObj->getEntityRef(), seqObj->getEntityRef().dbiRef, U2ObjectDbi::ROOT_FOLDER, os);
    SCRIPT_CHECK(!os.isCoR(), engine->currentContext(), "Can not copy sequence", result);

    U2EntityRef newEnt(seqObj->getEntityRef().dbiRef, clonedSeq.id);
    result = storage->getDataHandler(newEnt);

    return result;
}

QScriptValue SequenceScriptClass::newInstance(const QString &data, const QString &name) {
    Workflow::DbiDataStorage *storage = workflowEngine()->getWorkflowContext()->getDataStorage();
    Workflow::SharedDbiDataHandler id = storage->putSequence(DNASequence(name, data.toLatin1()));
    return newInstance(id);
}

QScriptValue SequenceScriptClass::newInstance(const ScriptDbiData &id, bool deepCopy) {
    if (deepCopy) {
        return newInstance(copySequence(id, engine()));
    } else {
        return newInstance(id.getId());
    }
}

QString SequenceScriptClass::name() const {
    return CLASS_NAME;
}

WorkflowScriptEngine * SequenceScriptClass::workflowEngine() const {
    return ScriptEngineUtils::workflowEngine(engine());
}

} // U2
