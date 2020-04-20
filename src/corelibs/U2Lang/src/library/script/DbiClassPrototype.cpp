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

#include "ScriptEngineUtils.h"
#include <U2Lang/WorkflowScriptEngine.h>

#include "DbiClassPrototype.h"

namespace U2 {

/************************************************************************/
/* ScriptDbiData */
/************************************************************************/
ScriptDbiData::ScriptDbiData() {

}

ScriptDbiData::~ScriptDbiData() {

}

ScriptDbiData::ScriptDbiData(const Workflow::SharedDbiDataHandler &_seqId)
: id(_seqId)
{

}

const Workflow::SharedDbiDataHandler & ScriptDbiData::getId() const {
    return id;
}

void ScriptDbiData::release() {
    id = Workflow::SharedDbiDataHandler();
}

/************************************************************************/
/* DbiClassPrototype */
/************************************************************************/
DbiClassPrototype::DbiClassPrototype(QObject *parent)
: QObject(parent)
{

}

DbiClassPrototype::~DbiClassPrototype() {

}

QScriptValue DbiClassPrototype::getId() {
    CHECK(NULL != thisData(), QScriptValue::NullValue);
    Workflow::SharedDbiDataHandler id = thisData()->getId();
    SCRIPT_CHECK(NULL != id.constData(), context(), "Invalid data id. Was it released?", QScriptValue::NullValue);

    return engine()->newVariant(qVariantFromValue(id));
}

void DbiClassPrototype::release() {
    CHECK(NULL != thisData(), );
    thisData()->release();
}

ScriptDbiData * DbiClassPrototype::thisData() const {
    ScriptDbiData *result = qscriptvalue_cast<ScriptDbiData*>(thisObject().data());
    SCRIPT_CHECK(NULL != result, context(), "No this object", NULL);
    return result;
}

WorkflowScriptEngine * DbiClassPrototype::workflowEngine() const {
    return ScriptEngineUtils::workflowEngine(engine());
}

Workflow::DbiDataStorage * DbiClassPrototype::dataStorage() const {
    WorkflowScriptEngine *we = workflowEngine();
    CHECK(NULL != we, NULL);
    Workflow::WorkflowContext *wc = we->getWorkflowContext();
    CHECK(NULL != wc, NULL);
    return wc->getDataStorage();
}

/************************************************************************/
/* DbiScriptClass */
/************************************************************************/
DbiScriptClass::DbiScriptClass(QScriptEngine *engine)
: QObject(engine), QScriptClass(engine)
{

}

QScriptValue DbiScriptClass::prototype() const {
    return proto;
}

QScriptValue DbiScriptClass::newInstance(const Workflow::SharedDbiDataHandler &id) {
    ScriptDbiData dbiData(id);
    QScriptValue data = engine()->newVariant(qVariantFromValue(dbiData));
    return engine()->newObject(this, data);
}

void DbiScriptClass::fromScriptValue(const QScriptValue &obj, ScriptDbiData &id) {
    id = qvariant_cast<ScriptDbiData>(obj.data().toVariant());
}

} // U2
