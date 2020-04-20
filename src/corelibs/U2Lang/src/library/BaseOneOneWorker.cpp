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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BaseOneOneWorker.h"

namespace U2 {
namespace LocalWorkflow {

BaseOneOneWorker::BaseOneOneWorker(Actor *a, bool autoTransitBus, const QString &inPortId, const QString &outPortId)
: BaseWorker(a, autoTransitBus), inPortId(inPortId), outPortId(outPortId), input(NULL), output(NULL), prepared(false)
{

}

void BaseOneOneWorker::init() {
    input = ports.value(inPortId);
    output = ports.value(outPortId);
    SAFE_POINT(NULL != input, QString("Input port '%1' is NULL").arg(inPortId), );
    SAFE_POINT(NULL != output, QString("Output port '%1' is NULL").arg(outPortId), );
}

Task * BaseOneOneWorker::tick() {
    if (!prepared) {
        U2OpStatusImpl os;
        Task *prepareTask = prepare(os);
        CHECK_OP(os, NULL);
        if (NULL != prepareTask) {
            return prepareTask;
        }
    }

    if (input->hasMessage()) {
        Task *tickTask = processNextInputMessage();
        CHECK(NULL != tickTask, NULL);
        connect(tickTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return tickTask;
    } else if (input->isEnded()) {
        Task *lastTickTask = onInputEnded();
        if (NULL == lastTickTask) {
            output->setEnded();
            setDone();
        } else {
            connect(lastTickTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
            return lastTickTask;
        }
    }
    return NULL;
}

Task * BaseOneOneWorker::createPrepareTask(U2OpStatus &/*os*/) const {
    return NULL;
}

void BaseOneOneWorker::onPrepared(Task * /*task*/, U2OpStatus &/*os*/) {

}

void BaseOneOneWorker::sl_taskFinished() {
    Task *task = dynamic_cast<Task*>(sender());
    CHECK(NULL != task, );
    CHECK(task->isFinished() && !task->isCanceled() && !task->hasError(), );
    U2OpStatusImpl os;
    QList<Message> result = fetchResult(task, os);
    CHECK_OP_EXT(os, reportError(os.getError()), );

    foreach (const Message &message, result) {
        if (-1 != message.getMetadataId()) {
            output->setContext(output->getContext(), -1);
        }
        output->put(message);
    }
}

void BaseOneOneWorker::sl_prepared() {
    Task *task = dynamic_cast<Task*>(sender());
    CHECK(NULL != task, );
    CHECK(task->isFinished(), );
    if (task->isCanceled() || task->hasError()) {
        output->setEnded();
        setDone();
        return;
    }

    U2OpStatusImpl os;
    onPrepared(task, os);
    if (os.hasError()) {
        reportError(os.getError());
        output->setEnded();
        setDone();
    }
}

Task * BaseOneOneWorker::prepare(U2OpStatus &os) {
    CHECK(!prepared, NULL);
    Task *task = createPrepareTask(os);
    if (os.hasError()) {
        reportError(os.getError());
        output->setEnded();
        setDone();
    }
    if (NULL != task) {
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_prepared()));
    }
    prepared = true;
    return task;
}

} // LocalWorkflow
} // U2
