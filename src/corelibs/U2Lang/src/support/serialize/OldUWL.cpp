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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/MarkerAttribute.h>

#include "Constants.h"
#include "HRSchemaSerializer.h"
#include "OldUWL.h"
#include "Utils.h"

namespace U2 {
namespace WorkflowSerialize {

/************************************************************************/
/* OldConstants */
/************************************************************************/
const QString OldConstants::MARKER_START("@");

/************************************************************************/
/* OldUWL */
/************************************************************************/
namespace {
    const QString MARKER_TYPE("marker-type");
    const QString MARKER_NAME("marker-name");
    const QString MARKERS("markers");

    void parseOldMarker(Actor *proc, ParsedPairs &pairs) {
        MarkerAttribute *markerAttr = dynamic_cast<MarkerAttribute*>(proc->getParameter(Constants::MARKER));
        if (NULL == markerAttr) {
            throw ReadFailed(QObject::tr("%1 actor has not markers attribute").arg(proc->getId()));
        }

        Marker *marker = HRSchemaSerializer::parseMarker(pairs, MARKER_TYPE, MARKER_NAME);
        SAFE_POINT(NULL != marker, "NULL marker", );
        if (markerAttr->contains(marker->getName())) {
            throw ReadFailed(QObject::tr("Redefinition of %1 marker at %2 actor").arg(marker->getName()).arg(proc->getId()));
        }

        markerAttr->getMarkers() << marker;

        assert(1 == proc->getOutputPorts().size());
        Port *outPort = proc->getOutputPorts().at(0);
        assert(outPort->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();
        Descriptor newSlot = MarkerSlots::getSlotByMarkerType(marker->getType(), marker->getName());
        outTypeMap[newSlot] = BaseTypes::STRING_TYPE();
        DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
        outPort->setNewType(newType);
    }

    bool isOldMarkerActor(Actor *actor) {
        const QMap<QString, Attribute*> attrs = actor->getParameters();
        CHECK(1 == attrs.size(), false);
        MarkerAttribute *attr = dynamic_cast<MarkerAttribute*>(*attrs.begin());
        return (NULL != attr);
    }
}

void OldUWL::parseMarkerDefinition(Tokenizer &tokenizer, QMap<QString, Actor*> &actorMap) {
    QString name = tokenizer.take();
    QString actorName = HRSchemaSerializer::parseAt(name, 0);
    QString markerId = HRSchemaSerializer::parseAfter(name, 0);
    if (!actorMap.contains(actorName)) {
        throw ReadFailed(QObject::tr("Unknown actor name \"%1\" at a marker definition").arg(actorName));
    }
    tokenizer.assertToken(Constants::BLOCK_START);

    ParsedPairs pairs(tokenizer);
    QString functionType = pairs.equalPairs.take(Constants::TYPE_ATTR);
    if (Constants::MARKER != functionType) {
        throw ReadFailed(QObject::tr("Unknown function type \"%1\" for the marker %2").arg(functionType).arg(markerId));
    }

    parseOldMarker(actorMap[actorName], pairs);
}

void OldUWL::parseOldAttributes(Actor *proc, ParsedPairs &pairs) {
    if (isOldMarkerActor(proc)) {
        pairs.equalPairs.take(MARKERS);
    }
}

} // WorkflowSerialize
} // U2
