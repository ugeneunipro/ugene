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

#ifndef _U2_SLOT_RELATION_H_
#define _U2_SLOT_RELATION_H_

#include <U2Core/global.h>

namespace U2 {

class U2LANG_EXPORT SlotRelationDescriptor{
public:
    SlotRelationDescriptor(const QString& portId, const QString& slotId, const QVariantList& valuesWithEnabledSlot)
        : portId(portId), slotId(slotId), valuesWithEnabledSlot(valuesWithEnabledSlot) {}
    virtual ~SlotRelationDescriptor() {}

    virtual SlotRelationDescriptor *clone() const { return new SlotRelationDescriptor(*this); }

    virtual bool isSlotEnabled(const QVariant& attrValue) const { return valuesWithEnabledSlot.contains(attrValue); }

    QString       portId;
    QString       slotId;
    QVariantList  valuesWithEnabledSlot;
};

} // U2 namespace

#endif // _U2_SLOT_RELATION_H_
