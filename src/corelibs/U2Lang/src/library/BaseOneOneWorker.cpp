/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "BaseOneOneWorker.h"

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {
namespace LocalWorkflow {

BaseOneOneWorker::BaseOneOneWorker(Actor* a, bool autoTransitBus, const QString& inPortId, const QString& outPortId)
    : BaseWorker(a, autoTransitBus), inPortId(inPortId), outPortId(outPortId), input(nullptr), output(nullptr), prepared(false) {
}

void BaseOneOneWorker::init() {
    input = ports.value(inPortId);
    output = ports.value(outPortId);
    SAFE_POINT(nullptr != input, QString("Input port '%1' is NULL").arg(inPortId), );
    SAFE_POINT(nullptr != output, QString("Output port '%1' is NULL").arg(outPortId), );
}

Task* BaseOneOneWorker::tick() {
    if (!prepared) {
        U2OpStatusImpl os;
        Task* prepareTask = prepare(os);
        CHECK_OP(os, nullptr);
        if (nullptr != prepareTask) {
            return prepareTask;
        }
    }

    if (input->hasMessage()) {
        Task* tickTask = processNextInputMessage();
        CHECK(nullptr != tickTask, nullptr);
        connect(tickTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
        return tickTask;
    } else if (input->isEnded()) {
        Task* lastTickTask = onInputEnded();
        if (lastTickTask == nullptr) {
            output->setEnded();
            setDone();
        } else {
            connect(lastTickTask, SIGNAL(si_stateChanged()), SLOT(sl_taskFinished()));
            return lastTickTask;
        }
    }
    return nullptr;
}

Task* BaseOneOneWorker::createPrepareTask(U2OpStatus& /*os*/) const {
    return nullptr;
}

void BaseOneOneWorker::onPrepared(Task* /*task*/, U2OpStatus& /*os*/) {
}

void BaseOneOneWorker::sl_taskFinished() {
    auto task = dynamic_cast<Task*>(sender());
    CHECK(nullptr != task, );
    CHECK(task->isFinished() && !task->isCanceled() && !task->hasError(), );
    U2OpStatusImpl os;
    QList<Message> result = fetchResult(task, os);
    CHECK_OP_EXT(os, reportError(os.getError()), );

    foreach (const Message& message, result) {
        if (-1 != message.getMetadataId()) {
            output->setContext(output->getContext(), -1);
        }
        output->put(message);
    }
}

void BaseOneOneWorker::sl_prepared() {
    auto task = dynamic_cast<Task*>(sender());
    CHECK(nullptr != task, );
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

Task* BaseOneOneWorker::prepare(U2OpStatus& os) {
    CHECK(!prepared, nullptr);
    Task* task = createPrepareTask(os);
    if (os.hasError()) {
        reportError(os.getError());
        output->setEnded();
        setDone();
    }
    if (nullptr != task) {
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_prepared()));
    }
    prepared = true;
    return task;
}

}  // namespace LocalWorkflow
}  // namespace U2
