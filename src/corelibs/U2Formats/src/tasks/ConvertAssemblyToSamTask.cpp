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
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2DbiRegistry.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/SAMFormat.h>

#include <QSharedPointer>

#include "ConvertAssemblyToSamTask.h"

namespace U2 {

ConvertAssemblyToSamTask::ConvertAssemblyToSamTask(GUrl db, GUrl sam)
: Task("ConvertAssemblyToSamTask", TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
    dbFileUrl(db),
    samFileUrl(sam),
    handle(NULL)
{
}

ConvertAssemblyToSamTask::ConvertAssemblyToSamTask(const DbiConnection *h, GUrl sam)
: Task("ConvertAssemblyToSamTask", TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
    dbFileUrl(NULL),
    samFileUrl(sam),
    handle(h)
{
}

ConvertAssemblyToSamTask::ConvertAssemblyToSamTask(const U2EntityRef& entityRef, GUrl sam)
: Task("ConvertAssemblyToSamTask", TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
    dbFileUrl(NULL),
    samFileUrl(sam),
    assemblyEntityRef(entityRef),
    handle(NULL)
{
}


void ConvertAssemblyToSamTask::run() {
    taskLog.details("Start converting assemblies to SAM");
    // Init assembly objects
    QSharedPointer<DbiConnection> dbiHandle;

    if (NULL == handle) {
        if (assemblyEntityRef.isValid()) {
            dbiHandle = QSharedPointer<DbiConnection>(
                new DbiConnection(assemblyEntityRef.dbiRef,
                false,
                stateInfo));
        }
        else {
            dbiHandle = QSharedPointer<DbiConnection>(
                new DbiConnection(U2DbiRef(SQLITE_DBI_ID, dbFileUrl.getURLString()),
                false,
                stateInfo));
        }
        handle = dbiHandle.data();
    }

    if (handle->dbi == NULL){
        setError(tr("Given file is not valid UGENE database file"));
        return;
    }

    U2ObjectDbi *odbi = handle->dbi->getObjectDbi();
    QList<U2DataId> objectIds;
    // If the entityRef has been passed to the class constructor,
    // then leave only one object with the specified ID
    if (assemblyEntityRef.isValid()) {
        objectIds.append(assemblyEntityRef.entityId);
    }
    // Otherwise convert all assembly objects
    else {
        objectIds = odbi->getObjects(U2Type::Assembly, 0, U2DbiOptions::U2_DBI_NO_LIMIT, stateInfo);
    }

    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::SAM);
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(samFileUrl));
    QScopedPointer<Document> doc(format->createNewLoadedDocument(iof, samFileUrl , stateInfo));
    CHECK_OP(stateInfo, );
    doc->setDocumentOwnsDbiResources(false);
    foreach (const U2DataId &id, objectIds) {
        U2Assembly assembly = handle->dbi->getAssemblyDbi()->getAssemblyObject(id, stateInfo);
        CHECK_OP(stateInfo, );
        U2EntityRef ref(handle->dbi->getDbiRef(), id);
        QString name = assembly.visualName.replace(QRegExp("\\s|\\t"), "_").toLatin1();
        doc->addObject(new AssemblyObject(name, ref));
    }

    BAMUtils::writeDocument(doc.data(), stateInfo);
    taskLog.details("Finish converting assemblies to SAM");
}

QString ConvertAssemblyToSamTask::generateReport() const {
    if (hasError() || isCanceled()) {
        return QString("Conversion task was finished with an error: %1").arg(getError());
    }

    return QString("Conversion task was finished. A new SAM file is: <a href=\"%1\">%2</a>").arg(samFileUrl.getURLString()).arg(samFileUrl.getURLString());
}

} // U2
