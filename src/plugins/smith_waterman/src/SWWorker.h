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

#ifndef _U2_SW_WORKER_H_
#define _U2_SW_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Algorithm/SmithWatermanSettings.h>
#include <U2Algorithm/SmithWatermanTaskFactory.h>
#include <U2Algorithm/SmithWatermanReportCallback.h>

namespace U2 {
namespace LocalWorkflow {

class SWPrompter : public PrompterBase<SWPrompter> {
    Q_OBJECT
public:
    SWPrompter(Actor* p = 0) : PrompterBase<SWPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class SWAlgoEditor : public ComboBoxDelegate {
    Q_OBJECT
public:
    SWAlgoEditor(ActorPrototype* proto) : ComboBoxDelegate(QVariantMap()), proto(proto) {}
public slots:
    void populate();
private:
    ActorPrototype* proto;
};

class SWWorker : public BaseWorker {
    Q_OBJECT
public:
    SWWorker(Actor* a);

    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished(Task*);

private:
    IntegralBus *input, *patternPort, *output;
    QMap<Task*, SmithWatermanReportCallbackAnnotImpl*> callbacks;
    QList<QByteArray> patternList;
    QMap<Task*, QByteArray> patterns;
    QMap<QString, QString> patternNames;
};

class SWWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    SWWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new SWWorker(a);}
};

} //namespace LocalWorkflow
} // U2 namespace

#endif
