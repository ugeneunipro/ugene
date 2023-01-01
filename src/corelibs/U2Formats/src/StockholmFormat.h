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

#ifndef _U2_STOCKHOLM_FORMAT_H_
#define _U2_STOCKHOLM_FORMAT_H_

#include <QByteArray>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT StockholmFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    StockholmFormat(QObject* obj);

    void storeTextDocument(IOAdapterWriter& writer, Document* doc, U2OpStatus& os) override;

    bool isObjectOpSupported(const Document* doc, DocumentFormat::DocObjectOp op, GObjectType t) const override;

protected:
    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

public:
    static const QString FILE_ANNOTATION_ID;
    static const QString FILE_ANNOTATION_AC;
    static const QString FILE_ANNOTATION_DE;
    static const QString FILE_ANNOTATION_GA;
    static const QString FILE_ANNOTATION_NC;
    static const QString FILE_ANNOTATION_TC;
    static const QString COLUMN_ANNOTATION_SS_CONS;
    static const QString COLUMN_ANNOTATION_RF;
    static const QString UNI_ANNOTATION_MARK;

};  // StockholmFormat

}  // namespace U2

#endif  //_U2_STOCKHOLM_FORMAT_H_
