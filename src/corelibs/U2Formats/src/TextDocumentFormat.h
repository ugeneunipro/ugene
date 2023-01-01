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

#ifndef _U2_TEXT_FORMAT_H_
#define _U2_TEXT_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

class IOAdapterReader;
class IOAdapterWriter;

/*
 * Base class for all non binary document formats that can be opened in a usual text editor.
 *
 * Warning: This implementation is deprecated: it uses hacks with unpredictable behavior for multi-byte encodings and does not support Unicode correctly.
 * Use the new 'TextDocumentFormat' class.
 */
class U2FORMATS_EXPORT TextDocumentFormatDeprecated : public DocumentFormat {
public:
    TextDocumentFormatDeprecated(QObject* p, const DocumentFormatId& id, DocumentFormatFlags _flags, const QStringList& fileExts = QStringList());
    virtual FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& = GUrl()) const;

protected:
    virtual DNASequence* loadSequence(IOAdapter* io, U2OpStatus& ti);
    virtual Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os);
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const = 0;
    virtual DNASequence* loadTextSequence(IOAdapter* io, U2OpStatus& ti);
    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) = 0;
};

/*
 * Base class for all text (non-binary) document formats that can be opened in a usual text editor.
 */
class U2FORMATS_EXPORT TextDocumentFormat : public DocumentFormat {
public:
    TextDocumentFormat(QObject* p, const DocumentFormatId& id, DocumentFormatFlags _flags, const QStringList& fileExts = QStringList());

protected:
    /** Checks if the 'dataPrefix' text is stored using this document format. */
    virtual FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const = 0;

    /**
     * Loads a document from the given stream.
     * 'hints' map contains set of loading hints like 'DocumentReadingMode_SequenceMergeGapSize', etc…
     * If loading is failed the nullptr is returned and the error is stored in 'os', otherwise a non-nullptr value is returned.
     */
    virtual Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) = 0;

    /**
     * Loads a DNASequence object from the current state of the reader.
     * Returns a valid sequence object or nullptr if there are no more sequences left in the stream.
     * By default returns nullptr & error that sequence loading is not supported by the current format.
     */
    virtual DNASequence* loadTextSequence(IOAdapterReader& reader, U2OpStatus& os);

    /**
     * Serializes 'document' into the given text stream. Sets error to 'os' if serialization fails.
     * By default text documents do not support writing, so this method must be re-implemented in every format to support serialization.
     */
    virtual void storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os);

    /**
     * Stores all supported document types from the given map of objects.
     * The document format should preserve the objects order in the map when possible.
     * The default implementation does nothing and sets 'unsupported' error to 'os'.
     */
    virtual void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os);

private:
    /** Detects encoding of the raw binary data and calls 'checkRawTextData'. */
    FormatCheckResult checkRawData(const QByteArray& rawBinaryData, const GUrl& originalDataUrl) const override;

    /** Delegates this call to 'loadTextDocument' with a valid text reader instance. */
    Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

    /** Delegates this call to 'loadTextSequence' with a valid text reader instance. */
    DNASequence* loadSequence(IOAdapter* io, U2OpStatus& os) override;

    /** Delegates this call to 'storeTextDocument' with a valid text writer instance. */
    void storeDocument(Document* document, IOAdapter* io, U2OpStatus& os) override;

    /** Delegates this call to 'storeTextEntry' with a valid text writer instance. */
    void storeEntry(IOAdapter* io, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;
};

}  // namespace U2

#endif
