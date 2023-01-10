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

#ifndef _U2_DIFFERENTIALFORMAT_H_
#define _U2_DIFFERENTIALFORMAT_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatus.h>

#include "ColumnDataParser.h"
#include "TextDocumentFormat.h"

namespace U2 {

/**
 * Common format for four cuffdiff outputs:
 * expression, splicing, promoters and cds.
 * http://cufflinks.cbcb.umd.edu/manual.html
 */
class U2FORMATS_EXPORT DifferentialFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    DifferentialFormat(QObject* parent);

protected:
    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

    void storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) override;

private:
    static QList<ColumnDataParser::Column> getColumns();
    static QString getAnnotationName();
    static QList<SharedAnnotationData> parseAnnotations(IOAdapterReader& reader, U2OpStatus& os);
    static QList<SharedAnnotationData> parseAnnotations(const ColumnDataParser& parser, IOAdapterReader& reader, U2OpStatus& os);
    static QList<ColumnDataParser::Column> getHeaderColumns(const QList<GObject*>& anns, U2OpStatus& os);
    static void writeHeader(IOAdapterWriter& writer, const QList<ColumnDataParser::Column>& columns, U2OpStatus& os);

    static QString readLine(IOAdapterReader& reader, QByteArray& buffer, U2OpStatus& os);
    static bool parseLocus(const QString& locus, SharedAnnotationData& data, U2OpStatus& os);
    static QString createLocus(const SharedAnnotationData& data, U2OpStatus& os);
    static QString createValue(const SharedAnnotationData& data, const ColumnDataParser::Column& column, U2OpStatus& os);

private:
    /** Maximum line length supported by the format. */
    static constexpr int MAX_LINE_LENGTH = 100000;
};

}  // namespace U2

#endif  // _U2_DIFFERENTIALFORMAT_H_
