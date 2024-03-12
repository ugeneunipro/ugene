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

#pragma once

#include <QStringList>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;
class DNASequence;

class U2FORMATS_EXPORT FastqFormat : public TextDocumentFormatDeprecated {
    Q_OBJECT
public:
    FastqFormat(QObject* p);

    void storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) override;

    bool isStreamingSupport() override {
        return true;
    }

    void storeEntry(IOAdapter* io, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;

    static void writeEntry(const QString& seqName, const DNASequence& seq, IOAdapter* io, const QString& errorMessage, U2OpStatus& os, bool cutLines = true);

protected:
    FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

    DNASequence* loadTextSequence(IOAdapter* io, U2OpStatus& os) override;

    Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;
};

}  // namespace U2
