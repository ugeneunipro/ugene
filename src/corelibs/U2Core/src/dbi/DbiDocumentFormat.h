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

#ifndef _U2_SQLITE_DOCUMENT_FORMAT_H_
#define _U2_SQLITE_DOCUMENT_FORMAT_H_

#include <U2Core/U2Dbi.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

/** UGENE v1 wrapper over DBI (UGENE v2 data access) interface */
class U2CORE_EXPORT DbiDocumentFormat: public DocumentFormat {
    Q_OBJECT
public:
    DbiDocumentFormat(const U2DbiFactoryId& id, const DocumentFormatId& formatId,
        const QString& formatName, const QStringList& exits, DocumentFormatFlags flags, QObject* p = NULL);

    virtual void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os);

    virtual FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& url = GUrl()) const;

protected:
    virtual Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

    QList<GObject *> prepareObjects(DbiConnection &handle, const QList<U2DataId> &objectIds);
    QList<GObject *> cloneObjects(const QList<GObject *> &srcObjects, const U2DbiRef &dstDbiRef, const QVariantMap &hints, U2OpStatus &os);

private:
    U2DbiFactoryId           id;
};

}//namespace

#endif
