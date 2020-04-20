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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DatabaseConnectionAdapter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ConnectSharedDatabaseTask.h"

namespace U2 {

ConnectSharedDatabaseTask::ConnectSharedDatabaseTask(const U2DbiRef &dbiRef, const QString &_documentName, bool initializeDb)
    : DocumentProviderTask(tr("Connecting to database: ") + getUrlFromRef(dbiRef).getURLString(), TaskFlag_None),
    dbiRef(dbiRef), documentName(_documentName), initializeDb(initializeDb)
{
    GCOUNTER(cvar, tvar, "ConnectSharedDatabaseTask");
    documentDescription = documentName;
}

void ConnectSharedDatabaseTask::run() {
    if (initializeDb) {
        DbiConnection con(dbiRef, stateInfo);
        CHECK_OP(stateInfo, );
        con.dbi->populateDefaultSchema(stateInfo);
        CHECK_OP(stateInfo, );
    }

    IOAdapterFactory* ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::DATABASE_CONNECTION);
    SAFE_POINT_EXT(NULL != ioAdapterFactory, setError("Database connection IO adapter factory is NULL"), );

    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::DATABASE_CONNECTION);
    SAFE_POINT_EXT(NULL != format, setError("Database connection format is NULL"), );

    resultDocument = format->loadDocument(ioAdapterFactory, getUrlFromRef(dbiRef), QVariantMap(), stateInfo);
    CHECK_OP(stateInfo, );
    resultDocument->setName(documentName);
}

GUrl ConnectSharedDatabaseTask::getUrlFromRef(const U2DbiRef& dbiRef) {
    return GUrl(dbiRef.dbiId, GUrl_Network);
}

}   // namespace U2
