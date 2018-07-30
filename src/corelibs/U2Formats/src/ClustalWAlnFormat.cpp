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

#include <math.h>

#include <QTextStream>

#include <U2Algorithm/BuiltInConsensusAlgorithms.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSAConsensusUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/MultipleSequenceAlignmentWalker.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include "ClustalWAlnFormat.h"

namespace U2 {

/* TRANSLATOR U2::ClustalWAlnFormat */
/* TRANSLATOR U2::IOAdapter */

const QByteArray ClustalWAlnFormat::CLUSTAL_HEADER = "CLUSTAL";

ClustalWAlnFormat::ClustalWAlnFormat(QObject* p) : TextDocumentFormat(p, DocumentFormatFlags(DocumentFormatFlag_SupportWriting) | DocumentFormatFlag_OnlyOneObject, QStringList("aln"))
{
    formatName = tr("CLUSTALW");
    formatDescription = tr("Clustalw is a format for storing multiple sequence alignments");
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
}

void ClustalWAlnFormat::load(IOAdapter* io, const U2DbiRef& dbiRef, QList<GObject*>& objects, const QVariantMap& fs, U2OpStatus& os) {
    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff  = readBuffer.data();

    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;
    const QBitArray& WHITES = TextUtils::WHITES;

    QString objName = io->getURL().baseFileName();
    MultipleSequenceAlignment al(objName);
    bool lineOk = false;
    bool firstBlock = true;
    int sequenceIdx = 0;
    int valStartPos = 0;
    int valEndPos = 0;
    int currentLen = 0;

    //1 skip first line
    int len = io->readUntil(buff, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
    if (!lineOk || !readBuffer.startsWith( CLUSTAL_HEADER )) {
        os.setError( ClustalWAlnFormat::tr("Illegal header line"));
    }

    //read data
    while (!os.isCoR() && (len = io->readUntil(buff, READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk)) > 0) {
        if( QByteArray::fromRawData( buff, len ).startsWith( CLUSTAL_HEADER ) ) {
            io->skip( -len );
            break;
        }
        int numNs = 0;
        while(len > 0 && LINE_BREAKS[(uchar)buff[len-1]]) {
            if ( buff[len-1] =='\n') {
                numNs++;
            }
            len--;
        }
        if (len == 0) {
            if (al->getNumRows() == 0) {
                continue;//initial empty lines
            }
            os.setError( ClustalWAlnFormat::tr("Error parsing file"));
            break;
        }

        QByteArray line = QByteArray(buff, len);
        if (valStartPos == 0) {
            int spaceIdx = line.indexOf(' ');
            int valIdx = spaceIdx + 1;
            while (valIdx < len && WHITES[(uchar)buff[valIdx]]) {
                valIdx++;
            }
            if (valIdx <= 0 || valIdx >= len-1) {
                os.setError( ClustalWAlnFormat::tr("Invalid alignment format"));
                break;
            }
            valStartPos = valIdx;
        }

        valEndPos = valStartPos + 1; //not inclusive
        while (valEndPos < len && !WHITES[(uchar)buff[valEndPos]]) {
            valEndPos++;
        }
        if (valEndPos!=len) { //there were numbers trimmed -> trim spaces now
            while (valEndPos > valStartPos && buff[valEndPos] == ' ') {
                valEndPos--;
            }
            valEndPos++; //leave non-inclusive
        }

        QByteArray name = line.left(valStartPos).trimmed();
        QByteArray value = line.mid(valStartPos, valEndPos - valStartPos);

        int seqsInModel = al->getNumRows();
        bool lastBlockLine = (!firstBlock && sequenceIdx == seqsInModel)
            || numNs >=2
            || name.isEmpty()
            || value.contains(' ') || value.contains(':') || value.contains('.');

        if (firstBlock) {
            if (lastBlockLine && name.isEmpty()) { //if name is not empty -> this is a sequence but consensus (for Clustal files without consensus)
                // this is consensus line - skip it
            } else {
                assert(al->getNumRows() == sequenceIdx);
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
                os.setError( ClustalWAlnFormat::tr("Incorrect number of sequences in block"));
                break;
            }
            if (rowIdx != -1) {
                const MultipleSequenceAlignmentRow row = al->getMsaRow(rowIdx);
                if (row->getName() != name) {
                    os.setError( ClustalWAlnFormat::tr("Sequence names are not matched"));
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

        os.setProgress(io->getProgress());
    }
    MSAUtils::checkPackedModelSymmetry(al, os);
    if (os.hasError()) {
        return;
    }
    U2AlphabetUtils::assignAlphabet(al);
    CHECK_EXT(al->getAlphabet()!=NULL, os.setError( ClustalWAlnFormat::tr("Alphabet is unknown")), );

    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    MultipleSequenceAlignmentObject* obj = MultipleSequenceAlignmentImporter::createAlignment(dbiRef, folder, al, os);
    CHECK_OP(os, );
    objects.append(obj);
}

Document* ClustalWAlnFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList<GObject*> objects;
    load(io, dbiRef, objects, fs, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);
    assert(objects.size() == 1);
    return new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs);
}

#define MAX_LINE_LEN    80
#define MAX_NAME_LEN    39
#define SEQ_ALIGNMENT    5

void ClustalWAlnFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &ti) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT), "Clustal entry storing: no alignment", );
    const QList<GObject*> &als = objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT];
    SAFE_POINT(1 == als.size(), "Clustal entry storing: alignment objects count error", );

    const MultipleSequenceAlignmentObject* obj = dynamic_cast<MultipleSequenceAlignmentObject*>(als.first());
    SAFE_POINT(NULL != obj, "Clustal entry storing: NULL alignment object", );

    const MultipleSequenceAlignment msa = obj->getMultipleAlignment();

    //write header
    QByteArray header("CLUSTAL W 2.0 multiple sequence alignment\n\n");
    int len = io->writeBlock(header);
    if (len != header.length()) {
        ti.setError(L10N::errorTitle());
        return;
    }

    //precalculate seq writing params
    int maxNameLength = 0;
    foreach(const MultipleSequenceAlignmentRow& row, msa->getMsaRows()) {
        maxNameLength = qMax(maxNameLength, row->getName().length());
    }
    maxNameLength = qMin(maxNameLength, MAX_NAME_LEN);

    int aliLen = msa->getLength();
    QByteArray consensus(aliLen, U2Msa::GAP_CHAR);

    MSAConsensusAlgorithmFactory* algoFactory = AppContext::getMSAConsensusAlgorithmRegistry()->getAlgorithmFactory(BuiltInConsensusAlgorithms::CLUSTAL_ALGO);
    QScopedPointer<MSAConsensusAlgorithm> algo(algoFactory->createAlgorithm(msa));
    MSAConsensusUtils::updateConsensus(msa, consensus, algo.data());

    int maxNumLength  = 1 + (aliLen < 10 ? 1 : (int)log10((double)aliLen));

    int seqStart = maxNameLength + 2; //+1 for space separator
    if (seqStart % SEQ_ALIGNMENT != 0) {
        seqStart = seqStart + SEQ_ALIGNMENT - (seqStart % SEQ_ALIGNMENT);
    }
    int seqEnd = MAX_LINE_LEN - maxNumLength - 1;
    if (seqEnd % SEQ_ALIGNMENT != 0) {
        seqEnd = seqEnd - (seqEnd % SEQ_ALIGNMENT);
    }
    assert(seqStart % SEQ_ALIGNMENT == 0 && seqEnd % SEQ_ALIGNMENT == 0 && seqEnd > seqStart);

    int seqPerPage = seqEnd - seqStart;
    const char* spaces = TextUtils::SPACE_LINE.constData();

    //write sequence
    U2OpStatus2Log os;
    MultipleSequenceAlignmentWalker walker(msa);
    for(int i = 0; i < aliLen; i+=seqPerPage) {
        int partLen = i + seqPerPage > aliLen ? aliLen - i : seqPerPage;
        QList<QByteArray> seqs = walker.nextData(partLen, os);
        CHECK_OP(os, );
        QList<QByteArray>::ConstIterator si = seqs.constBegin();
        QList<MultipleSequenceAlignmentRow> rows = msa->getMsaRows();
        QList<MultipleSequenceAlignmentRow>::ConstIterator ri = rows.constBegin();
        for (; si != seqs.constEnd(); si++, ri++) {
            const MultipleSequenceAlignmentRow &row = *ri;
            QByteArray line = row->getName().toLatin1();
            if (line.length() > MAX_NAME_LEN) {
                line = line.left(MAX_NAME_LEN);
            }
            TextUtils::replace(line.data(), line.length(), TextUtils::WHITES, '_');
            line.append(QByteArray(spaces, seqStart - line.length()));
            line.append(*si);
            line.append(' ');
            line.append(QString::number(qMin(i+seqPerPage, aliLen)));
            assert(line.length() <= MAX_LINE_LEN);
            line.append('\n');

            len = io->writeBlock(line);
            if (len != line.length()) {
                ti.setError(L10N::errorTitle());
                return;
            }
        }
        //write consensus
        QByteArray line = QByteArray(spaces, seqStart);
        line.append(consensus.mid(i, partLen));
        line.append("\n\n");
        len = io->writeBlock(line);
        if (len != line.length()) {
            ti.setError(L10N::errorTitle());
            return;
        }
    }
}

void ClustalWAlnFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    CHECK_EXT(d!=NULL, os.setError(L10N::badArgument("doc")), );
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), );

    MultipleSequenceAlignmentObject *obj = NULL;
    if( (d->getObjects().size() != 1)
        || ((obj = qobject_cast<MultipleSequenceAlignmentObject*>(d->getObjects().first())) == NULL)) {
            os.setError("No data to write;");
            return;
    }

    QList<GObject*> als; als << obj;
    QMap< GObjectType, QList<GObject*> > objectsMap;
    objectsMap[GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT] = als;
    storeEntry(io, objectsMap, os);
    CHECK_EXT(!os.isCoR(), os.setError(L10N::errorWritingFile(d->getURL())), );
}

FormatCheckResult ClustalWAlnFormat::checkRawTextData(const QByteArray& data, const GUrl&) const {
    if (TextUtils::contains(TextUtils::BINARY, data.constData(), data.size())) {
        return FormatDetection_NotMatched;
    }
    if (!data.startsWith(CLUSTAL_HEADER)) {
        return FormatDetection_NotMatched;
    }
    QTextStream s(data);
    QString line = s.readLine();
    if ( (line == CLUSTAL_HEADER) || (line.endsWith("multiple sequence alignment")) ) {
        return FormatDetection_Matched;
    }
    return FormatDetection_AverageSimilarity;
}

}//namespace
