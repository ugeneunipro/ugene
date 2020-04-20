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
    DifferentialFormat(QObject *parent);

    void storeDocument(Document *d, IOAdapter *io, U2OpStatus &os);

protected:
    FormatCheckResult checkRawTextData(const QByteArray &rawData, const GUrl &url = GUrl()) const;
    Document * loadTextDocument(IOAdapter *io, const U2DbiRef &targetDb, const QVariantMap &hints, U2OpStatus &os);

private:
    QList<ColumnDataParser::Column> getColumns() const;
    QString getAnnotationName() const;
    QList<SharedAnnotationData> parseAnnotations(IOAdapter *io, U2OpStatus &os);
    QList<SharedAnnotationData> parseAnnotations(const ColumnDataParser &parser, IOAdapter *io, QByteArray &buffer, U2OpStatus &os);
    QList<ColumnDataParser::Column> getHeaderColumns(const QList<GObject*> &anns, U2OpStatus &os);
    void writeHeader(IOAdapter *io, const QList<ColumnDataParser::Column> &columns);

    static QString readLine(IOAdapter *io, QByteArray &buffer, U2OpStatus &os);
    static bool parseLocus(const QString &locus, SharedAnnotationData &data, U2OpStatus &os);
    static QString createLocus(const SharedAnnotationData &data, U2OpStatus &os);
    static QString createValue(const SharedAnnotationData &data, const ColumnDataParser::Column &column, U2OpStatus &os);

private:
    static const int BUFFER_SIZE;
};

} // U2

#endif // _U2_DIFFERENTIALFORMAT_H_
