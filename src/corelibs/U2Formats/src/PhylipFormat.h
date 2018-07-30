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

#ifndef _U2_PHYLIP_FORMAT_H_
#define _U2_PHYLIP_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

#include "TextDocumentFormat.h"

namespace U2 {

class U2FORMATS_EXPORT PhylipFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    PhylipFormat(QObject *p);
    virtual void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os);

protected:
    MultipleSequenceAlignmentObject* load(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap &fs, U2OpStatus& os);
    bool parseHeader(QByteArray data, int &species, int &characters) const;
    void removeSpaces(QByteArray &data) const;
    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

    virtual MultipleSequenceAlignment parse(IOAdapter* io, U2OpStatus &os) const = 0;

    QString formatName;
};

class U2FORMATS_EXPORT PhylipSequentialFormat : public PhylipFormat {
    Q_OBJECT
public:
    PhylipSequentialFormat(QObject* p);
    virtual DocumentFormatId getFormatId() const {return BaseDocumentFormats::PHYLIP_SEQUENTIAL;}
    virtual const QString& getFormatName() const {return formatName;}
    virtual void storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;
    virtual MultipleSequenceAlignment parse(IOAdapter* io, U2OpStatus &os) const;
};


class U2FORMATS_EXPORT PhylipInterleavedFormat : public PhylipFormat {
    Q_OBJECT
public:
    PhylipInterleavedFormat(QObject* p);
    virtual DocumentFormatId getFormatId() const {return BaseDocumentFormats::PHYLIP_INTERLEAVED;}
    virtual const QString& getFormatName() const {return formatName;}
    virtual void storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;
    MultipleSequenceAlignment parse(IOAdapter *io, U2OpStatus &os) const;
};

} // namespace

#endif // _U2_PHYLIP_FORMAT_H_
