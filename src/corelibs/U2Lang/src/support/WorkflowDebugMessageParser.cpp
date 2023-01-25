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

#include "WorkflowDebugMessageParser.h"

#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/Descriptor.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowTransport.h>

const QString MESSAGE_PATH_DELIMETER = ">";

namespace U2 {

using namespace Workflow;

QStringList WorkflowDebugMessageParser::possibleMessageTypes = QStringList();

WorkflowDebugMessageParser::WorkflowDebugMessageParser()
    : context(nullptr) {
}

WorkflowDebugMessageParser::~WorkflowDebugMessageParser() {
}

void WorkflowDebugMessageParser::setSourceData(const QQueue<Message>& initSource) {
    sourceMessages.clear();
    if (Q_UNLIKELY(possibleMessageTypes.isEmpty())) {
        possibleMessageTypes << BaseSlots::DNA_SEQUENCE_SLOT().getId();
        possibleMessageTypes << BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId();
        possibleMessageTypes << BaseSlots::ANNOTATION_TABLE_SLOT().getId();
        possibleMessageTypes << BaseSlots::TEXT_SLOT().getId();
        possibleMessageTypes << BaseSlots::URL_SLOT().getId();
        possibleMessageTypes << BaseSlots::FASTA_HEADER_SLOT().getId();
        possibleMessageTypes << BaseSlots::VARIATION_TRACK_SLOT().getId();
        possibleMessageTypes << BaseSlots::ASSEMBLY_SLOT().getId();
        possibleMessageTypes << BaseSlots::DATASET_SLOT().getId();
    }
    for (const Message& message : qAsConst(initSource)) {
        QVariantMap data = message.getData().toMap();
        foreach (const QString& key, data.keys()) {
            if (key.contains(MESSAGE_PATH_DELIMETER)) {
                coreLog.trace(QString("WorkflowDebugMessageParser.cpp:%1 'key' = %2").arg(QString::number(__LINE__)).arg(key));
                data[key.left(key.indexOf(MESSAGE_PATH_DELIMETER))] = data[key];
                data.remove(key);
            }
        }
        sourceMessages.enqueue(data);
    }
}

void WorkflowDebugMessageParser::setContext(Workflow::WorkflowContext* initContext) {
    SAFE_POINT(nullptr != initContext, "Invalid workflow context!", );
    context = initContext;
}

}  // namespace U2
