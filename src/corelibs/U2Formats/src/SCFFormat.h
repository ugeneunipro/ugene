/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

namespace U2 {

class IOAdapter;
class SeekableBuf;
class Chromatogram;
class DNASequence;
class U2OpStatus;

class U2FORMATS_EXPORT SCFFormat : public DocumentFormat {
    Q_OBJECT
public:
    SCFFormat(QObject* p);

    FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

    static void exportDocumentToSCF(const QString& fileName, const Chromatogram& cd, const QByteArray& seq, U2OpStatus& ts);

protected:
    Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;

    DNASequence* loadSequence(IOAdapter* io, U2OpStatus& ti) override;

private:
    Document* parseSCF(const U2DbiRef& dbiRef, IOAdapter* io, const QVariantMap& fs, U2OpStatus& os);

    bool loadSCFObjects(IOAdapter* io, DNASequence& dna, Chromatogram& chromatogram, U2OpStatus& os);
};

}  // namespace U2
