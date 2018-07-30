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
#include <U2Core/L10n.h>
#include <U2Core/TextObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "PlainTextFormat.h"

namespace U2 {

/* TRANSLATOR U2::IOAdapter */

PlainTextFormat::PlainTextFormat(QObject* p) : TextDocumentFormat(p, DocumentFormatFlags_W1, QStringList("txt")) {
    formatName = tr("Plain text");
    supportedObjectTypes+=GObjectTypes::TEXT;
    formatDescription = tr("A simple plain text file.");
}

#define BUFF_SIZE 1024

Document* PlainTextFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os){
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    QString text;
    int size = io->left();
    if (size > 0) {
        text.reserve(size);
    }
    QByteArray block(BUFF_SIZE, '\0');
    int blockLen = 0;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        int sizeBefore = text.length();
        QString line = QString::fromLocal8Bit(block.data(), blockLen);
        text.append(line);
        if (text.length() != sizeBefore + blockLen) {
            os.setError(L10N::errorReadingFile(io->getURL()));
            break;
        }
        os.setProgress(io->getProgress());
    }

    CHECK_OP(os, NULL);

    //todo: check file-readonly status?

    QVariantMap hints;
    hints.insert(DBI_FOLDER_HINT, fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));

    TextObject* to = TextObject::createInstance(text, io->getURL().baseFileName(), dbiRef, os, hints);
    CHECK_OP(os, NULL);
    QList<GObject*> objects;
    objects.append(to);
    Document* d = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
    return d;
}

void PlainTextFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    CHECK(d->getObjects().size() == 1, );
    GObject* obj = d->getObjects().first();
    TextObject* to = qobject_cast<TextObject*>(obj);
    SAFE_POINT(NULL != to, L10N::nullPointerError("Text object"), );
    const QString& text = to->getText();

    QByteArray local8bit = text.toLocal8Bit();
    storeRawData(local8bit, os, io);
}

void PlainTextFormat::storeRawData(const QByteArray& rawData, U2OpStatus& ts, IOAdapter* io) {
    int nWritten = 0;
    int nTotal = rawData.size();
    while(nWritten < nTotal) {
        int l = io->writeBlock(rawData.data() + nWritten, nTotal - nWritten);
        if (l <= 0 ) {
            ts.setError(L10N::errorWritingFile(io->getURL()));
            return;
        }
        nWritten+= l;
    }
}


FormatCheckResult PlainTextFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    return hasBinaryData ? FormatDetection_NotMatched :FormatDetection_LowSimilarity;
}

}//namespace
