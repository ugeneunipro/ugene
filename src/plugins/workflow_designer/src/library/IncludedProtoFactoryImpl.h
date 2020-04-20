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

#ifndef _SCRIPT_PROTO_MAKER_IMPL_H_
#define _SCRIPT_PROTO_MAKER_IMPL_H_

#include <U2Lang/IncludedProtoFactory.h>

namespace U2 {
namespace Workflow {

class IncludedProtoFactoryImpl : public IncludedProtoFactory {
public:
    virtual ActorPrototype *_getScriptProto(QList<DataTypePtr > input, QList<DataTypePtr > output, QList<Attribute*> attrs,
        const QString &name,const QString &description, const QString &actorFilePath, bool isAliasName);
    virtual ActorPrototype *_getExternalToolProto(ExternalProcessConfig *cfg);
    virtual ActorPrototype *_getSchemaActorProto(Schema *schema, const QString &name, const QString &actorFilePath);

    virtual bool _registerExternalToolWorker(ExternalProcessConfig *cfg);
    virtual void _registerScriptWorker(const QString &actorName);

    virtual ExternalProcessConfig* _getExternalToolWorker(const QString& id) override;
    virtual ExternalProcessConfig *_unregisterExternalToolWorker(const QString &id);

private:
    static Descriptor generateUniqueSlotDescriptor( const QList<Descriptor> &existingSlots,
        const DataConfig &dcfg );
};

} // Workflow
} // U2

#endif // _SCRIPT_PROTO_MAKER_IMPL_H_
