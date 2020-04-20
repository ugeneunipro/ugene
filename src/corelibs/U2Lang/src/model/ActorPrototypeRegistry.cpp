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

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace Workflow {
bool ActorPrototypeRegistry::registerProto(const Descriptor& group, ActorPrototype* proto) {
    // debug check for proto name
    const QString id = proto->getId();
    assert(WorkflowEntityValidator::ACCEPTABLE_ID.match(id).isValid());
    ActorPrototype *existingProto = getProto(id);
    if (nullptr != existingProto) {
        coreLog.error(tr("Can't register element config with ID '%1'%2. There is already registered element with this ID%3.")
                      .arg(id)
                      .arg(proto->getFilePath().isEmpty() ? QString() : " (" + proto->getFilePath() + ")")
                      .arg(existingProto->getFilePath().isEmpty() ? QString() : " (" + existingProto->getFilePath() + ")"));
        return false;
    }

    groups[group].append(proto);
    emit si_registryModified();
    return true;
}

ActorPrototype * ActorPrototypeRegistry::unregisterProto(const QString &id) {
    foreach(const Descriptor &desc, groups.keys()) {
        QList<ActorPrototype*> &l = groups[desc];
        foreach (ActorPrototype *p, l) {
            if (p->getId() == id) {
                l.removeAll(p);
                if (l.isEmpty()) {
                    groups.remove(desc);
                }
                emit si_registryModified();
                return p;
            }
        }
    }
    return NULL;
}

ActorPrototype* ActorPrototypeRegistry::getProto(const QString& id) const {
    foreach(QList<ActorPrototype*> l, groups.values()) {
        foreach(ActorPrototype* p, l) {
            if (p->getId() == id) {
                return p;
            }
        }
    }
    return NULL;
}

ActorPrototypeRegistry::~ActorPrototypeRegistry()
{
    foreach(QList<ActorPrototype*> l, groups) {
        qDeleteAll(l);
    }
    groups.clear();
}
}//namespace Workflow
}//namespace U2
