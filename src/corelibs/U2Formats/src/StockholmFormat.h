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
    static const QByteArray FILE_ANNOTATION_ID;
    static const QByteArray FILE_ANNOTATION_AC;
    static const QByteArray FILE_ANNOTATION_DE;
    static const QByteArray FILE_ANNOTATION_GA;
    static const QByteArray FILE_ANNOTATION_NC;
    static const QByteArray FILE_ANNOTATION_TC;
    static const QByteArray COLUMN_ANNOTATION_SS_CONS;
    static const QByteArray COLUMN_ANNOTATION_RF;
    static const QByteArray UNI_ANNOTATION_MARK;

private:
    QString format_name;

public:
    StockholmFormat( QObject* obj );

    virtual DocumentFormatId getFormatId() const {
        return BaseDocumentFormats::STOCKHOLM;
    }

    virtual const QString& getFormatName() const {
        return format_name;
    }

    virtual void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os);

    virtual bool isObjectOpSupported( const Document* d , DocumentFormat::DocObjectOp op, GObjectType t ) const;

    //exceptions
    struct StockholmBaseException {
        QString msg;
        StockholmBaseException( const QString& str ): msg( str ){}
    };
    struct ReadError : public StockholmBaseException {
        ReadError(const GUrl& url);
    };
    struct WriteError: public StockholmBaseException {
        WriteError(const GUrl& url);
    };
    struct BadFileData : public StockholmBaseException {
        BadFileData( const QString& msg ): StockholmBaseException( msg ){}
    };

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& data, const GUrl& = GUrl()) const;

    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);

}; // StockholmFormat

} // namespace


#endif //_U2_STOCKHOLM_FORMAT_H_
