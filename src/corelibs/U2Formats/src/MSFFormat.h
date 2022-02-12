/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSF_FORMAT_H_
#define _U2_MSF_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT MSFFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    MSFFormat(QObject* parent);

    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;

    void storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) override;

    void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

private:
    static void load(IOAdapterReader& reader, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os);

    static int getCheckSum(const QByteArray& seq);

    static const int CHECK_SUM_MOD;
    static const QByteArray MSF_FIELD;
    static const QByteArray CHECK_FIELD;
    static const QByteArray LEN_FIELD;
    static const QByteArray NAME_FIELD;
    static const QByteArray TYPE_FIELD;
    static const QByteArray WEIGHT_FIELD;
    static const QByteArray TYPE_VALUE_PROTEIN;
    static const QByteArray TYPE_VALUE_NUCLEIC;
    static const double WEIGHT_VALUE;
    static const QByteArray END_OF_HEADER_LINE;
    static const QByteArray SECTION_SEPARATOR;
    static const int CHARS_IN_ROW;
    static const int CHARS_IN_WORD;
};

}  // namespace U2

#endif
