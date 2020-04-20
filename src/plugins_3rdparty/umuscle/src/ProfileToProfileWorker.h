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

#ifndef _U2_PROFILE_TO_PROFILE_WORKER_H_
#define _U2_PROFILE_TO_PROFILE_WORKER_H_

#include <U2Core/MultipleSequenceAlignmentObject.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

class MultipleSequenceAlignmentObject;

namespace LocalWorkflow {

class ProfileToProfileWorker : public BaseWorker {
    Q_OBJECT
public:
    ProfileToProfileWorker(Actor *a);

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished();

private:
    IntegralBus *inPort;
    IntegralBus *outPort;

    QList<MultipleSequenceAlignmentObject*> objects;
};

class ProfileToProfileWorkerFactory : public DomainFactory {
public:
    ProfileToProfileWorkerFactory() : DomainFactory(ACTOR_ID) {}

    static void init();
    virtual Worker * createWorker(Actor *a);

private:
    static const QString ACTOR_ID;
};

class ProfileToProfilePrompter : public PrompterBase<ProfileToProfilePrompter> {
    Q_OBJECT
public:
    ProfileToProfilePrompter(Actor* p = 0) : PrompterBase<ProfileToProfilePrompter>(p) {}

protected:
    QString composeRichDoc();
};

class ProfileToProfileTask : public Task {
    Q_OBJECT
public:
    ProfileToProfileTask(const MultipleSequenceAlignment &masterMsa, const MultipleSequenceAlignment &secondMsa);
    ~ProfileToProfileTask();

    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task *subTask);

    const MultipleSequenceAlignment & getResult();

private:
    MultipleSequenceAlignment masterMsa;
    MultipleSequenceAlignment secondMsa;
    MultipleSequenceAlignment result;
    int seqIdx;
    int subtaskCount;

private:
    void appendResult(Task *task);
    QList<Task*> createAlignTasks();
    bool canCreateTask() const;
};

} // LocalWorkflow
} // U2

#endif // _U2_PROFILE_TO_PROFILE_WORKER_H_
