/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GFF_FORMAT_H_
#define _U2_GFF_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT  GFFFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    GFFFormat(QObject* p);

    virtual DocumentFormatId getFormatId() const {return BaseDocumentFormats::GFF;}

    virtual const QString& getFormatName() const {return formatName;}

    virtual void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;

    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

private:
    void load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& si);

    static QString extractSeqObjectName( QString &fastaHeaderName, const QStringList &words, QSet<QString> &names, bool &isNameModified);

    QStringList parseLine(const QString& line) const;

    QString formatName;

    static const int LOCAL_READ_BUFFER_SIZE;
};

}//namespace

#endif
