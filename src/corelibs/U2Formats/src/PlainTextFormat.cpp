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

#include "PlainTextFormat.h"

#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/TextObject.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

PlainTextFormat::PlainTextFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::PLAIN_TEXT, DocumentFormatFlags_W1, {"txt"}) {
    formatName = tr("Plain text");
    supportedObjectTypes += GObjectTypes::TEXT;
    formatDescription = tr("A simple plain text file.");
}

Document* PlainTextFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    // Read the whole text file.
    QString text;
    reader.read(os, text, -1);
    CHECK_OP(os, nullptr);

    // todo: check file-readonly status?

    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, nullptr);

    QVariantMap textObjectHints;
    textObjectHints.insert(DBI_FOLDER_HINT, hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
    TextObject* textObject = TextObject::createInstance(text, reader.getURL().baseFileName(), dbiRef, os, textObjectHints);
    CHECK_OP(os, nullptr);
    QList<GObject*> objects = {textObject};
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints);
}

void PlainTextFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    CHECK(document->getObjects().size() == 1, );
    auto textObject = qobject_cast<TextObject*>(document->getObjects().first());
    SAFE_POINT(textObject != nullptr, L10N::nullPointerError("Text object"), );
    QString text = textObject->getText();
    writer.write(os, text);
}

FormatCheckResult PlainTextFormat::checkRawTextData(const QString&, const GUrl&) const {
    // Any plain text document can be opened as a plain text.
    return FormatDetection_LowSimilarity;
}

}  // namespace U2
