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

#include <QScriptEngineDebugger>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FailTask.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorContext.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/BaseActorCategories.h>
#include <U2Lang/BaseSlots.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/CoreLibConstants.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/IntegralBusType.h>
#include <U2Lang/ScriptEngineUtils.h>
#include <U2Lang/ScriptLibrary.h>
#include <U2Lang/SequencePrototype.h>
#include <U2Lang/WorkflowEnv.h>

#include "ScriptWorker.h"

namespace U2 {
namespace LocalWorkflow {

const QString ScriptWorkerFactory::ACTOR_ID("Script-");

const static QString INPUT_PORT_TYPE("input-for-");
const static QString OUTPUT_PORT_TYPE("output-for-");

static const QString IN_PORT_ID("in");
static const QString OUT_PORT_ID("out");

ScriptWorkerTask::ScriptWorkerTask(WorkflowScriptEngine *_engine, AttributeScript *_script)
    : Task(tr("Script worker task"), AppContext::isGUIMode() ? TaskFlag_RunInMainThread
    : TaskFlag_None), engine(_engine), script(_script)
{
    WorkflowScriptLibrary::initEngine(engine);
}

void ScriptWorkerTask::run() {
    QMap<QString, QScriptValue> scriptVars;
    foreach(const Descriptor &key, script->getScriptVars().uniqueKeys()) {
        assert(!key.getId().isEmpty());
        if(!(script->getScriptVars().value(key)).isNull()) {
            scriptVars[key.getId()] = engine->newVariant(script->getScriptVars().value(key));
        } else {
            scriptVars[key.getId()] = engine->newVariant(engine->globalObject().property(key.getId().toLatin1().data()).toVariant());
        }
    }
    QScriptValue scriptResultValue = ScriptTask::runScript(engine, scriptVars, script->getScriptText(), stateInfo);
    if (engine->hasUncaughtException()) {
        scriptResultValue = engine->uncaughtException();
        QString message = scriptResultValue.toString();
        stateInfo.setError(tr("Error in line ") + QString::number(engine->uncaughtExceptionLineNumber()) + ":" + message.split(":").last());
    }
    result = scriptResultValue.toVariant();

    if( stateInfo.cancelFlag ) {
        if(!stateInfo.hasError()) {
            stateInfo.setError("Script task canceled");
        }
    }
}

QVariant ScriptWorkerTask::getResult() const {
    return result;
}

WorkflowScriptEngine * ScriptWorkerTask::getEngine() {
    return engine;
}

QString ScriptPromter::composeRichDoc() {
    return target->getProto()->getDocumentation();
}

bool ScriptWorkerFactory::init(QList<DataTypePtr> input,
                               QList<DataTypePtr> output,
                               QList<Attribute*> attrs,
                               const QString &name,
                               const QString &description,
                               const QString &actorFilePath) {
    ActorPrototype *proto = IncludedProtoFactory::getScriptProto(
        input, output, attrs, name, description, actorFilePath);
    WorkflowEnv::getProtoRegistry()->registerProto(BaseActorCategories::CATEGORY_SCRIPT(), proto);
    IncludedProtoFactory::registerScriptWorker(ACTOR_ID + name);
    return true;
}

Worker * ScriptWorkerFactory::createWorker(Actor *a) {
    return new ScriptWorker(a);
}

ScriptWorker::ScriptWorker(Actor *a)
: BaseWorker(a), input(NULL), output(NULL), taskFinished(false)
{
    script = a->getScript();
    engine = NULL;
}

void ScriptWorker::init() {
    input = ports.value(IN_PORT_ID);
    output = ports.value(OUT_PORT_ID);
    engine = new WorkflowScriptEngine(context);
    if ( AppContext::isGUIMode( ) ) { // add script debugger
        engine->setProcessEventsInterval(50);
        QScriptEngineDebugger *scriptDebugger = new QScriptEngineDebugger(engine);
        scriptDebugger->setAutoShowStandardWindow(true);
        scriptDebugger->attachTo(engine);
    }
}

void ScriptWorker::bindPortVariables() {
    foreach( IntegralBus * bus, ports.values() ) {
        assert(bus != NULL);
        if(actor->getPort(bus->getPortId())->isOutput()) { // means that it is bus for output port
            continue;
        }

        QVariantMap busData = bus->lookMessage().getData().toMap();
        foreach(const QString & slotId, busData.keys()) {
            QString attrId = "in_" + slotId;
            if( script->hasVarWithId(attrId)) {
                script->setVarValueWithId(attrId, busData.value(slotId));
            }
        }
    }
}

void ScriptWorker::bindAttributeVariables() {
    QMap<QString, Attribute*> attrs = actor->getParameters();
    QMap<QString, Attribute*>::iterator it;
    for(it = attrs.begin(); it!=attrs.end();it++) {
        Attribute *attr = it.value();
        if(script->hasVarWithId(attr->getId())) {
            script->setVarValueWithId(attr->getId(), attr->getAttributePureValue());
        }
    }
}

bool ScriptWorker::isNeedToBeDone() const {
    bool result = false;
    if (actor->getInputPorts().isEmpty()) {
        result = taskFinished;
    } else {
        bool hasNotEnded = false;
        foreach (Port *port, actor->getInputPorts()) {
            IntegralBus *input = ports[port->getId()];
            SAFE_POINT(NULL != input, "NULL input bus", false);
            if (!input->isEnded()) {
                hasNotEnded = true;
                break;
            }
        }
        result = !hasNotEnded;
    }
    return result;
}

bool ScriptWorker::isNeedToBeRun() const {
    bool result = true;
    if (actor->getInputPorts().isEmpty()) {
        result = !taskFinished;
    } else {
        foreach (Port *port, actor->getInputPorts()) {
            IntegralBus *input = ports[port->getId()];
            SAFE_POINT(NULL != input, "NULL input bus", false);
            if (!input->hasMessage()) {
                result = false;
                break;
            }
        }
    }
    return result;
}

void ScriptWorker::setDone() {
    BaseWorker::setDone();
    foreach (Port *port, actor->getOutputPorts()) {
        IntegralBus *output = ports[port->getId()];
        SAFE_POINT(NULL != output, "NULL output bus", );
        output->setEnded();
    }
}

Task * ScriptWorker::tick() {
    if(script->isEmpty()) {
        coreLog.error(tr("no script text"));
        return new FailTask(tr("no script text"));
    }

    if (isNeedToBeRun()) {
        //WorkflowScriptLibrary::initEngine(engine);
        //engine->globalObject().setProperty("ctx", ActorContext::createContext(this, engine), QScriptValue::ReadOnly);
        bindPortVariables();
        bindAttributeVariables();
        foreach (Port *port, actor->getInputPorts()) {
            getMessageAndSetupScriptValues(ports[port->getId()]);
        }
        Task *t = new ScriptWorkerTask(engine, script);
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return t;
    } else if (isNeedToBeDone()) {
        setDone();
    }
    return NULL;
}

void ScriptWorker::sl_taskFinished() {
    taskFinished = true;
    ScriptWorkerTask *t = qobject_cast<ScriptWorkerTask*>(sender());
    if (t->getState() != Task::State_Finished || t->hasError() || t->isCanceled()) {
        return;
    }

    QString name = actor->getProto()->getDisplayName();
    DataTypeRegistry *dtr = WorkflowEnv::getDataTypeRegistry();
    assert(dtr);
    DataTypePtr ptr = dtr->getById(OUTPUT_PORT_TYPE + name);

    if(ptr->getAllDescriptors().size() == 1 && ptr->getAllDescriptors().first().getId() == BaseTypes::MULTIPLE_ALIGNMENT_TYPE()->getId()) {
        if (input != NULL && !input->isEnded()) {
            return;
        }
    }

    QVariantMap map;
    bool hasSeqArray = false;
    foreach(const Descriptor &desc, ptr->getAllDescriptors()) {
        QString varName = "out_" + desc.getId();
        QScriptValue value = t->getEngine()->globalObject().property(varName.toLatin1().data());
        if (BaseSlots::DNA_SEQUENCE_SLOT().getId() == desc.getId()) {
            if (value.isArray()) {
                hasSeqArray = true;
                continue;
            }
            SharedDbiDataHandler seqId = ScriptEngineUtils::getDbiId(t->getEngine(), value,
                SequenceScriptClass::CLASS_NAME);
            if (!seqId.constData() || !seqId.constData()->isValid()) {
                continue;
            }
            map[desc.getId()] = qVariantFromValue(seqId);
        } else {
            map[desc.getId()] = value.toVariant();
        }
    }
    if (output) {
        if (hasSeqArray) {
            QString varName = "out_" + BaseSlots::DNA_SEQUENCE_SLOT().getId();
            QScriptValue value = t->getEngine()->globalObject().property(varName.toLatin1().data());
            for (int i=0; i<value.property("length").toInt32(); i++) {
                SharedDbiDataHandler seqId = ScriptEngineUtils::getDbiId(t->getEngine(), value.property(i),
                    SequenceScriptClass::CLASS_NAME);
                if (seqId.constData() && seqId.constData()->isValid()) {
                    map[BaseSlots::DNA_SEQUENCE_SLOT().getId()] = qVariantFromValue(seqId);
                    output->put(Message(ptr,map));
                }
            }
        } else {
            QVariant scriptResult = t->getResult();
            if (!map.isEmpty()) {
                output->put(Message(ptr,map));
            }
        }
    }
}

void ScriptWorker::cleanup() {
    delete engine;
}

} // LocalWorkflow
} // U2
