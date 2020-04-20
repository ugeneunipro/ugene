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

#include <QScopedPointer>

#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/WorkflowContext.h>

#include "AssemblyMessageTranslator.h"

const char *ASSEMBLY_LENGTH_LABEL = "Length: ";
const char *COUNT_OF_READS_LABEL = " Count of reads: ";

namespace U2 {

using namespace Workflow;

AssemblyMessageTranslator::AssemblyMessageTranslator( const QVariant &atomicMessage,
    Workflow::WorkflowContext *initContext )
    : BaseMessageTranslator( atomicMessage, initContext )
{
    SAFE_POINT( source.canConvert<SharedDbiDataHandler>( ), "Message doesn't contain dbi reference", );
    SharedDbiDataHandler dbId = source.value<SharedDbiDataHandler>( );
    QScopedPointer<AssemblyObject> assemblyObject( StorageUtils::getAssemblyObject(
        context->getDataStorage( ), dbId ) );
    SAFE_POINT( !assemblyObject.isNull( ), "Couldn't obtain assembly object", );
    assemblyRef = assemblyObject->getEntityRef( );
}

QString AssemblyMessageTranslator::getTranslation( ) const {
    U2OpStatusImpl os;
    DbiConnection connection( assemblyRef.dbiRef, os );
    SAFE_POINT_OP( os, QString( ) );

    U2AssemblyDbi *dbi = connection.dbi->getAssemblyDbi( );
    SAFE_POINT( NULL != dbi, "Invalid assembly DBI!", QString( ) );
    const U2DataId assemblyId = assemblyRef.entityId;
    const qint64 assemblyLength = dbi->getMaxEndPos( assemblyId, os ) + 1;
    SAFE_POINT_OP( os, QString( ) );

    const U2Region wholeAssembly( 0, assemblyLength );
    const qint64 countOfReads = dbi->countReads( assemblyId, wholeAssembly, os );
    SAFE_POINT_OP( os, QString( ) );

    QString result = QObject::tr( ASSEMBLY_LENGTH_LABEL ) + QString::number( assemblyLength )
        + INFO_FEATURES_SEPARATOR;
    result += QObject::tr( COUNT_OF_READS_LABEL ) + QString::number( countOfReads );

    return result;
}

} // namespace U2
