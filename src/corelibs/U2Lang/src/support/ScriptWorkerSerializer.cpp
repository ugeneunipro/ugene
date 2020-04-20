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

#include <QDomElement>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/IncludedProtoFactory.h>
#include <U2Lang/WorkflowEnv.h>

#include "ScriptWorkerSerializer.h"

#define WORKFLOW_DOC "GB2WORKFLOW"
#define ACTOR_ELEMENT "Actor"
#define INPUT_PORT_ELEMENT "Input-port"
#define OUTPUT_PORT_ELEMENT "Output-port"
#define ATTRIBUTE_ELEMENT "Attributes"
#define IN_SLOT_ELEMENT "In-Slots"
#define OUT_SLOT_ELEMENT "Out-Slots"
#define SLOT_ID "Slot"
#define ATTR_ELEMENT "Attribute"
#define NAME_ID "Name"
#define TYPE_ID "Type"
#define NAME_ELEMENT "Element-name"
#define DESCR_ELEMENT "Element-description"
#define DESCR_ID "Description"

namespace U2 {

Workflow::ActorPrototype *ScriptWorkerSerializer::string2actor(const QString data, const QString actorName, QString &error, const QString actorFilePath) {
    QDomDocument xml;
    xml.setContent(data,false,&error);
    if(!error.isEmpty()) {
        return NULL;
    }
    QDomElement doc = xml.documentElement();
    DataTypeRegistry *dtr = Workflow::WorkflowEnv::getDataTypeRegistry();
    assert(dtr);

    QDomNodeList inputs = doc.elementsByTagName(IN_SLOT_ELEMENT);
    QList<DataTypePtr> inputTypes;
    for(int i = 0; i < inputs.size(); i++) {
        QDomElement slot = inputs.item(i).toElement();
        QString id = slot.attribute(SLOT_ID);
        inputTypes << dtr->getById(id);
    }

    QDomNodeList outputs = doc.elementsByTagName(OUT_SLOT_ELEMENT);
    QList<DataTypePtr> outputTypes;
    for(int i = 0; i < outputs.size(); i++) {
        QDomElement slot = outputs.item(i).toElement();
        QString id = slot.attribute(SLOT_ID);
        outputTypes << dtr->getById(id);
    }

    QDomNodeList attributes = doc.elementsByTagName(ATTR_ELEMENT);
    QList<Attribute *>attrs;
    for(int i = 0;i < attributes.size(); i++) {
        QDomElement attr = attributes.item(i).toElement();
        QString typeId = attr.attribute(TYPE_ID);
        QString name = attr.attribute(NAME_ID);

        DataTypePtr ptr = dtr->getById(typeId);
        Descriptor desc(name, name, ptr->getDisplayName());
        if(ptr == BaseTypes::BOOL_TYPE()) {
            attrs << new Attribute(desc, ptr, false, QVariant(false));
        }
        else {
            attrs << new Attribute(desc, ptr);
        }
    }

    QString newActorName = actorName;
    if (newActorName.isEmpty()) {
        QDomElement name = doc.elementsByTagName(NAME_ELEMENT).item(0).toElement();
        newActorName = name.attribute(NAME_ID);
    }

    QDomElement descr = doc.elementsByTagName(DESCR_ELEMENT).item(0).toElement();
    QString actorDesc = descr.attribute(DESCR_ID);

    // if actorName is not set then it is not alias name
    bool isAliasName = !actorName.isEmpty();

    Workflow::ActorPrototype *proto = Workflow::IncludedProtoFactory::getScriptProto(inputTypes, outputTypes, attrs, newActorName, actorDesc, actorFilePath, isAliasName);

    if (NULL == proto) {
        error = QObject::tr("UGENE external error. Please, try again");
        return NULL;
    }

    return proto;
}

} // U2
