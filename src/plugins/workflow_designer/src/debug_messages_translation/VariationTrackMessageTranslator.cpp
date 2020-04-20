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
#include <QVariant>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2VariantDbi.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/WorkflowContext.h>

#include "VariationTrackMessageTranslator.h"

const char *VARIATIONS_COUNT_LABEL = "Count of variations: ";

namespace U2 {

using namespace Workflow;

VariationTrackMessageTranslator::VariationTrackMessageTranslator( const QVariant &atomicMessage,
    WorkflowContext *initContext )
    : BaseMessageTranslator( atomicMessage, initContext )
{
    SAFE_POINT( source.canConvert<SharedDbiDataHandler>( ), "Message doesn't contain dbi reference", );
    SharedDbiDataHandler dbId = source.value<SharedDbiDataHandler>( );
    QScopedPointer<VariantTrackObject> variantTrackObject( StorageUtils::getVariantTrackObject(
        context->getDataStorage( ), dbId ) );
    SAFE_POINT( !variantTrackObject.isNull( ), "Couldn't obtain variant object", );
    variantTrackRef = variantTrackObject->getEntityRef( );
}

QString VariationTrackMessageTranslator::getTranslation( ) const {
    U2OpStatusImpl os;
    DbiConnection connection( variantTrackRef.dbiRef, os );
    SAFE_POINT_OP( os, QString( ) );

    U2VariantDbi *dbi = connection.dbi->getVariantDbi( );
    SAFE_POINT( NULL != dbi, "Invalid variation DBI!", QString( ) );
    const U2DataId variantId = variantTrackRef.entityId;
    const int variantCount = dbi->getVariantCount( variantId, os );
    SAFE_POINT_OP( os, QString( ) );

    QString result = QObject::tr( VARIATIONS_COUNT_LABEL )
        + QString::number( variantCount );

    return result;
}

} // namespace U2
