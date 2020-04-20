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

#ifndef _GROUP_WORKER_H_
#define _GROUP_WORKER_H_

#include "util/GrouperActionUtils.h"

#include <U2Lang/Datatype.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class GroupPrompter : public PrompterBase<GroupPrompter> {
    Q_OBJECT
public:
    GroupPrompter(Actor *p = NULL) : PrompterBase<GroupPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // GroupPrompter

class GroupWorker : public BaseWorker {
    Q_OBJECT
public:
    GroupWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    IntegralBus *inChannel;
    IntegralBus *outChannel;
    DataTypePtr mtype;

    bool produceOneGroup;
    QString groupSlot;
    QString groupOp;
    QList<GrouperOutSlot> outSlots;
    DataTypePtr inType;

    // TODO: needs to be refactored (extract class)
    QMap<int, PerformersMap> groupedData;
    QMap<int, QVariant> uniqueData;
    QMap<int, int> groupSize;
}; // GroupWorker

class GroupWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    GroupWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // GroupWorkerFactory

} // LocalWorkflow
} // U2

#endif // _GROUP_WORKER_H_
