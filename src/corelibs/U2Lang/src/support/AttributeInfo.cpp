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

#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowUtils.h>

#include "AttributeInfo.h"

namespace U2 {

const QString AttributeInfo::TYPE("type");
const QString AttributeInfo::DEFAULT("default");
const QString AttributeInfo::DATASETS("datasets");
const QString AttributeInfo::LABEL("label");

AttributeInfo::AttributeInfo(const QString &_actorId, const QString &_attrId, const QVariantMap &_hints)
: actorId(_actorId), attrId(_attrId), hints(_hints)
{

}

void AttributeInfo::validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const {
    Workflow::Actor *actor = WorkflowUtils::actorById(actors, actorId);
    if (NULL == actor) {
        os.setError(QObject::tr("Actor is not found, id: %1").arg(actorId));
        return;
    }
    if (!actor->hasParameter(attrId)) {
        os.setError(QObject::tr("Actor '%1' does not have this parameter: %2").arg(actorId).arg(attrId));
        return;
    }
}

bool AttributeInfo::operator== (const AttributeInfo &other) const {
    return toString() == other.toString();
}

QString AttributeInfo::toString() const {
    return actorId + ":" + attrId;
}

AttributeInfo AttributeInfo::fromString(const QString &value, U2OpStatus &os) {
    QStringList tokens = value.split(":");
    if (2 != tokens.size()) {
        os.setError("Bad attribute value: " + value);
        return AttributeInfo("", "");
    }
    return AttributeInfo(tokens[0], tokens[1]);
}

} // U2
