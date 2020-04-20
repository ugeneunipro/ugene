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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DatabaseConnectionAdapter.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2VariantDbi.h>

#include "DatabaseConnectionFormat.h"

namespace U2 {

DatabaseConnectionFormat::DatabaseConnectionFormat(QObject *p) :
    DocumentFormat(p, BaseDocumentFormats::DATABASE_CONNECTION,
                   DocumentFormatFlags(DocumentFormatFlag_NoPack)
                   | DocumentFormatFlag_NoFullMemoryLoad
                   | DocumentFormatFlag_Hidden
                   | DocumentFormatFlag_SupportWriting
                   | DocumentFormatFlag_CannotBeCreated
                   | DocumentFormatFlag_AllowDuplicateNames
                   | DocumentFormatFlag_DirectWriteOperations)
{
    formatName = DocumentFormat::tr("Database connection");
    formatDescription = DocumentFormat::tr("A fake format that was added to implement shared database connection within existing document model.");

    supportedObjectTypes << GObjectTypes::SEQUENCE
                         << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT
                         << GObjectTypes::UNKNOWN
                         << GObjectTypes::UNLOADED
                         << GObjectTypes::TEXT
                         << GObjectTypes::SEQUENCE
                         << GObjectTypes::ANNOTATION_TABLE
                         << GObjectTypes::VARIANT_TRACK
                         << GObjectTypes::CHROMATOGRAM
                         << GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT
                         << GObjectTypes::PHYLOGENETIC_TREE
                         << GObjectTypes::BIOSTRUCTURE_3D
                         << GObjectTypes::ASSEMBLY;
}

const QString& DatabaseConnectionFormat::getFormatName() const {
    return formatName;
}

FormatCheckResult DatabaseConnectionFormat::checkRawData(QByteArray const &, GUrl const &) const {
    return FormatCheckResult();
}

Document* DatabaseConnectionFormat::loadDocument(IOAdapter* io, const U2DbiRef&, const QVariantMap& hints, U2OpStatus& os) {
    DatabaseConnectionAdapter* databaseConnectionAdapter = qobject_cast<DatabaseConnectionAdapter*>(io);
    SAFE_POINT(NULL != databaseConnectionAdapter, QString("Can't use current IOAdapter: %1").arg(io->getAdapterName()), NULL);

    U2Dbi* dbi = databaseConnectionAdapter->getConnection().dbi;
    SAFE_POINT(NULL != dbi, "NULL dbi", NULL);
    QList<GObject*> objects = getObjects(dbi, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    const QString modLockDesc = dbi->getFeatures().contains(U2DbiFeature_GlobalReadOnly)
        ? DocumentFormat::tr("You have no permissions to modify the content of this database")
        : QString();
    Document* resultDocument = new Document(this, io->getFactory(), io->getURL(), dbi->getDbiRef(), objects, hints, modLockDesc);
    resultDocument->setDocumentOwnsDbiResources(false);
    return resultDocument;
}

namespace {
    void updateProgress(U2OpStatus &os, int current, int size) {
        if (0 == size) {
            os.setProgress(100);
        } else {
            os.setProgress(100*current/size);
        }
    }
}

#define CHECK_OBJECT(object) \
    CHECK_OP(os, NULL); \
    if (object.visualName.isEmpty()) { \
    os.setError(DocumentFormat::tr("Empty object name")); \
        return NULL; \
    }

#define UPDATE_STATE() \
    updateProgress(os, progressCounter, objectCount); \
    progressCounter++; \
    CHECK(!os.isCanceled(), resultList);

QList<GObject*> DatabaseConnectionFormat::getObjects(U2Dbi* dbi, U2OpStatus &os) {
    QList<GObject*> resultList;

    QHash<U2DataId, QString> object2Name = dbi->getObjectDbi()->getObjectNames(0, U2DbiOptions::U2_DBI_NO_LIMIT, os);
    CHECK_OP(os, resultList);

    int progressCounter = 0;
    const int objectCount = object2Name.size();
    Q_UNUSED(progressCounter);
    Q_UNUSED(objectCount);
    const U2DbiRef dbiRef = dbi->getDbiRef();

    foreach (const U2DataId &id, object2Name.keys()) {
        UPDATE_STATE();
        GObject *obj = GObjectUtils::createObject(dbiRef, id, object2Name.value(id));
        if (NULL != obj) {
            resultList << obj;
        }
    }

    return resultList;
}

}   // namespace U2
