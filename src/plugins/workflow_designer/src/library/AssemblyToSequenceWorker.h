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

#ifndef _ASSEMBLY_TO_SEQUENCES_WORKER_
#define _ASSEMBLY_TO_SEQUENCES_WORKER_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class AssemblyToSequencesTask;

class AssemblyToSequencesWorker : public BaseWorker {
    Q_OBJECT
public:
    AssemblyToSequencesWorker(Actor *p);
    virtual void init();
    virtual Task *tick();
    virtual void cleanup() {}

private slots:
    void sl_taskFinished();

private:
    AssemblyToSequencesTask *converter;
    IntegralBus *inChannel;
    IntegralBus *outChannel;
}; // AssemblyToSequencesWorker

class AssemblyToSequencesTask : public Task {
    Q_OBJECT
public:
    AssemblyToSequencesTask(const Message &m, const QVariantMap &context, IntegralBus *channel, DbiDataStorage *storage);

    virtual void run();
    virtual void cleanup();

private:
    Message message;
    QVariantMap ctx;
    IntegralBus *channel;
    DbiDataStorage *storage;
};

class AssemblyToSequencesWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    AssemblyToSequencesWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // AssemblyToSequencesWorkerFactory

class AssemblyToSequencesPrompter : public PrompterBase<AssemblyToSequencesPrompter>{
    Q_OBJECT
public:
    AssemblyToSequencesPrompter(Actor *p = NULL) : PrompterBase<AssemblyToSequencesPrompter>(p) {}

protected:
    QString composeRichDoc();
}; // AssemblyToSequencesPrompter

} // LocalWorkflow
} // U2

#endif // _ASSEMBLY_TO_SEQUENCES_WORKER_
