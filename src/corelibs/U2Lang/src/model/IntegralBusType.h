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

#ifndef _U2_INTEGRAL_BUS_TYPE_H_
#define _U2_INTEGRAL_BUS_TYPE_H_

#include <U2Lang/Datatype.h>
#include <U2Lang/IntegralBusModel.h>

namespace U2 {

namespace Workflow {

/**
 * IntegralPort can receive data from actors that are not connected with him directly
 * but connected in transitive closure of schema graph
 *
 * for such purposes, we need IntegralBusType
 */
class U2LANG_EXPORT IntegralBusType : public MapDataType {
public:
    IntegralBusType(const Descriptor& d, const QMap<Descriptor, DataTypePtr>& m);

    // add port's type to map
    void addInputs(const Port* p, bool addPaths);
    void addOutput(DataTypePtr, const Port* producer);

public:
    // in busmap (see IntegralBusPort) attributes of other actors saved as 'actorId:attrId'
    // these are utility functions to work with it
    static Descriptor assignSlotDesc(const Descriptor& elementDesc, const Port* producer);
    static ActorId parseSlotDesc(const QString& id);
    static QString parseAttributeIdFromSlotDesc(const QString & str);
    // when schema is deeply copied we need to remap actorIds in busmap
    static void remap(StrStrMap& busMap, const QMap<ActorId, ActorId>&);
    static void remapPaths(SlotPathMap &pathsMap, const QMap<ActorId, ActorId> &actorIdsMap);
    static void remapSlotString(QString &slotStr, const QMap<ActorId, ActorId> &actorIdsMap);

}; // IntegralBusType

} //namespace Workflow

} //namespace U2

#endif // _U2_INTEGRAL_BUS_TYPE_H_
