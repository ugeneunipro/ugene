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

#include <math.h>

#include <QTextStream>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "ClustalWAlnFormat.h"

namespace U2 {

/* TRANSLATOR U2::ClustalWAlnFormat */
/* TRANSLATOR U2::IOAdapter */

const QString ClustalWAlnFormat::CLUSTAL_HEADER = "CLUSTAL";
const int ClustalWAlnFormat::MAX_LINE_LEN = 190;
// The sequence name's length maximum is defined in the "clustalw.h" file of the "CLUSTALW" source code
const int ClustalWAlnFormat::MAX_NAME_LEN = 150;
const int ClustalWAlnFormat::MAX_SEQ_LEN = 70;
const int ClustalWAlnFormat::SEQ_ALIGNMENT = 5;

ClustalWAlnFormat::ClustalWAlnFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::CLUSTAL_ALN, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, QStringList("aln")) {
    formatName = tr("CLUSTALW");
    formatDescription = tr("Clustalw is a format for storing multiple sequence alignments");
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

void ClustalWAlnFormat::load(IOAdapterReader& reader, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& fs, U2OpStatus& os) {
    QString buf;
    buf.reserve(READ_BUFF_SIZE);

    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;

    QString objName = reader.getURL().baseFileName();
    MultipleSequenceAlignment al(objName);
    bool lineOk = false;
    bool firstBlock = true;
    int sequenceIdx = 0;
    int valStartPos = 0;
    int valEndPos = 0;
    int currentLen = 0;

    // Skip the first line.
    reader.read(os, buf, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    CHECK_OP(os, )

    if (!lineOk || !buf.startsWith(CLUSTAL_HEADER)) {
        os.setError(ClustalWAlnFormat::tr("Illegal header line"));
        return;
    }

    // Read names and sequences.
    while (reader.read(os, buf, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk) > 0 && !os.isCoR()) {
        if (buf.startsWith(CLUSTAL_HEADER)) {
            reader.undo(os);  // Start of the next document in the stream.
            CHECK_OP(os, )
            break;
        }
        int numNs = 0;

        int len = buf.length();
        while (len > 0 && TextUtils::isLineBreak(buf, len - 1)) {
            if (buf[len - 1] == '\n') {
                numNs++;
            }
            len--;
        }
        if (len == 0) {
            if (al->getRowCount() == 0) {
                continue;  // initial empty lines
            }
            os.setError(ClustalWAlnFormat::tr("Error parsing file"));
            break;
        }

        if (valStartPos == 0) {
            int spaceIdx = buf.indexOf(' ');
            int valIdx = spaceIdx + 1;
            while (valIdx < len && TextUtils::isWhiteSpace(buf, valIdx)) {
                valIdx++;
            }
            if (valIdx <= 0 || valIdx >= len - 1) {
                os.setError(ClustalWAlnFormat::tr("Invalid alignment format"));
                break;
            }
            valStartPos = valIdx;
        }

        valEndPos = valStartPos + 1;  // not inclusive
        while (valEndPos < len && !TextUtils::isWhiteSpace(buf, valEndPos)) {
            valEndPos++;
        }
        if (valEndPos != len) {  // there were numbers trimmed -> trim spaces now
            while (valEndPos > valStartPos && (buf[valEndPos] == ' ' || buf[valEndPos] == '\t')) {
                valEndPos--;
            }
            valEndPos++;  // leave non-inclusive
        }

        QString name = buf.left(valStartPos).trimmed();
        QByteArray value = buf.mid(valStartPos, valEndPos - valStartPos).toLatin1();  // DNA sequences use 1-byte chars.

        int seqsInModel = al->getRowCount();
        bool lastBlockLine = (!firstBlock && sequenceIdx == seqsInModel) || numNs >= 2 || name.isEmpty() || value.contains(' ') || value.contains(':') || value.contains('.');

        if (firstBlock) {
            if (lastBlockLine && name.isEmpty()) {  // if name is not empty -> this is a sequence but consensus (for Clustal files without consensus)
                // this is consensus line - skip it
            } else {
                assert(al->getRowCount() == sequenceIdx);
                al->addRow(name, value);
            }
        } else {
            int rowIdx = -1;
            if (sequenceIdx < seqsInModel) {
                rowIdx = sequenceIdx;
            } else if (sequenceIdx == seqsInModel) {
                assert(lastBlockLine);
                // consensus line
            } else {
                os.setError(ClustalWAlnFormat::tr("Incorrect number of sequences in block"));
                break;
            }
            if (rowIdx != -1) {
                const MultipleSequenceAlignmentRow row = al->getMsaRow(rowIdx);
                if (row->getName() != name) {
                    os.setError(ClustalWAlnFormat::tr("Sequence names are not matched: '%1' vs '%2', row index: %3").arg(name, row->getName(), QString::number(rowIdx)));
                    break;
                }
                al->appendChars(rowIdx, currentLen, value.constData(), value.size());
            }
        }
        if (lastBlockLine) {
            firstBlock = false;
            if (!MSAUtils::checkPackedModelSymmetry(al, os)) {
                break;
            }
            sequenceIdx = 0;
            currentLen = al->getLength();
        } else {
            sequenceIdx++;
        }

        os.setProgress(reader.getProgress());
    }
    MSAUtils::checkPackedModelSymmetry(al, os);
    CHECK_OP(os, );

    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet() != nullptr, os.setError(ClustalWAlnFormat::tr("Alphabet is unknown")), );

    QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

Document* ClustalWAlnFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    QList<GObject*> objects;
    load(reader, dbiRef, objects, hints, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), nullptr);
    assert(objects.size() == 1);
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints);
}

void ClustalWAlnFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "Clustal entry storing: no alignment", );
    const QList<GObject*>& als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(als.size() == 1, "Clustal entry storing: alignment objects count error", );

    auto obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(obj != nullptr, "Clustal entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    // Write header.
    QString header("CLUSTAL W 2.0 multiple sequence alignment\n\n");
    writer.write(os, header);
    CHECK_OP(os, );

    // Precalculate maximum sequence name length.
    int maxNameLength = 0;
    foreach (const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        maxNameLength = qMax(maxNameLength, row->getName().length());
    }
    maxNameLength = qMin(maxNameLength, MAX_NAME_LEN);

    int aliLen = msa->getLength();
    QByteArray consensus(aliLen, U2Msa::GAP_CHAR);

    MSAConsensusAlgorithmFactory* algoFactory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(BuiltInConsensusAlgorithms::CLUSTAL_ALGO);
    QScopedPointer<MSAConsensusAlgorithm> algo(algoFactory->createAlgorithm(msa));
    MSAConsensusUtils::updateConsensus(msa, consensus, algo.data());

    int maxNumLength = 1 + (aliLen < 10 ? 1 : (int)log10((double)aliLen));

    int seqStart = maxNameLength + 2;  //+1 for space separator
    if (seqStart % SEQ_ALIGNMENT != 0) {
        seqStart = seqStart + SEQ_ALIGNMENT - (seqStart % SEQ_ALIGNMENT);
    }
    int seqEnd = MAX_LINE_LEN - maxNumLength - 1;
    if (seqEnd % SEQ_ALIGNMENT != 0) {
        seqEnd = seqEnd - (seqEnd % SEQ_ALIGNMENT);
    }
    seqEnd = qMin(seqEnd, seqStart + MAX_SEQ_LEN);
    assert(seqStart % SEQ_ALIGNMENT == 0 && seqEnd % SEQ_ALIGNMENT == 0 && seqEnd > seqStart);

    int seqPerPage = seqEnd - seqStart;
    const char* spaces = TextUtils::SPACE_LINE.constData();

    // Write sequence.
    MultipleSequenceAlignmentWalker walker(msa);
    for (int i = 0; i < aliLen; i += seqPerPage) {
        int partLen = i + seqPerPage > aliLen ? aliLen - i : seqPerPage;
        QList<QByteArray> seqs = walker.nextData(partLen, os);
        CHECK_OP(os, );
        QList<QByteArray>::ConstIterator si = seqs.constBegin();
        QList<MultipleSequenceAlignmentRow> rows = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = rows.constBegin();
        for (; si != seqs.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow& row = *ri;
            // Name.
            QString line = row->getName();
            if (line.length() > MAX_NAME_LEN) {
                line = line.left(MAX_NAME_LEN);
            }

            // Separator.
            TextUtils::replace(line, TextUtils::WHITES, '_');
            line.append(QByteArray(spaces, seqStart - line.length()));

            // Sequence.
            QByteArray sequence = *si;
            line.append(sequence);
            line.append(' ');
            line.append(QString::number(qMin(i + seqPerPage, aliLen)));
            assert(line.length() <= MAX_LINE_LEN);
            line.append('\n');

            writer.write(os, line);
            CHECK_OP(os, );
        }

        // Write consensus.
        QByteArray line = QByteArray(spaces, seqStart);
        line.append(consensus.mid(i, partLen));
        line.append("\n\n");
        writer.write(os, line);
        CHECK_OP(os, );
    }
}

void ClustalWAlnFormat::storeTextDocument(IOAdapterWriter& writer, Document* d, U2OpStatus& os) {
    CHECK_EXT(d != nullptr, os.setError(L10N::badArgument("doc")), );

    const QList<GObject*>& objectList = d->getObjects();
    CHECK_EXT(objectList.size() == 1, (objectList.isEmpty() ? tr("No data to write") : tr("Too many objects: %1").arg(objectList.size())), );

    auto obj = qobject_cast<MultipleSequenceAlignmentObject*>(objectList.first());
    CHECK_EXT(obj != nullptr, os.setError(tr("Not a multiple alignment object")), );

    QMap<GObjectType, QList<GObject*>> objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = objectList;
    storeTextEntry(writer, objectsMap, os);
}

FormatCheckResult ClustalWAlnFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    if (!dataPrefix.startsWith(CLUSTAL_HEADER)) {
        return FormatDetection_NotMatched;
    }
    QString line = TextUtils::readFirstLine(dataPrefix);
    return line == CLUSTAL_HEADER || line.endsWith("multiple sequence alignment")
               ? FormatDetection_Matched
               : FormatDetection_AverageSimilarity;
}

}  // namespace U2
