/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ABSTRACT_VARIATION_FORMAT_H_
#define _U2_ABSTRACT_VARIATION_FORMAT_H_

#include <U2Core/DocumentModel.h>
#include <U2Core/VariantTrackObject.h>

#include "TextDocumentFormat.h"

namespace U2 {

class U2FORMATS_EXPORT AbstractVariationFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    enum ColumnRole {
        ColumnRole_Unknown = 0,
        ColumnRole_StartPos,
        ColumnRole_EndPos,
        ColumnRole_RefData,
        ColumnRole_ObsData,
        ColumnRole_PublicId,
        ColumnRole_ChromosomeId,
        ColumnRole_Info
    };

    enum PositionIndexing {
        ZeroBased = 0,
        OneBased
    };

    // Variation1: chr1 123 G A,C
    // to
    // Variation1.1: chr1 123 G A
    // Variation1.2: chr1 123 G C
    enum SplitAlleles {
        Split = 0,
        NoSplit
    };

    AbstractVariationFormat(QObject* p, const DocumentFormatId& id, const QStringList& fileExtensions, bool _isSupportHeader = false);

protected:
    bool isSupportHeader;
    /** Write only columns up to 'maxColumnIndex'. */
    bool useOnlyBaseColumns = false;

    QMap<int, ColumnRole> columnRoles;

    int maxColumnIndex = 0;

    PositionIndexing indexing;

    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

    void storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) override;

    void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

    virtual bool checkFormatByColumnCount(int columnCount) const = 0;

    static const QString META_INFO_START;
    static const QString HEADER_START;
    static const QString COLUMNS_SEPARATOR;

public:
    void storeHeader(const VariantTrackObject* trackObject, IOAdapterWriter& writer, U2OpStatus& os) const;

    void storeTrack(IOAdapterWriter& writer, const VariantTrackObject* trackObject, U2OpStatus& os) const;

    static QString getMetaInfo(const VariantTrackObject* trackObject, U2OpStatus& os);

    static QStringList getHeader(const VariantTrackObject* trackObject, U2OpStatus& os);
};

}  // namespace U2

#endif  // _U2_ABSTRACT_VARIATION_FORMAT_H_
