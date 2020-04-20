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

#ifndef _WORKFLOW_ELEMENT_FACADE_H_
#define _WORKFLOW_ELEMENT_FACADE_H_

#include <QString>

#include "globals.h"

namespace U2 {

class WorkflowElementFacade
{
public:
    static U2ErrorType      doesElementTypeExist( const QString &type, bool *exists );
    static U2ErrorType      getElementNameByType( const QString &type, QString &name );

    static U2ErrorType      doesElementHaveParameter( const QString &elementType,
                                const QString &parameterName, bool *has );
    static U2ErrorType      doesElementHavePort( const QString &elementType,
                                const QString &portId, bool *has );
    static U2ErrorType      doesElementHaveOutputSlot( const QString &elementType,
                                const QString &slotId, bool *has );
    static U2ErrorType      doesElementHaveInputSlot( const QString &elementType,
                                const QString &portId, const QString &slotId, bool *has );
    static U2ErrorType      doesElementSuitForSas( const QString &elementType, bool *suits );
    /*
     * If `portId` is an empty string then `slotId` will contain an arbitrary slot id
     * from an arbitrary element port with given direction. Such behavior might be useful
     * if the element is known to have only one port with one slot.
     *
     * If `portId` is non empty string then the `inputSlot` value is ignored
     */
    static U2ErrorType      getElementSlotIds( const QString &elementType, bool inputSlot,
                                const QString &portId, QStringList &slotIds );
    static U2ErrorType      getReadElementTypeForSlot( const QString &slotId, QString &readerType );
    static U2ErrorType      getWriteElementTypeForSlot( const QString &slotId, QString &writerType);
    static U2ErrorType      getInputPortIdForSlot( const QString &elementId, const QString &slotId, QString &portId );
    static U2ErrorType      getOutputPortIdForSlot( const QString &elementId, const QString &slotId, QString &portId );
};

} // namespace U2

#endif // _WORKFLOW_ELEMENT_FACADE_H_
