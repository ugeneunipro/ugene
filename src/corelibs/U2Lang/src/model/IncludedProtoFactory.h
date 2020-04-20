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

#ifndef _SCRIPT_PROTO_MAKER_H_
#define _SCRIPT_PROTO_MAKER_H_

#include <U2Core/global.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/Attribute.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/Schema.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT IncludedProtoFactory {
public:
    virtual ~IncludedProtoFactory() = default;

    static void init(IncludedProtoFactory *protoMaker);
    static ActorPrototype *getScriptProto(QList<DataTypePtr > input, QList<DataTypePtr > output, QList<Attribute*> attrs,
        const QString &name, const QString &description, const QString &actorFilePath, bool isAliasName = false);
    static ActorPrototype *getExternalToolProto(ExternalProcessConfig *cfg);
    static ActorPrototype *getSchemaActorProto(Schema *schema, const QString &name, const QString &actorFilePath);
    static bool registerExternalToolWorker(ExternalProcessConfig *cfg);
    static void registerScriptWorker(const QString &actorName);
    static ExternalProcessConfig* getExternalToolWorker(const QString& id);
    static ExternalProcessConfig *unregisterExternalToolWorker(const QString &id);

    static bool isRegistered(const QString &actorName);
    static bool isRegisteredTheSameProto(const QString &actorName, ActorPrototype *proto);

protected:
    virtual ActorPrototype *_getScriptProto(QList<DataTypePtr > input, QList<DataTypePtr > output, QList<Attribute*> attrs,
        const QString &name,const QString &description, const QString &actorFilePath, bool isAliasName) = 0;
    virtual ActorPrototype *_getExternalToolProto(ExternalProcessConfig *cfg) = 0;
    virtual ActorPrototype *_getSchemaActorProto(Schema *schema, const QString &name, const QString &actorFilePath) = 0;
    virtual bool _registerExternalToolWorker(ExternalProcessConfig *cfg) = 0;
    virtual void _registerScriptWorker(const QString &actorName) = 0;
    virtual ExternalProcessConfig* _getExternalToolWorker(const QString& id) = 0;
    virtual ExternalProcessConfig *_unregisterExternalToolWorker(const QString &id) = 0;

private:
    static IncludedProtoFactory *instance;
};

} // Workflow
} // U2

#endif // _SCRIPT_PROTO_MAKER_H_
