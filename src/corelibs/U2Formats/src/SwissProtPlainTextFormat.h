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

#include <QDate>

#include "EMBLGenbankAbstractDocument.h"

namespace U2 {

class U2FORMATS_EXPORT SwissProtPlainTextFormat : public EMBLGenbankAbstractDocument {
    Q_OBJECT
public:
    SwissProtPlainTextFormat(QObject* p);

protected:
    FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const override;

    bool readIdLine(ParserState*);
    bool readEntry(ParserState*, U2SequenceImporter&, int& seqSize, int& fullSeqSize, bool merge, int gapSize, U2OpStatus&) override;
    bool readSequence(ParserState*, U2SequenceImporter&, int&, int&, U2OpStatus&) override;
    void readAnnotations(ParserState*, int offset) override;
    // SWISS-PROT presented new format rules 11.12.2019
    // If the file has been changed since this date, the following function will return true
    // Otherwise - false
    bool isNewAnnotationFormat(const QVariant& dateList, U2OpStatus& si);
    SharedAnnotationData readAnnotationOldFormat(IOAdapter* io, char* cbuff, int contentLen, int bufSize, U2OpStatus& si, int offset);
    SharedAnnotationData readAnnotationNewFormat(char* cbuff, U2OpStatus& si, int offset);

    QMap<QString, QString> tagMap;

private:
    static void check4SecondaryStructure(AnnotationData* a);
    static void processAnnotationRegion(AnnotationData* a, const int start, const int end, const int offset);

    static const QDate UPDATE_DATE;
    static const QMap<QString, int> MONTH_STRING_2_INT;
    static const QString ANNOTATION_HEADER_REGEXP;
    static const QString ANNOTATION_QUALIFIERS_REGEXP;
};

}  // namespace U2
