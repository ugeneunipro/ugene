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

#include "EMBLGenbankAbstractDocument.h"

namespace U2 {

class U2FORMATS_EXPORT EMBLPlainTextFormat : public EMBLGenbankAbstractDocument {
    Q_OBJECT
public:
    EMBLPlainTextFormat(QObject* p);

protected:
    FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

    bool readIdLine(ParserState*);
    bool readEntry(ParserState*, U2SequenceImporter&, int& seqSize, int& fullSeqSize, bool merge, int gapSize, U2OpStatus&) override;
    // void readAnnotations(ParserState*, int offset);
    QMap<QString, QString> tagMap;
};

}  // namespace U2
