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

#ifndef _U2_FASTA_FORMAT_H_
#define _U2_FASTA_FORMAT_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include "TextDocumentFormat.h"

namespace U2 {

class IOAdapter;

class U2FORMATS_EXPORT FastaFormat : public TextDocumentFormat {
    Q_OBJECT
public:
    FastaFormat(QObject* parent);

    /** First header-line symbol for every sequence block in FASTA files. */
    static constexpr char FASTA_HEADER_START_SYMBOL = '>';

    /** A comment marker: the line started with this symbol is a comment. */
    static constexpr char FASTA_COMMENT_START_SYMBOL = ';';

    /** Recommended length of the FASTA sequence line. Using 80 symbols to make FASTA files readable in text editors. */
    static constexpr int FASTA_SEQUENCE_LINE_LENGTH = 80;

    /** Saves sequence to the ioAdapter in FASTA format. */
    static void storeSequence(const DNASequence& sequence, IOAdapter* ioAdapter, U2OpStatus& os);

    /** Saves sequence to the ioAdapter in FASTA format. */
    static void storeSequence(const U2SequenceObject* sequence, IOAdapter* ioAdapter, U2OpStatus& os);

    /** Parses input text and returns map of sequence name -> sequence values. */
    static QList<QPair<QString, QString>> getSequencesAndNamesFromUserInput(const QString& userInput, U2OpStatus& os);

protected:
    /** Checks if the 'rawTextData' text is stored using this document format. */
    FormatCheckResult checkRawTextData(const QString& dataPrefix, const GUrl& originalDataUrl) const override;

    Document* loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) override;

    DNASequence* loadTextSequence(IOAdapterReader& reader, U2OpStatus& os) override;

    void storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) override;

    void storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) override;
};

}  // namespace U2

#endif
