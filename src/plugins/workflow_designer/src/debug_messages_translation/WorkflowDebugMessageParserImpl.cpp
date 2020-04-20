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

#include <QScopedPointer>
#include <QStringList>
#include <QVariantMap>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/ExportObjectUtils.h>

#include <U2Lang/BaseSlots.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowTransport.h>

#include "AnnotationsMessageTranslator.h"
#include "AssemblyMessageTranslator.h"
#include "BaseMessageTranslator.h"
#include "MultipleAlignmentMessageTranslator.h"
#include "SequenceMessageTranslator.h"
#include "VariationTrackMessageTranslator.h"
#include "WorkflowDebugMessageParserImpl.h"

const QString PRODUCING_ACTOR_AND_DATA_TYPE_SEPARATOR = ":";
const QString FILE_NAME_WORDS_SEPARATOR = "_";
const QString INVESTIGATION_FILES_DIR_NAME = "investigation_files";

namespace U2 {

using namespace Workflow;

void WorkflowDebugMessageParserImpl::initParsedInfo() {
    if (Q_LIKELY(!messageTypes.isEmpty())) {
        foreach (const QString &typeName, messageTypes) {
            parsedInfo[typeName] = QQueue<QString>();
        }
    }
}

QString WorkflowDebugMessageParserImpl::convertToString(const QString &contentIdentifier,
    const QVariant &content) const
{
    QScopedPointer<BaseMessageTranslator> messageTranslator(createMessageTranslator(
        getMessageTypeFromIdentifier(contentIdentifier), content));
    SAFE_POINT(!messageTranslator.isNull(), "Invalid message translator detected!", QString());
    const QString result = messageTranslator->getTranslation();
    return result;
}

QString WorkflowDebugMessageParserImpl::getMessageTypeFromIdentifier(
    const QString &messageIdentifier) const
{
    return messageIdentifier.right(messageIdentifier.size() - messageIdentifier.lastIndexOf(
        PRODUCING_ACTOR_AND_DATA_TYPE_SEPARATOR) - 1);
}

WorkflowInvestigationData WorkflowDebugMessageParserImpl::getAllMessageValues() {
    if (!sourceMessages.isEmpty()) {
        foreach (const QString &key, sourceMessages.head().keys()) {
            const QString messageType = getMessageTypeFromIdentifier(key);
            if (Q_UNLIKELY(!possibleMessageTypes.contains(messageType))) {
                coreLog.info(QObject::tr("Messages in requested queue include info of the '%1' "
                    "data type that is currently unsupported for view. "
                    "No intermediate data will be displayed").arg(messageType));
                return parsedInfo;
            }
            if(Q_UNLIKELY(!messageTypes.contains(key))) {
                messageTypes << key;
            }
        }
        initParsedInfo();
        foreach (const QVariantMap &messageContent, sourceMessages) {
            foreach (const QString &key, messageContent.keys()) {
                SAFE_POINT(messageTypes.contains(key), "Unexpected message type encountered!",
                    parsedInfo);
                parsedInfo[key].enqueue(convertToString(key, messageContent[key]));
            }
        }
    }
    return parsedInfo;
}

void WorkflowDebugMessageParserImpl::convertMessagesToDocuments(const QString &convertedType, const QString &schemeName, quint32 messageNumber) {
    SAFE_POINT(!convertedType.isEmpty(), "Invalid message type detected!",);
    const AppSettings *appSettings = AppContext::getAppSettings();
    SAFE_POINT(NULL != appSettings, "Invalid application settings' storage!",);
    const UserAppsSettings *userSettings = appSettings->getUserAppsSettings();
    SAFE_POINT(NULL != userSettings, "Invalid user application settings' storage!",);
    QString tmpFolderUrl = (userSettings->getCurrentProcessTemporaryDirPath());
    tmpFolderUrl.replace("//", "/");

    quint32 messageCounter = ++messageNumber;
    foreach(const QVariantMap &mapData, sourceMessages) {
        SAFE_POINT(mapData.keys().contains(convertedType), "Invalid message type detected!",);
        const QString messageType = getMessageTypeFromIdentifier(convertedType);
        const QString baseFileUrl = tmpFolderUrl + "/" + schemeName + FILE_NAME_WORDS_SEPARATOR + messageType + FILE_NAME_WORDS_SEPARATOR
            + "m" + QString::number(messageCounter);
        if (BaseSlots::ANNOTATION_TABLE_SLOT().getId() == messageType) {
            const QVariant annotationsData = mapData[convertedType];
            const QList<SharedAnnotationData> annList = StorageUtils::getAnnotationTable(context->getDataStorage(), annotationsData);

            AnnotationTableObject *annsObj = new AnnotationTableObject("Annotations", context->getDataStorage()->getDbiRef());
            annsObj->addAnnotations(annList);

            ExportObjectUtils::exportAnnotations(annsObj, baseFileUrl);
        } else {
            GObject *objectToWrite = fetchObjectFromMessage(messageType, mapData[convertedType]);
            if(Q_LIKELY(NULL != objectToWrite)) {
                ExportObjectUtils::exportObject2Document(objectToWrite, baseFileUrl, false);
                ++messageCounter;
            }
        }
    }
}

BaseMessageTranslator *WorkflowDebugMessageParserImpl::createMessageTranslator(const QString &messageType, const QVariant &messageData) const {
    BaseMessageTranslator *result = NULL;
    if (BaseSlots::DNA_SEQUENCE_SLOT().getId() == messageType) {
        result = new SequenceMessageTranslator(messageData, context);
    } else if (BaseSlots::ANNOTATION_TABLE_SLOT().getId() == messageType) {
        result = new AnnotationsMessageTranslator(messageData, context);
    } else if (BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId() == messageType) {
        result = new MultipleAlignmentMessageTranslator(messageData, context);
    } else if (BaseSlots::ASSEMBLY_SLOT().getId() == messageType) {
        result = new AssemblyMessageTranslator(messageData, context);
    } else if (BaseSlots::VARIATION_TRACK_SLOT().getId() == messageType) {
        result = new VariationTrackMessageTranslator(messageData, context);
    } else if (BaseSlots::TEXT_SLOT().getId() == messageType || BaseSlots::URL_SLOT().getId()
        == messageType || BaseSlots::DATASET_SLOT().getId() == messageType
        || BaseSlots::FASTA_HEADER_SLOT().getId() == messageType)
    {
        result = new BaseMessageTranslator(messageData, context);
    } else {
        FAIL("Unable to determine message type", result);
    }
    return result;
}

GObject *WorkflowDebugMessageParserImpl::fetchObjectFromMessage(const QString &messageType, const QVariant &messageData) const {
    GObject *result = NULL;
    if (BaseSlots::TEXT_SLOT().getId() == messageType) {
        SAFE_POINT(messageData.canConvert<QString>(), "Supplied message doesn't contain text data",
            NULL);
        const QString documentText = messageData.value<QString>();
        U2OpStatus2Log os;
        result = TextObject::createInstance(documentText, "wd_investigation_tmp_text_object", context->getDataStorage()->getDbiRef(), os);
        return result;
    } else if (BaseSlots::URL_SLOT().getId() == messageType
        || BaseSlots::DATASET_SLOT().getId() == messageType
        || BaseSlots::FASTA_HEADER_SLOT().getId() == messageType
        || BaseSlots::ANNOTATION_TABLE_SLOT().getId() == messageType)
    {
        return result;
    }
    SAFE_POINT(messageData.canConvert<SharedDbiDataHandler>(),
        "Supplied message doesn't contain DB reference", NULL);
    SharedDbiDataHandler objectId = messageData.value<SharedDbiDataHandler>();

    if (BaseSlots::DNA_SEQUENCE_SLOT().getId() == messageType) {
        result = StorageUtils::getSequenceObject(context->getDataStorage(), objectId);
    } else if (BaseSlots::MULTIPLE_ALIGNMENT_SLOT().getId() == messageType) {
        result = StorageUtils::getMsaObject(context->getDataStorage(), objectId);
    } else if(BaseSlots::ASSEMBLY_SLOT().getId() == messageType) {
        result = StorageUtils::getAssemblyObject(context->getDataStorage(), objectId);
    } else if (BaseSlots::VARIATION_TRACK_SLOT().getId() == messageType) {
        result = StorageUtils::getVariantTrackObject(context->getDataStorage(), objectId);
    }
    SAFE_POINT(NULL != result, "Could not obtain object from dbi", NULL);
    return result;
}

} // namespace U2
