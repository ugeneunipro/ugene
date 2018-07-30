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

#include <QTextStream>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "FastaFormat.h"

namespace U2 {

/* TRANSLATOR U2::FastaFormat */
/* TRANSLATOR U2::IOAdapter */
/* TRANSLATOR U2::Document */

const char FastaFormat::FASTA_HEADER_START_SYMBOL = '>';
const char FastaFormat::FASTA_COMMENT_START_SYMBOL = ';';

FastaFormat::FastaFormat(QObject* p)
: TextDocumentFormat(p, DocumentFormatFlags_SW, QStringList()<<"fa"<<"mpfa"<<"fna"<<"fsa"<<"fas"<<"fasta"<<"sef"<<"seq"<<"seqs")
{
    formatName = tr("FASTA");
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    supportedObjectTypes+=GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatDescription = tr("FASTA format is a text-based format for representing either nucleotide sequences or peptide sequences, "
        "in which base pairs or amino acids are represented using single-letter codes. "
        "The format also allows for sequence names and comments to precede the sequences.");
}

static QVariantMap analyzeRawData(const QByteArray& data) {
    int hasGaps = false;
    int minLen = -1;
    int maxLen = -1;
    int len = 0;
    int nSequences = 0;

    QTextStream input(data, QIODevice::ReadOnly);
    QString line;
    do {
        line = input.readLine();
        if (line[0] == FastaFormat::FASTA_HEADER_START_SYMBOL) {
            nSequences++;
            if (len > 0) {
                minLen = minLen == -1 ? len : qMin(minLen, len);
                maxLen = maxLen == -1 ? len : qMax(maxLen, len);
            }
            len = 0;
        } else {
            len += line.length();
            if (!hasGaps && line.contains(U2Msa::GAP_CHAR)) {
                hasGaps = true;
            }
        }
    } while (!input.atEnd());

    QVariantMap res;
    res[RawDataCheckResult_Sequence] = true;
    if (hasGaps) {
        res[RawDataCheckResult_SequenceWithGaps] = true;
    }
    if (nSequences > 1) {
        res[RawDataCheckResult_MultipleSequences] = true;
    }
    if (minLen > 0) {
        res[RawDataCheckResult_MaxSequenceSize] = maxLen;
        res[RawDataCheckResult_MinSequenceSize] = minLen;
    }
    return res;
}

FormatCheckResult FastaFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();

    int n = TextUtils::skip(TextUtils::WHITES, data, size);
    int newSize = size - n;
    const char* newData = data + n;
    if (newSize <= 0 || (newData[0] != FASTA_HEADER_START_SYMBOL && newData[0] != FASTA_COMMENT_START_SYMBOL) ) {
        return FormatDetection_NotMatched;
    }
    bool hasBinaryBlocks = TextUtils::contains(TextUtils::BINARY, data, size);
    if (hasBinaryBlocks) {
        return FormatDetection_NotMatched;
    }

    //ok, format is matched -> add hints on sequence sizes
    FormatCheckResult res(FormatDetection_Matched);
    res.properties = analyzeRawData(data);
    return res;
}

static void load(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, QList<GObject*>& objects,
                 int gapSize, QString& writeLockReason, U2OpStatus& os)
{
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );
    Q_UNUSED(opBlock);

    static char fastaCommentStartChar = FastaFormat::FASTA_COMMENT_START_SYMBOL;

    MemoryLocker memoryLocker(os, 1);
    CHECK_OP(os, );

    writeLockReason.clear();
    QByteArray readBuff(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buff = readBuff.data();
    qint64 len = 0;

    bool merge = gapSize != -1;
    QStringList headers;
    QSet<QString> uniqueNames;
    QVector<U2Region> mergedMapping;

    // for lower case annotations
    GObjectReference sequenceRef;

    //skip leading whites if present
    bool lineOk = true;
    static QBitArray nonWhites = ~TextUtils::WHITES;
    io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, nonWhites, IOAdapter::Term_Exclude, &lineOk);

    U2SequenceImporter seqImporter(fs, true);
    const QString folder = fs.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    qint64 sequenceStart = 0;
    int sequenceNumber = 0;
    DbiConnection con(dbiRef, os);
    bool headerReaded = false;
    QStringList emptySeqNames;

    const int objectsCountLimit = fs.contains(DocumentReadingMode_MaxObjectsInDoc) ? fs[DocumentReadingMode_MaxObjectsInDoc].toInt() : -1;
    const bool settingsMakeUniqueName = !fs.value(DocumentReadingMode_DontMakeUniqueNames, false).toBool();
    while (!os.isCoR()) {
        //skip start comments and read header
        if(!headerReaded){
            do{
                len = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE);
            }while(buff[0] == fastaCommentStartChar && len > 0);
        }

        if (len == 0 && io->isEof()) { //end if stream
            break;
        }
        CHECK_EXT_BREAK(lineOk, os.setError(FastaFormat::tr("Line is too long")));

        QString headerLine = QString(QByteArray(buff+1, len-1)).trimmed();
        CHECK_EXT_BREAK(buff[0] == FastaFormat::FASTA_HEADER_START_SYMBOL, os.setError(FastaFormat::tr("First line is not a FASTA header")));

        //read sequence
        if (sequenceNumber == 0 || !merge) {
            QString objName = headerLine;
            if(objName.isEmpty()){
                objName = "Sequence";
            }
            if (settingsMakeUniqueName) {
                objName = (merge) ? "Sequence" : TextUtils::variate(objName, "_", uniqueNames);
                objName.squeeze();
                memoryLocker.tryAcquire(2*objName.size());
                CHECK_OP_BREAK(os);
                uniqueNames.insert(objName);
            }
            seqImporter.startSequence(os, dbiRef, folder, objName, false);
            CHECK_OP_BREAK(os);

            sequenceRef = GObjectReference(io->getURL().getURLString(), objName, GObjectTypes::SEQUENCE);
        }
        if (sequenceNumber >= 1 && merge) {
            seqImporter.addDefaultSymbolsBlock(gapSize, os);
            sequenceStart += gapSize;
            CHECK_OP_BREAK(os);
        }
        int sequenceLen = 0;
        while (!os.isCoR()) {
            do{
                len = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE);
            }while(len <= 0 && !io->isEof());

            if (len <= 0 && io->isEof()) {
                break;
            }
            buff[len] = 0;

            if(buff[0] != fastaCommentStartChar && buff[0] != FastaFormat::FASTA_HEADER_START_SYMBOL){
                len = TextUtils::remove(buff, len, TextUtils::WHITES);
                if(len > 0){
                    seqImporter.addBlock(buff, len, os);
                    sequenceLen += len;
                }
            }else if( buff[0] == FastaFormat::FASTA_HEADER_START_SYMBOL){
                headerReaded = true;
                break;
            }

            CHECK_OP_BREAK(os);
            os.setProgress(io->getProgress());
        }

        if (merge) {
            memoryLocker.tryAcquire(headerLine.size());
            CHECK_OP_BREAK(os);
            headers.append(headerLine);
            mergedMapping.append(U2Region(sequenceStart, sequenceLen));
        } else {
            if (objectsCountLimit > 0 && objects.size() >= objectsCountLimit) {
                os.setError(FastaFormat::tr("File \"%1\" contains too many sequences to be displayed. "
                    "However, you can process these data using instruments from the menu <i>Tools -> NGS data analysis</i> "
                    "or pipelines built with Workflow Designer.")
                    .arg(io->getURL().getURLString()));
                break;
            }
            memoryLocker.tryAcquire(800);
            CHECK_OP_BREAK(os);
            U2Sequence seq = seqImporter.finalizeSequenceAndValidate(os);
            if (os.hasError() && os.getError() == U2SequenceImporter::EMPTY_SEQUENCE_ERROR) {
                os.setError("");
                emptySeqNames << headerLine;
                continue;
            }
            sequenceRef.entityRef = U2EntityRef(dbiRef, seq.id);

            //TODO parse header
            U2StringAttribute attr(seq.id, DNAInfo::FASTA_HDR, headerLine);
            con.dbi->getAttributeDbi()->createStringAttribute(attr, os);
            CHECK_OP_BREAK(os);

            objects << new U2SequenceObject(seq.visualName, U2EntityRef(dbiRef, seq.id));
            CHECK_OP_BREAK(os);

            U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, fs);
        }
        sequenceStart += sequenceLen;
        sequenceNumber++;
        ioLog.trace(QString("Sequence #%1 is processed").arg(sequenceNumber));
    }

    CHECK_OP_EXT(os, qDeleteAll(objects); objects.clear(), );
    CHECK_EXT(!objects.isEmpty() || merge, os.setError(Document::tr("Document is empty.")), );
    SAFE_POINT(headers.size() == mergedMapping.size(), "headers <-> regions mapping failed!", );
    ioLog.trace("All sequences are processed");

    if (!emptySeqNames.isEmpty()) {
        QString warningMessage;
        warningMessage.append(FastaFormat::tr("Loaded sequences: %1.\n").arg(sequenceNumber));
        warningMessage.append(FastaFormat::tr("Skipped sequences: %1.\n").arg(emptySeqNames.size()));
        warningMessage.append(FastaFormat::tr("The following sequences are empty:\n%1").arg(emptySeqNames.join(",\n")));
        os.addWarning(warningMessage);
    }

    if (!merge) {
        return;
    }

    U2Sequence seq = seqImporter.finalizeSequenceAndValidate(os);
    CHECK_OP(os, );
    sequenceRef.entityRef = U2EntityRef(dbiRef, seq.id);

    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, fs);
    objects << new U2SequenceObject(seq.visualName, U2EntityRef(dbiRef, seq.id));
    objects << DocumentFormatUtils::addAnnotationsForMergedU2Sequence( sequenceRef, dbiRef, headers, mergedMapping, fs );
    if (headers.size() > 1) {
        writeLockReason = DocumentFormat::MERGED_SEQ_LOCK;
    }
}


Document* FastaFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    CHECK_EXT(io!=NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);

    QList<GObject*> objects;

    int gapSize = qBound(-1, DocumentFormatUtils::getMergeGap(fs), 1000 * 1000);

    QString lockReason;
    load(io, dbiRef, fs, objects, gapSize, lockReason, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, fs, lockReason);
    return doc;
}

static void writeHeaderToFile( IOAdapter* io, const QString &sequenceName, U2OpStatus &os ) {
    QByteArray block;
    block.append( FastaFormat::FASTA_HEADER_START_SYMBOL ).append( sequenceName ).append( '\n' );
    if ( io->writeBlock( block ) != block.length( ) ) {
        os.setError( L10N::errorWritingFile( io->getURL( ) ) );
    }
}

static void writeBlockToFile( IOAdapter* io, const char *block, int blockSize, U2OpStatus &os ) {
    if ( io->writeBlock( block, blockSize ) != blockSize || !io->writeBlock( "\n", 1 ) ) {
        os.setError( L10N::errorWritingFile( io->getURL( ) ) );
    }
}

#define SAVE_LINE_LEN 1024 * 1024

static void saveSequence( IOAdapter* io, const U2SequenceObject *sequence, U2OpStatus& os ) {
    writeHeaderToFile( io, sequence->getSequenceName( ), os );
    CHECK_OP( os, );

    const int len = sequence->getSequenceLength( );
    for ( int i = 0; i < len; i += SAVE_LINE_LEN ) {
        int chunkSize = qMin( SAVE_LINE_LEN, len - i );
        const QByteArray chunkContent = sequence->getSequenceData( U2Region( i, chunkSize ), os );
        CHECK_OP( os, );
        writeBlockToFile( io, chunkContent.constData( ), chunkSize, os );
        CHECK_OP( os, );
    }
}

static void saveSequence(IOAdapter* io, const DNASequence& sequence, U2OpStatus& os) {
    writeHeaderToFile( io, sequence.getName( ), os );
    CHECK_OP( os, );

    const char *seq = sequence.seq.constData( );
    const int len = sequence.seq.length( );
    for ( int i = 0; i < len; i += SAVE_LINE_LEN ) {
        const int chunkSize = qMin( SAVE_LINE_LEN, len - i );
        writeBlockToFile( io, seq + i, chunkSize, os );
        CHECK_OP( os, );
    }
}

void FastaFormat::storeDocument( Document* doc, IOAdapter* io, U2OpStatus& os ) {
    //TODO: check saved op states!!!
    foreach( GObject *o, doc->getObjects( ) ) {
        U2SequenceObject *seqObj = dynamic_cast<U2SequenceObject *>( o );
        if ( NULL != seqObj ) {
            saveSequence( io, seqObj, os );
            CHECK_OP( os, );
        } else {
            QList<DNASequence> sequences = DocumentFormatUtils::toSequences( o );
            foreach( const DNASequence &s, sequences ) {
                saveSequence( io, s, os );
                CHECK_OP( os, );
            }
        }
    }
}

void FastaFormat::storeEntry( IOAdapter *io, const QMap<GObjectType, QList<GObject *> > &objectsMap,
    U2OpStatus &os )
{
    SAFE_POINT( objectsMap.contains( GObjectTypes::SEQUENCE ), "Fasta entry storing: no sequences", );
    const QList<GObject *> &seqs = objectsMap[GObjectTypes::SEQUENCE];
    SAFE_POINT( 1 == seqs.size( ), "Fasta entry storing: sequence objects count error", );

    U2SequenceObject *seq = dynamic_cast<U2SequenceObject *>( seqs.first( ) );
    SAFE_POINT( NULL != seq, "Fasta entry storing: NULL sequence object", );
    saveSequence(io, seq, os);
}

DNASequence *FastaFormat::loadTextSequence(IOAdapter* io, U2OpStatus& os) {
    try {
        MemoryLocker l(os);
        CHECK_OP(os, NULL);

        static QBitArray fastaHeaderStart = TextUtils::createBitMap(FASTA_HEADER_START_SYMBOL);
        static QBitArray nonWhites = ~TextUtils::WHITES;

        CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);

        QByteArray readBuff(READ_BUFF_SIZE+1, 0);
        char* buff = readBuff.data();
        qint64 len = 0;

        //skip leading whites if present
        bool lineOk = true;
        io->readUntil(buff, READ_BUFF_SIZE, nonWhites, IOAdapter::Term_Exclude, &lineOk);

        //read header
        len = io->readUntil(buff, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &lineOk);
        CHECK(len > 0, NULL); //end of stream
        CHECK_EXT(lineOk, os.setError(FastaFormat::tr("Line is too long")), NULL);
        QByteArray headerLine = QByteArray(buff + 1, len-1).trimmed();
        CHECK_EXT(buff[0] == FASTA_HEADER_START_SYMBOL, os.setError(FastaFormat::tr("First line is not a FASTA header")), NULL);

        l.tryAcquire(headerLine.capacity());
        CHECK_OP(os, NULL);

        //read sequence
        QByteArray sequence;
        int predictedSize = 1000;
        sequence.reserve(predictedSize);

        do {
            len = io->readUntil(buff, READ_BUFF_SIZE, fastaHeaderStart, IOAdapter::Term_Exclude);
            if (len <= 0) {
                break;
            }
            len = TextUtils::remove(buff, len, TextUtils::WHITES);
            buff[len] = 0;

            l.tryAcquire(len);
            CHECK_OP(os, NULL);

            sequence.append(buff);
        } while (!os.isCoR());
        sequence.squeeze();

        DNASequence *seq = new DNASequence(headerLine, sequence);
        seq->alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
        assert(seq->alphabet!=NULL);

        if (!seq->alphabet->isCaseSensitive()) {
            TextUtils::translate(TextUtils::UPPER_CASE_MAP, const_cast<char*>(seq->seq.constData()), seq->seq.length());
        }

        return seq;
    } catch (...) {
        os.setError("Memory error");
        return NULL;
    }
}

void FastaFormat::storeSequence(const DNASequence& sequence, IOAdapter* io, U2OpStatus& os) {
    saveSequence(io, sequence, os);
}

void FastaFormat::storeSequence( const U2SequenceObject *sequence, IOAdapter *io, U2OpStatus &os ) {
    saveSequence( io, sequence, os );
}

static QString skipComments(const QString &userInput, U2OpStatus &os) {
    QStringList lines = userInput.trimmed().split("\n", QString::SkipEmptyParts);
    QStringList result = lines;
    QStringList unreferenced;
    foreach (const QString &line, lines) {
        if (line.startsWith(FastaFormat::FASTA_HEADER_START_SYMBOL)) {
            break;
        } else {
            result.removeFirst();
        }

        if (!line.startsWith(FastaFormat::FASTA_COMMENT_START_SYMBOL)) {
            unreferenced << line;
        }
    }

    if (!unreferenced.isEmpty()) {
        QString seq = unreferenced.join(" ");
        os.setError(FastaFormat::tr("Unreferenced sequence in the beginning of patterns: %1").arg(seq));
    }

    return result.join("\n");
}

QList <QPair<QString, QString> > FastaFormat::getSequencesAndNamesFromUserInput( const QString &userInput, U2OpStatus &os ){
    QList <QPair<QString, QString> > result;

    if(userInput.contains(FASTA_HEADER_START_SYMBOL)) {
        QString patterns = skipComments(userInput, os);
        QStringList seqDefs = patterns.trimmed().split(FASTA_HEADER_START_SYMBOL, QString::SkipEmptyParts);

        foreach (const QString &seqDef, seqDefs) {
            QStringList seqData = seqDef.split("\n");
            CHECK_EXT(!seqData.isEmpty(), os.setError("Invalid fasta input"), result);
            QString name = seqData.takeFirst();
            QString sequence;
            foreach (const QString &line, seqData) {
                if (line.startsWith(FASTA_COMMENT_START_SYMBOL)) {
                    continue;
                }
                sequence += line;
            }
            result.append(qMakePair(name, sequence));
        }
    }

    return result;
}

}//namespace
