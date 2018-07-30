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

#include <U2Core/IOAdapter.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatus.h>

#include "TextDocumentFormat.h"

namespace U2 {

TextDocumentFormat::TextDocumentFormat(QObject* p, DocumentFormatFlags _flags, const QStringList& fileExts) : DocumentFormat(p, _flags, fileExts) {}

DNASequence* TextDocumentFormat::loadSequence(IOAdapter* io, U2OpStatus& ti) {
    io->setFormatMode(IOAdapter::TextMode);
    DNASequence* seq = loadTextSequence(io, ti);

    return seq;
}

FormatCheckResult TextDocumentFormat::checkRawData(const QByteArray& rawData, const GUrl& url) const {
    QByteArray cuttedRawData = TextUtils::cutByteOrderMarks(rawData);
    FormatCheckResult checkResult = checkRawTextData(cuttedRawData, url);

    return checkResult;
}

Document* TextDocumentFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    io->setFormatMode(IOAdapter::TextMode);
    Document* doc = loadTextDocument(io, dbiRef, fs, os);

    return doc;
}

DNASequence* TextDocumentFormat::loadTextSequence(IOAdapter* io, U2OpStatus& ti) {
    ti.setError("This document format does not support streaming reading mode");
    return NULL;
}

}