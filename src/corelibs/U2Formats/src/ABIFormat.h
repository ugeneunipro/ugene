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
class U2OpStatus;
class Chromatogram;

class U2FORMATS_EXPORT ABIFormat : public DocumentFormat {
    Q_OBJECT
public:
    ABIFormat(QObject* p);

    FormatCheckResult checkRawData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

protected:
    Document* loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) override;

    DNASequence* loadSequence(IOAdapter* io, U2OpStatus& ti) override;

private:
    Document* parseABI(const U2DbiRef& dbiRef, SeekableBuf*, IOAdapter* io, const QVariantMap& fs, U2OpStatus& os);
    bool loadABIObjects(SeekableBuf* fp, DNASequence& seq, Chromatogram& chromatogram);
};

}  // namespace U2
