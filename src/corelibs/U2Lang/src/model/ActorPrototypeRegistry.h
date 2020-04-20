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

#ifndef _U2_WORKFLOW_REG_H_
#define _U2_WORKFLOW_REG_H_

#include <U2Lang/ActorModel.h>

namespace U2 {
namespace Workflow {

/**
 * registry of actor prototypes
 * register proto to one of groups - computational categories
 *
 * Base categories are listed in BioActorLibrary
 */
class U2LANG_EXPORT ActorPrototypeRegistry: public QObject {
    Q_OBJECT
public:
    virtual ~ActorPrototypeRegistry();

    // standard registry
    bool registerProto(const Descriptor& group, ActorPrototype* proto);
    ActorPrototype* unregisterProto(const QString& id);
    const QMap<Descriptor, QList<ActorPrototype*> >& getProtos() const {return groups;}
    ActorPrototype* getProto(const QString& id) const;

signals:
    void si_registryModified();

private:
     QMap<Descriptor, QList<ActorPrototype*> > groups;

}; // ActorPrototypeRegistry

}//namespace Workflow

}//namespace U2

#endif // _U2_WORKFLOW_REG_H_
