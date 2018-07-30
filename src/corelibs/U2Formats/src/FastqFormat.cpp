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

#include <QtMath>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "FastqFormat.h"

/* TRANSLATOR U2::FastqFormat */

namespace U2 {

static const int PROGRESS_UPDATE_STEP = 1000;

FastqFormat::FastqFormat(QObject* p)
    : TextDocumentFormat(p, DocumentFormatFlags_SW | DocumentFormatFlag_LockedIfNotCreatedByUGENE, QStringList() << "fastq" << "fq"), fn(tr("FASTQ"))
{
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    formatDescription  = tr("FASTQ format is a text-based format for storing both a biological sequence (usually nucleotide sequence) "
        "and its corresponding quality scores. \
        Both the sequence letter and quality score are encoded with a single ASCII character for brevity. \
        It was originally developed at the Wellcome Trust Sanger Institute to bundle a FASTA sequence and its quality data, \
        but has recently become the de facto standard for storing the output of high throughput sequencing instruments.");
}


#define STATE_START_PARSING     44
#define STATE_QUALITY_HEADER    45
#define STATE_SEQ_HEADER        46
#define STATE_SEQ               47
#define STATE_QUALITY           48

FormatCheckResult FastqFormat::checkRawTextData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();

    int sequenceCount = 0;
    int qualCount = 0;
    int state = STATE_START_PARSING;

    QList<QByteArray> lines = rawData.split('\n');

    int lastSequenceLength = 0;
    int lastQualityLength = 0;
    foreach (const QByteArray& line, lines) {
        if (line.isEmpty()) {
            continue;
        }
        if (line.startsWith('@') &&
            ((STATE_START_PARSING == state)  ||
            (STATE_QUALITY == state && lastQualityLength == lastSequenceLength))
            && (line.length() > 1) && QChar(line.at(1)).isLetterOrNumber()) {
            sequenceCount++;
            state = STATE_SEQ_HEADER;
            lastSequenceLength = 0;
        } else if(line.startsWith('+') && STATE_SEQ == state){
            qualCount++;
            state = STATE_QUALITY_HEADER;
            lastQualityLength = 0;
        } else if (STATE_SEQ_HEADER == state || STATE_SEQ == state) {
            if (!QChar(line[0]).isLetter()) {
                return FormatDetection_NotMatched;
            }
            state = STATE_SEQ;
            lastSequenceLength += line.length();
        } else if (STATE_QUALITY_HEADER == state || STATE_QUALITY == state) {
            state = STATE_QUALITY;
            lastQualityLength += line.length();
        } else {
            return FormatDetection_NotMatched;
        }
    }

    bool hasBinaryBlocks = TextUtils::contains(TextUtils::BINARY, data, size);
    if (hasBinaryBlocks || (sequenceCount == 0)) {
        return FormatDetection_NotMatched;
    }

    //check thats every seq had its own qual
    if (STATE_QUALITY_HEADER == state || STATE_QUALITY == state){
        if (sequenceCount != qualCount) {
            return FormatDetection_NotMatched;
        }
    }else{
        if((sequenceCount - 1) != qualCount){
            return FormatDetection_NotMatched;
        }
    }

    FormatCheckResult res(FormatDetection_HighSimilarity);
    res.properties[RawDataCheckResult_Sequence] = true;
    res.properties[RawDataCheckResult_MultipleSequences] = sequenceCount > 1;
    res.properties[RawDataCheckResult_SequenceWithGaps] = false;
    res.properties[RawDataCheckResult_MinSequenceSize] = 10;
    res.properties[RawDataCheckResult_MaxSequenceSize] = 1000;
    return res;
}

static QString readSequenceName(U2OpStatus& os, IOAdapter *io, char beginWith = '@') {
    static const QString errorMessage = U2::FastqFormat::tr("Error while trying to find sequence name start");

    QByteArray buffArray(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    { // read name string
        char *buff = buffArray.data();
        bool sequenceNameStartFound = false;
        int readedCount = 0;
        while ((readedCount == 0) && !io->isEof()) { // skip \ns
            readedCount = io->readLine(buff, DocumentFormat::READ_BUFF_SIZE, &sequenceNameStartFound);
        }
        CHECK_EXT(io->isEof() == false,,"");
        CHECK_EXT(readedCount >= 0, os.setError(errorMessage), "");

        buffArray.resize(readedCount);
        buffArray = buffArray.trimmed();
    }

    const char *buff = buffArray.constData();
    CHECK_EXT((buffArray.size() > 0) && (buff[0] == beginWith), os.setError(errorMessage), "");

    QString sequenceName = QString::fromLatin1(buff+1, buffArray.size()-1);
    return sequenceName;
}

static bool checkFirstSymbol(const QByteArray& b, char symbol) {
    return (b.size()>0 && b.data()[0] == symbol);
}

static void readSequence(U2OpStatus& os, IOAdapter *io, QByteArray &sequence, char readUntil = '+') {

    QByteArray buffArray(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buff = buffArray.data();

    // reading until readUntil symbol i.e. quality or dna sequence name start, ignoring whitespace at the beginning and the end of lines

    while (!io->isEof()) {
        bool eolnFound = false;
        int readedCount = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &eolnFound);
        CHECK_EXT(readedCount >= 0, os.setError(U2::FastqFormat::tr("Error while reading sequence")),);

        QByteArray trimmed = QByteArray(buffArray.data(), readedCount);
        trimmed = trimmed.trimmed();

        if (eolnFound && checkFirstSymbol(trimmed, readUntil)) { // read quality sequence name line, reverting back
            io->skip(-readedCount);
            return;
        }

        sequence.append(trimmed);
        CHECK_OP(os,);
    }
}

static void readQuality(U2OpStatus& os, IOAdapter *io, QByteArray &sequence, int count) {

    QByteArray buffArray(DocumentFormat::READ_BUFF_SIZE + 1, 0);
    char* buff = buffArray.data();

    // reading quality sequence, ignoring whitespace at the beginning and the end of lines

    int readed = 0;
    while (!io->isEof() && (readed < count)) {
        bool eolnFound = false;
        int readedCount = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include, &eolnFound);
        CHECK_EXT(readedCount >= 0, os.setError(U2::FastqFormat::tr("Error while reading sequence")),);

        QByteArray trimmed = QByteArray(buffArray.data(), readedCount);
        trimmed = trimmed.trimmed();

        int qualitySize = sequence.size() + trimmed.size();
        if (eolnFound && (qualitySize > count)) { // read quality sequence name line, reverting back
            io->skip(-readedCount);
            return;
        }

        sequence.append(trimmed);
        CHECK_OP(os,);
    }
}

#define SKIPPED_LINES_ERRORS_LIMIT 50
static bool errorLoggingBreak(U2OpStatus& os, QMap<QString, QString>& skippedLines, const QString& seqName){
    if (os.isCoR()){
        if (skippedLines.size() < SKIPPED_LINES_ERRORS_LIMIT){
            skippedLines.insert(seqName, os.getError());
        }
        return true;
    }
    return false;
}

/**
 * FASTQ format specification: http://maq.sourceforge.net/fastq.shtml
 */
static void load(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& hints, QList<GObject*>& objects, U2OpStatus& os,
                 int gapSize, int predictedSize, QString& writeLockReason, QMap<QString, QString>& skippedLines) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );
    Q_UNUSED(opBlock);
    writeLockReason.clear();

    bool merge = gapSize!=-1;
    QByteArray sequence;
    QByteArray qualityScores;
    QStringList headers;
    QSet<QString> uniqueNames;

    QVector<U2Region> mergedMapping;
    QByteArray gapSequence((merge ? gapSize : 0), 0);
    sequence.reserve(predictedSize);
    qualityScores.reserve(predictedSize);

    // for lower case annotations
    GObjectReference sequenceRef;
    qint64 sequenceStart = 0;

    U2SequenceImporter seqImporter(hints, true);
    const QString folder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    int seqNumber = 0;
    int progressUpNum = 0;


    const int objectsCountLimit = hints.contains(DocumentReadingMode_MaxObjectsInDoc) ? hints[DocumentReadingMode_MaxObjectsInDoc].toInt() : -1;
    const bool settingsMakeUniqueName = !hints.value(DocumentReadingMode_DontMakeUniqueNames, false).toBool();
    while (!os.isCoR()) {
        U2OpStatus2Log warningOs;

        //read header
        QString sequenceName = readSequenceName(warningOs, io, '@');
        // check for eof while trying to read another FASTQ block
        if (io->isEof()) {
            break;
        }

        if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
            continue;
        }

        if(sequenceName.isEmpty()){
            sequenceName = "Sequence";
        }

        if ((merge == false) || (seqNumber == 0)) {
            QString objName = sequenceName;
            if (settingsMakeUniqueName) {
                objName = (merge) ? "Sequence" : TextUtils::variate(sequenceName, "_", uniqueNames);
                objName.squeeze();
                uniqueNames.insert(objName);
            }
            seqImporter.startSequence(warningOs, dbiRef, folder, objName, false);
            if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
                U2OpStatusImpl seqOs;
                seqImporter.finalizeSequenceAndValidate(seqOs);
                continue;
            }
        }

        //read sequence
        if (merge && sequence.length() > 0) {
            seqImporter.addDefaultSymbolsBlock(gapSize, warningOs);
            sequenceStart += sequence.length();
            sequenceStart+=gapSize;
            if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
                U2OpStatusImpl seqOs;
                seqImporter.finalizeSequenceAndValidate(seqOs);
                continue;
            }
        }

        sequence.clear();
        readSequence(warningOs, io, sequence);
        if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
            U2OpStatusImpl seqOs;
            seqImporter.finalizeSequenceAndValidate(seqOs);
            continue;
        }
        MemoryLocker lSequence(os, qCeil(sequence.size()/(1000*1000)));
        CHECK_OP_BREAK(os);
        Q_UNUSED(lSequence);

        seqImporter.addBlock(sequence.data(),sequence.length(), warningOs);
        if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
            U2OpStatusImpl seqOs;
            seqImporter.finalizeSequenceAndValidate(seqOs);
            continue;
        }

        QString qualSequenceName = readSequenceName(warningOs, io, '+');
        if (!qualSequenceName.isEmpty()) {
            static const QString err = U2::FastqFormat::tr("Sequence name differs from quality scores name: %1 and %2");
            if (sequenceName != qualSequenceName){
                warningOs.setError(err.arg(sequenceName).arg(qualSequenceName));
            }
            if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
                U2OpStatusImpl seqOs;
                seqImporter.finalizeSequenceAndValidate(seqOs);
                continue;
            }
        }

        // read qualities
        qualityScores.clear();
        readQuality(warningOs, io, qualityScores, sequence.size());
        if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
            U2OpStatusImpl seqOs;
            seqImporter.finalizeSequenceAndValidate(seqOs);
            continue;
        }

        static const QString err = U2::FastqFormat::tr("Bad quality scores: inconsistent size.");
        if(sequence.length() != qualityScores.length()){
            warningOs.setError(err);
        }
        if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
            U2OpStatusImpl seqOs;
            seqImporter.finalizeSequenceAndValidate(seqOs);
            continue;
        }


        seqNumber++;
        progressUpNum++;
        if (merge) {
            headers.append(sequenceName);
            mergedMapping.append(U2Region(sequenceStart, sequence.length() ));
        }
        else {
            if (objectsCountLimit > 0 && objects.size() >= objectsCountLimit) {
                os.setError(FastqFormat::tr("File \"%1\" contains too many sequences to be displayed. "
                    "However, you can process these data using instruments from the menu <i>Tools -> NGS data analysis</i> "
                    "or pipelines built with Workflow Designer.")
                    .arg(io->getURL().getURLString()));
                break;
            }

            U2Sequence u2seq = seqImporter.finalizeSequenceAndValidate(warningOs);
            if(errorLoggingBreak(warningOs, skippedLines, sequenceName)){
                continue;
            }
            sequenceRef = GObjectReference(io->getURL().getURLString(), u2seq.visualName, GObjectTypes::SEQUENCE, U2EntityRef(dbiRef, u2seq.id));

            U2SequenceObject* seqObj = new U2SequenceObject(u2seq.visualName, U2EntityRef(dbiRef, u2seq.id));
            CHECK_EXT_BREAK(seqObj != NULL, os.setError("U2SequenceObject is NULL"));
            seqObj->setQuality(DNAQuality(qualityScores));
            objects << seqObj;

            U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, hints);
        }
        if (PROGRESS_UPDATE_STEP == progressUpNum) {
            progressUpNum = 0;
            os.setProgress(io->getProgress());
        }
    }

    CHECK_OP_EXT(os, qDeleteAll(objects); objects.clear(), );
    bool emptyObjects = objects.isEmpty();
    CHECK_EXT(!emptyObjects || merge, os.setError(Document::tr("Document is empty.")), );
    SAFE_POINT(headers.size() == mergedMapping.size(), "headers <-> regions mapping failed!", );

    if (!merge) {
        return;
    }
    U2Sequence u2seq = seqImporter.finalizeSequenceAndValidate(os);
    CHECK_OP(os,);
    sequenceRef = GObjectReference(io->getURL().getURLString(), u2seq.visualName, GObjectTypes::SEQUENCE, U2EntityRef(dbiRef, u2seq.id));

    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, NULL, hints);
    objects << new U2SequenceObject(u2seq.visualName, U2EntityRef(dbiRef, u2seq.id));
    objects << DocumentFormatUtils::addAnnotationsForMergedU2Sequence(sequenceRef, dbiRef, headers, mergedMapping, hints);
    if (headers.size() > 1) {
        writeLockReason = DocumentFormat::MERGED_SEQ_LOCK;
    }
}

Document* FastqFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& _hints, U2OpStatus& os) {
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);
    QVariantMap hints = _hints;
    QList<GObject*> objects;
    QMap<QString, QString> skippedLines;

    int gapSize = qBound(-1, DocumentFormatUtils::getMergeGap(_hints), 1000*1000);
    int predictedSize = qMax(100*1000, DocumentFormatUtils::getMergedSize(hints, gapSize==-1 ? 0 : io->left()));

    QString lockReason;
    load(io, dbiRef, _hints, objects, os, gapSize, predictedSize, lockReason, skippedLines);
    if (skippedLines.size() > 0){
        QMapIterator<QString, QString> i(skippedLines);
        QStringList errors;
        while (i.hasNext()) {
            i.next();
            QString msg = i.key() + ": " + i.value();
            if (objects.length() > 0){
                os.addWarning(msg);
            }else{
                errors.append(msg);
            }
        }
        if (errors.length() > 0){
            os.setError(errors.join("\n"));
        }
    }

    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);
    DocumentFormatUtils::updateFormatHints(objects, hints);
    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects, hints, lockReason);

    return doc;
}

void FastqFormat::storeDocument( Document* d, IOAdapter* io, U2OpStatus& os) {
    foreach (GObject* obj, d->getObjects()) {
        U2SequenceObject* seqObj = qobject_cast< U2SequenceObject* >( obj);
        if( seqObj == NULL ) {
            continue;
        }
        QList<GObject*> seqs;
        seqs << seqObj;
        QMap< GObjectType, QList<GObject*> > objectsMap;
        objectsMap[GObjectTypes::SEQUENCE] = seqs;

        storeEntry(io, objectsMap, os);
        CHECK_OP(os, );
    }
}

void writeSequence(U2OpStatus &os, IOAdapter *io, const char* seq, int len, const QString& errorMessage, bool cutLines = true) {

    CHECK_EXT(io != NULL, os.setError("can't write sequence"),);

    static const int lineLength = 80;

    if (cutLines){
        for (int i = 0; i < len; i += lineLength ) {
            int chunkSize = qMin(lineLength, len - i);

            QByteArray toWrite(seq + i, chunkSize);
            int writtenCount = io->writeBlock(toWrite);
            int writtenEoln = io->writeBlock("\n", 1);
            CHECK_EXT((writtenCount == chunkSize) && (writtenEoln > 0), os.setError(errorMessage),);
        }
    }else{
        QByteArray toWrite(seq, len);
        int writtenCount = io->writeBlock(toWrite);
        int writtenEoln = io->writeBlock("\n", 1);
        CHECK_EXT((writtenCount == len) && (writtenEoln > 0), os.setError(errorMessage),);
    }
}

void FastqFormat::writeEntry(const QString& sequenceName, const DNASequence& wholeSeq, IOAdapter *io, const QString& errorMessage, U2OpStatus &os, bool cutLines){
    QByteArray block;

    block.append('@').append(sequenceName).append('\n');

    int writtenCount = io->writeBlock(block);
    CHECK_EXT(writtenCount == block.length(), os.setError(errorMessage),);

    // write sequence
    writeSequence(os, io, wholeSeq.constData(), wholeSeq.length(), errorMessage, cutLines);

    //write transition
    block.clear();
    block.append("+\n");

    writtenCount = io->writeBlock(block);
    CHECK_EXT(writtenCount == block.length(), os.setError(errorMessage),);

    //write quality
    QByteArray buf;
    const char* qualityData = NULL;
    if (wholeSeq.hasQualityScores()) {
        const QByteArray& quality = wholeSeq.quality.qualCodes;
        CHECK_EXT(wholeSeq.length() == quality.length(), os.setError(errorMessage),);
        qualityData = quality.constData();
    }
    else {
        // record the highest possible quality
        buf.fill('I', wholeSeq.length());
        qualityData = buf.constData();
    }

    writeSequence(os, io, qualityData, wholeSeq.length(), errorMessage, cutLines);
}

void FastqFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::SEQUENCE), "Fastq entry storing: no sequences", );
    const QList<GObject*> &seqs = objectsMap[GObjectTypes::SEQUENCE];
    SAFE_POINT(1 == seqs.size(), "Fastq entry storing: sequence objects count error", );
    U2SequenceObject *seqObj = dynamic_cast<U2SequenceObject*>(seqs.first());
    SAFE_POINT(NULL != seqObj, "Fastq entry storing: NULL sequence object", );

    GUrl url = seqObj->getDocument() ? seqObj->getDocument()->getURL() : GUrl();
    static QString errorMessage = L10N::errorWritingFile(url);

    //write header;
    QString sequenceName = seqObj->getGObjectName();
    const DNASequence& seqData = seqObj->getWholeSequence(os);
    CHECK_OP(os, );
    writeEntry(sequenceName, seqData, io, errorMessage, os);
    CHECK_OP(os, );
}

DNASequence *FastqFormat::loadTextSequence(IOAdapter* io, U2OpStatus& os) {
    CHECK_EXT((io != NULL) && (io->isOpen() == true), os.setError(L10N::badArgument("IO adapter")), NULL);
    U2OpStatus2Log logOs;
    QByteArray readBuff;
    QByteArray sequence;
    QByteArray qualityScores;
    int predictedSize = 1000;
    sequence.reserve(predictedSize);
    qualityScores.reserve(predictedSize);

    //read header
    readBuff.clear();
    QString sequenceName = readSequenceName(os, io, '@');
    // check for eof while trying to read another FASTQ block
    CHECK_EXT(!io->isEof(), , NULL);
    CHECK_OP(os, new DNASequence());

    sequence.clear();
    readSequence(logOs, io, sequence);
    CHECK_OP(logOs, new DNASequence());

    QString qualSequenceName = readSequenceName(logOs, io, '+');
    if (!qualSequenceName.isEmpty()) {
        static const QString err = U2::FastqFormat::tr("Not a valid FASTQ file, sequence name differs from quality scores name");
        CHECK_EXT(sequenceName == qualSequenceName, logOs.setError(err), new DNASequence());
    }

    // read qualities
    qualityScores.clear();
    readQuality(logOs, io, qualityScores, sequence.size());
    CHECK_OP(logOs, new DNASequence());

    static const QString err = U2::FastqFormat::tr("Not a valid FASTQ file. Bad quality scores: inconsistent size.");
    CHECK_EXT(sequence.length() == qualityScores.length(), logOs.setError(err), new DNASequence());

    DNASequence *seq = new DNASequence(sequenceName, sequence);
    seq->quality = DNAQuality(qualityScores);
    seq->alphabet = U2AlphabetUtils::getById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    SAFE_POINT(seq->alphabet != NULL, "FastqFormat::loadSequence alphabet is NULL", new DNASequence());

    if (!seq->alphabet->isCaseSensitive()) {
        TextUtils::translate(TextUtils::UPPER_CASE_MAP, const_cast<char*>(seq->seq.constData()), seq->seq.length());
    }

    return seq;
}

} //namespace
