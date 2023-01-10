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

#include "TextDocumentFormat.h"

#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

TextDocumentFormatDeprecated::TextDocumentFormatDeprecated(QObject* p, const DocumentFormatId& id, DocumentFormatFlags _flags, const QStringList& fileExts)
    : DocumentFormat(p, id, _flags, fileExts) {
}

DNASequence* TextDocumentFormatDeprecated::loadSequence(IOAdapter* io, U2OpStatus& ti) {
    io->setFormatMode(IOAdapter::TextMode);
    DNASequence* seq = loadTextSequence(io, ti);

    return seq;
}

FormatCheckResult TextDocumentFormatDeprecated::checkRawData(const QByteArray& rawData, const GUrl& url) const {
    QString error;
    QByteArray cuttedRawData = TextUtils::cutByteOrderMarks(rawData, error);
    CHECK(error.isEmpty(), FormatDetection_NotMatched);

    FormatCheckResult checkResult = checkRawTextData(cuttedRawData, url);

    return checkResult;
}

Document* TextDocumentFormatDeprecated::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    io->setFormatMode(IOAdapter::TextMode);
    Document* doc = loadTextDocument(io, dbiRef, fs, os);

    return doc;
}

DNASequence* TextDocumentFormatDeprecated::loadTextSequence(IOAdapter* io, U2OpStatus& ti) {
    Q_UNUSED(io);
    ti.setError("This document format does not support streaming reading mode");
    return nullptr;
}

///////////////////////////////////////////
/// TextDocumentFormat
///////////////////////////////////////////

TextDocumentFormat::TextDocumentFormat(QObject* p, const DocumentFormatId& id, DocumentFormatFlags _flags, const QStringList& fileExts)
    : DocumentFormat(p, id, _flags, fileExts) {
}

FormatCheckResult TextDocumentFormat::checkRawData(const QByteArray& rawBinaryData, const GUrl& url) const {
    GTIMER(c1, t1, "TextDocumentFormat::checkRawData");
    QTextStream stream(rawBinaryData, QIODevice::ReadOnly);  // Use QTextStream to auto-detect multi-byte encoding.
    QString text = stream.readAll();
    // QTextStream does not provide any info if the codec was successfully detected or not and
    // fall backs to a local 8-bit in case if it can't find a correct codec.
    // The check below is a trivial test that filters most of the binaries.
    // The rest of the check should be done in the DocumentFormat itself: the per-format detection must be
    // sensitive enough not to mark bad texts as 'Matched'.
    bool isBinaryData = false;
    for (int i = 0; i < text.length() && !isBinaryData; i++) {
        quint16 unicodeValue = text.at(i).unicode();
        isBinaryData = unicodeValue < TextUtils::BINARY.size() && TextUtils::BINARY.testBit(unicodeValue);
    }
    if (isBinaryData) {
        return FormatDetection_NotMatched;
    }
    FormatCheckResult result = checkRawTextData(text, url);
    result.properties[RawDataCheckResult_RawTextData] = text;
    return result;
}

Document* TextDocumentFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    GTIMER(c1, t1, "TextDocumentFormat::loadDocument");
    CHECK_OP(os, nullptr);
    IOAdapterReader reader(io);  // TODO: store codec in the result document hints.
    Document* document = loadTextDocument(reader, dbiRef, hints, os);
    SAFE_POINT(document != nullptr || os.hasError() || os.isCanceled(), "Either document must not be null or there must be an error/cancel flag!", document);
    return document;
}

DNASequence* TextDocumentFormat::loadSequence(IOAdapter* io, U2OpStatus& os) {
    GTIMER(c1, t1, "TextDocumentFormat::loadSequence");
    CHECK_OP(os, nullptr);
    if (io->isEof()) {
        return nullptr;
    }
    IOAdapterReader reader(io);
    return loadTextSequence(reader, os);
}

DNASequence* TextDocumentFormat::loadTextSequence(IOAdapterReader&, U2OpStatus& os) {
    os.setError(tr("The document format does not support streaming reading mode: %1").arg(getFormatId()));
    return nullptr;
}

void TextDocumentFormat::storeDocument(Document* document, IOAdapter* io, U2OpStatus& os) {
    GTIMER(c1, t1, "TextDocumentFormat::storeDocument");
    IOAdapterWriter writer(io);  // TODO: re-use original codec if possible (store it in the document hints while loading).
    storeTextDocument(writer, document, os);
}

void TextDocumentFormat::storeTextDocument(IOAdapterWriter&, Document*, U2OpStatus& os) {
    os.setError(tr("The document format does not support writing of documents: %1").arg(getFormatId()));
}

void TextDocumentFormat::storeEntry(IOAdapter* io, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    IOAdapterWriter writer(io);  // TODO: re-use original codec if possible (store it in the document hints while loading).
    storeTextEntry(writer, objectsMap, os);
}

void TextDocumentFormat::storeTextEntry(IOAdapterWriter& /*writer*/, const QMap<GObjectType, QList<GObject*>>& /*objectsMap*/, U2OpStatus& os) {
    os.setError(tr("The document format does not support writing of documents in streaming mode: %1").arg(getFormatId()));
}

}  // namespace U2
