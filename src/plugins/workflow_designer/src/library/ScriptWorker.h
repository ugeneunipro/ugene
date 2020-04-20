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

#ifndef _U2_SCRIPT_WORKER_H_
#define _U2_SCRIPT_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/WorkflowScriptEngine.h>

namespace U2 {

namespace LocalWorkflow {

class ScriptWorkerTask: public Task {
    Q_OBJECT
public:
    ScriptWorkerTask(WorkflowScriptEngine *engine, AttributeScript *script);
    void run();
    QVariant getResult() const;
    WorkflowScriptEngine * getEngine();

private:
    QVariant result;
    WorkflowScriptEngine *engine;
    AttributeScript *script;
};

class ScriptPromter : public PrompterBase<ScriptPromter> {
    Q_OBJECT
public:
    ScriptPromter(Actor * p = 0) : PrompterBase<ScriptPromter>(p) {}
protected:
    QString composeRichDoc();
};

class ScriptWorker: public BaseWorker {
    Q_OBJECT
public:
    ScriptWorker(Actor *a);
    virtual void init();
    virtual Task * tick();
    virtual void cleanup();
    virtual void setDone();

private slots:
    void sl_taskFinished();

private:
    void bindAttributeVariables();
    void bindPortVariables();

    CommunicationChannel *input;
    CommunicationChannel *output;
    WorkflowScriptEngine *engine;
    AttributeScript *script;
    bool taskFinished;

private:
    bool isNeedToBeDone() const;
    bool isNeedToBeRun() const;
};

class ScriptWorkerFactory: public DomainFactory {
public:
    ScriptWorkerFactory(QString name) : DomainFactory(name) {}

    virtual Worker * createWorker(Actor *a);

    static bool init(QList<DataTypePtr> input, QList<DataTypePtr> output, QList<Attribute*> attrs,
        const QString &name,const QString &description, const QString &actorFilePath);

public:
    static const QString ACTOR_ID;
};


} // LocalWorkflow
} // U2

#endif
