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

#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "BaseDatasetWorker.h"

namespace U2 {
namespace LocalWorkflow {

BaseDatasetWorker::BaseDatasetWorker(Actor *a, const QString &inPortId, const QString &outPortId)
: BaseOneOneWorker(a, /* autoTransitBus= */false, inPortId, outPortId), datasetInited(false)
{

}

void BaseDatasetWorker::init() {
    BaseOneOneWorker::init();
    datasetInited = false;
}

void BaseDatasetWorker::cleanup() {
    datasetMessages.clear();
}

Task * BaseDatasetWorker::processNextInputMessage() {
    if (datasetChanged(input->lookMessage())) {
        return onDatasetChanged();
    }
    takeMessage();
    return NULL;
}

Task * BaseDatasetWorker::onInputEnded() {
    if (!datasetMessages.isEmpty()) {
        return onDatasetChanged();
    }
    return NULL;
}

QList<Message> BaseDatasetWorker::fetchResult(Task *task, U2OpStatus &os) {
    QVariantMap data = getResult(task, os);
    const MessageMetadata metadata = generateMetadata(datasetName);
    context->getMetadataStorage().put(metadata);

    QList<Message> result;
    result << Message(output->getBusType(), data, metadata.getId());
    return result;
}

MessageMetadata BaseDatasetWorker::generateMetadata(const QString &datasetName) const {
    return MessageMetadata(datasetName);
}

QString BaseDatasetWorker::getDatasetName(const Message &message) const {
    const int metadataId = message.getMetadataId();
    const MessageMetadata metadata = context->getMetadataStorage().get(metadataId);
    return metadata.getDatasetName();
}

bool BaseDatasetWorker::datasetChanged(const Message &message) const {
    CHECK(datasetInited, false);
    return (getDatasetName(message) != datasetName);
}

void BaseDatasetWorker::takeMessage() {
    const Message message = getMessageAndSetupScriptValues(input);
    datasetMessages << message;

    if (!datasetInited) {
        datasetInited = true;
        datasetName = getDatasetName(message);
    }

    SAFE_POINT(!datasetChanged(message), L10N::internalError("Unexpected method call"), );
}

Task * BaseDatasetWorker::onDatasetChanged() {
    datasetInited = false;
    Task *task = createTask(datasetMessages);
    datasetMessages.clear();
    return task;
}

} // LocalWorkflow
} // U2
