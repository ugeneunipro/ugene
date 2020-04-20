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
#include <U2Lang/WorkflowEnv.h>

#include "IncludedProtoFactory.h"

namespace U2 {
namespace Workflow {

IncludedProtoFactory *IncludedProtoFactory::instance = NULL;

void IncludedProtoFactory::init(IncludedProtoFactory *protoMaker) {
    instance = protoMaker;
}

ActorPrototype *IncludedProtoFactory::getScriptProto(QList<DataTypePtr > input, QList<DataTypePtr > output, QList<Attribute*> attrs,
                                                     const QString &name, const QString &description, const QString &actorFilePath, bool isAliasName) {
    if (NULL != instance) {
        return instance->_getScriptProto(input, output, attrs, name, description, actorFilePath, isAliasName);
    } else {
        return NULL;
    }
}

ActorPrototype *IncludedProtoFactory::getExternalToolProto(ExternalProcessConfig *cfg) {
    if (NULL != instance) {
        return instance->_getExternalToolProto(cfg);
    } else {
        return NULL;
    }
}

ActorPrototype *IncludedProtoFactory::getSchemaActorProto(Schema *schema, const QString &name, const QString &actorFilePath) {
    if (NULL != instance) {
        return instance->_getSchemaActorProto(schema, name, actorFilePath);
    } else {
        return NULL;
    }
}

bool IncludedProtoFactory::registerExternalToolWorker(ExternalProcessConfig *cfg) {
    if (NULL != instance) {
        return instance->_registerExternalToolWorker(cfg);
    } else {
        return false;
    }
}

void IncludedProtoFactory::registerScriptWorker(const QString &actorName) {
    if (NULL != instance) {
        return instance->_registerScriptWorker(actorName);
    } else {
        return;
    }
}

ExternalProcessConfig* IncludedProtoFactory::getExternalToolWorker(const QString& id) {
    if (nullptr != instance) {
        return instance->_getExternalToolWorker(id);
    } else {
        return nullptr;
    }
}

ExternalProcessConfig *IncludedProtoFactory::unregisterExternalToolWorker(const QString &id) {
    if (nullptr != instance) {
        return instance->_unregisterExternalToolWorker(id);
    } else {
        return nullptr;
    }
}

bool IncludedProtoFactory::isRegistered(const QString &actorName) {
    ActorPrototype *proto = WorkflowEnv::getProtoRegistry()->getProto(actorName);

    if (NULL == proto) {
        return false;
    } else {
        return true;
    }
}

bool IncludedProtoFactory::isRegisteredTheSameProto(const QString &actorId, ActorPrototype *proto) {
    ActorPrototype *regProto = WorkflowEnv::getProtoRegistry()->getProto(actorId);
    assert(NULL != proto);

    // compare simple proto parameters
    if (regProto->isScriptFlagSet() != proto->isScriptFlagSet()) {
        return false;
    }

    // compare attributes
    {
        QList<Attribute*> attrList = proto->getAttributes();
        QList<Attribute*> regAttrList = regProto->getAttributes();
        if (attrList.size() != regAttrList.size()) {
            return false;
        }
        foreach (Attribute *attr, attrList) {
            bool found = false;
            foreach (Attribute *regAttr, regAttrList) {
                if (*attr == *regAttr) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
    }

    // compare ports
    {
        QList<PortDescriptor*> portList = proto->getPortDesciptors();
        QList<PortDescriptor*> regPortList = regProto->getPortDesciptors();
        if (portList.size() != regPortList.size()) {
            return false;
        }
        foreach (PortDescriptor *port, portList) {
            bool found = false;
            foreach (PortDescriptor *regPort, regPortList) {
                if (*port == *regPort) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
    }

    // everything is compared, protos are equal
    return true;
}

} // Workflow
} // U2
