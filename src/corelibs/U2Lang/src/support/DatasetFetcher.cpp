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
#include <U2Core/U2SafePoints.h>

#include "DatasetFetcher.h"

namespace U2 {
namespace LocalWorkflow {

DatasetFetcher::DatasetFetcher()
: worker(NULL), port(NULL), context(NULL), datasetInitialized(false), fullDataset(false)
{

}

DatasetFetcher::DatasetFetcher(BaseWorker *worker, IntegralBus *port, WorkflowContext *context)
: worker(worker), port(port), context(context), datasetInitialized(false), fullDataset(false)
{

}

bool DatasetFetcher::hasFullDataset() const {
    return fullDataset;
}

bool DatasetFetcher::isDone() const {
    return datasetMessages.isEmpty() && !port->hasMessage() && port->isEnded();
}

const QString &DatasetFetcher::getDatasetName() const {
    return datasetName;
}

QList<Message> DatasetFetcher::takeFullDataset() {
    SAFE_POINT(hasFullDataset(), L10N::internalError("Unexpected method call"), datasetMessages);
    QList<Message> result = datasetMessages;
    cleanup();
    return result;
}

void DatasetFetcher::processInputMessage() {
    if (port->hasMessage() && !hasFullDataset()) {
        if (datasetChanged(port->lookMessage())) {
            fullDataset = true;
            return;
        }
        takeMessage();
    }

    if (!datasetMessages.isEmpty() && !port->hasMessage() && port->isEnded()) {
        fullDataset = true;
    }
}

QString DatasetFetcher::getPortId() const {
    return port->getPortId();
}

QString DatasetFetcher::getDatasetName(const Message &message) const {
    const int metadataId = message.getMetadataId();
    const MessageMetadata metadata = context->getMetadataStorage().get(metadataId);
    return metadata.getDatasetName();
}

bool DatasetFetcher::datasetChanged(const Message &message) const {
    if (!datasetInitialized) {
        return false;
    }
    return (getDatasetName(message) != datasetName);
}

void DatasetFetcher::takeMessage() {
    const Message message = worker->getMessageAndSetupScriptValues(port);
    datasetMessages << message;

    if (!datasetInitialized) {
        datasetInitialized = true;
        datasetName = getDatasetName(message);
    }

    SAFE_POINT(!datasetChanged(message), L10N::internalError("Unexpected method call"), );
}

void DatasetFetcher::cleanup() {
    datasetInitialized = false;
    fullDataset = false;
    datasetMessages.clear();
    datasetName.clear();
}

} //LocalWorkflow
} //U2
