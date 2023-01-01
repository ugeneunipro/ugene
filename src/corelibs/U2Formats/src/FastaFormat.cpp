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

#include "FastaFormat.h"

#include <QTextStream>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/Task.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"

namespace U2 {

FastaFormat::FastaFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::FASTA, DocumentFormatFlags_SW, {"fa", "mpfa", "fna", "fsa", "fas", "fasta", "sef", "seq", "seqs"}) {
    formatName = tr("FASTA");
    supportedObjectTypes += GObjectTypes::SEQUENCE;
    supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatDescription = tr("FASTA format is a text-based format for representing either nucleotide sequences or peptide sequences, "
                           "in which base pairs or amino acids are represented using single-letter codes. "
                           "The format also allows for sequence names and comments to precede the sequences.");
}

/** Returns FormatDetectionResult properties (hints) for the given data. */
static QVariantMap buildFormatDetectionHints(const QString& data) {
    bool hasGaps = false;
    int minLen = -1;
    int maxLen = -1;
    int currentSequenceLength = 0;
    int nSequences = 0;

    QString rwData = data;
    QTextStream textStream(&rwData, QIODevice::ReadOnly);
    do {
        QString line = textStream.readLine();
        if (line.startsWith(FastaFormat::FASTA_HEADER_START_SYMBOL)) {
            nSequences++;
            if (currentSequenceLength > 0) {
                minLen = minLen == -1 ? currentSequenceLength : qMin(minLen, currentSequenceLength);
                maxLen = maxLen == -1 ? currentSequenceLength : qMax(maxLen, currentSequenceLength);
            }
            currentSequenceLength = 0;
        } else {
            currentSequenceLength += line.length();
            if (!hasGaps && line.contains(U2Msa::GAP_CHAR)) {
                hasGaps = true;
            }
        }
    } while (!textStream.atEnd());

    QVariantMap hints;
    hints[RawDataCheckResult_Sequence] = true;
    if (hasGaps) {
        hints[RawDataCheckResult_SequenceWithGaps] = true;
    }
    if (nSequences > 1) {
        hints[RawDataCheckResult_MultipleSequences] = true;
    }
    if (minLen > 0) {
        hints[RawDataCheckResult_MaxSequenceSize] = maxLen;
        hints[RawDataCheckResult_MinSequenceSize] = minLen;
    }
    return hints;
}

/** Returns true if the line is a valid comment line for FASTA format. */
static bool isCommentLine(const QString& line) {
    return line.startsWith(FastaFormat::FASTA_COMMENT_START_SYMBOL);
}

/** Returns true if the line is a valid comment line for FASTA format. */
static bool isHeaderLine(const QString& line) {
    return line.startsWith(FastaFormat::FASTA_HEADER_START_SYMBOL);
}

FormatCheckResult FastaFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString data = TextUtils::skip(TextUtils::WHITES, dataPrefix);
    FormatDetectionScore score;
    if (isHeaderLine(data)) {
        // A perfect match: start of the FASTA file.
        score = FormatDetection_Matched;
    } else if (isCommentLine(data) && data.contains(QString("\n") + FASTA_HEADER_START_SYMBOL)) {
        // A good match: looks like a FASTA comment before a FASTA header line.
        score = FormatDetection_HighSimilarity;
    } else {
        return FormatDetection_NotMatched;
    }
    // Ok, format is matched -> add hints about sequence sizes.
    FormatCheckResult result(score);
    result.properties = buildFormatDetectionHints(data);
    return result;
}

/** Skips leading 'whites' and comment lines from the stream. */
static void skipLeadingWhitesAndComments(IOAdapterReader& reader, U2OpStatus& os) {
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE);
        CHECK_OP(os, );
        bool isCommentOrEmptyLine = isCommentLine(line) || line.trimmed().isEmpty();
        if (!isCommentOrEmptyLine) {
            reader.undo(os);
            return;
        }
    }
}

/** Reads a full FASTA header line from the current position and returns a part with no leading '>' character. */
static QString readHeader(IOAdapterReader& reader, U2OpStatus& os) {
    QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).trimmed();
    CHECK_OP(os, "");
    CHECK_EXT(isHeaderLine(line), os.setError(FastaFormat::tr("First line is not a FASTA header")), "");
    return line.mid(1);  // Return a part with no '>' prefix.
}

/** Loads sequence objects into 'objects' list from the text data in FASTA format available via 'reader'. */
static void load(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, QList<GObject*>& objects, int gapSize, QString& writeLockReason, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );

    MemoryLocker memoryLocker(os, 1);
    CHECK_OP(os, );

    writeLockReason.clear();

    QString buf(DocumentFormat::READ_BUFF_SIZE + 1, '\0');
    bool mergeIntoSingleSequence = gapSize != -1;

    QStringList headers;
    QSet<QString> uniqueNames;
    QVector<U2Region> mergedMapping;

    GObjectReference sequenceRef;
    U2SequenceImporter seqImporter(hints, true);
    QString folder = hints.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    qint64 sequenceStart = 0;
    int sequenceNumber = 0;
    DbiConnection con(dbiRef, os);
    QStringList emptySeqNames;

    int objectsCountLimit = hints.contains(DocumentReadingMode_MaxObjectsInDoc) ? hints[DocumentReadingMode_MaxObjectsInDoc].toInt() : INT_MAX;
    bool makeUniqueSequenceNames = !hints.value(DocumentReadingMode_DontMakeUniqueNames, false).toBool();
    while (!os.isCoR() && !reader.atEnd()) {
        skipLeadingWhitesAndComments(reader, os);
        CHECK_OP(os, );

        QString header = readHeader(reader, os).trimmed();
        CHECK_OP(os, );

        // Construct sequence name.
        if (sequenceNumber == 0 || !mergeIntoSingleSequence) {
            QString objectName = header;
            if (objectName.isEmpty()) {
                objectName = "Sequence";
            }
            if (makeUniqueSequenceNames) {
                objectName = (mergeIntoSingleSequence) ? "Sequence" : TextUtils::variate(objectName, "_", uniqueNames);
                objectName.squeeze();
                memoryLocker.tryAcquire(2 * objectName.size());
                CHECK_OP_BREAK(os);
                uniqueNames.insert(objectName);
            }
            seqImporter.startSequence(os, dbiRef, folder, objectName, false);
            CHECK_OP_BREAK(os);

            sequenceRef = GObjectReference(reader.getURL().getURLString(), objectName, GObjectTypes::SEQUENCE);
        }
        if (sequenceNumber >= 1 && mergeIntoSingleSequence) {
            seqImporter.addDefaultSymbolsBlock(gapSize, os);
            sequenceStart += gapSize;
            CHECK_OP_BREAK(os);
        }

        // Read sequence.
        int sequenceLen = 0;
        while (!os.isCoR() && !reader.atEnd()) {
            reader.readLine(os, buf, DocumentFormat::READ_BUFF_SIZE);
            CHECK_OP(os, );
            if (isCommentLine(buf)) {
                continue;  // Skip comments.
            }
            if (isHeaderLine(buf)) {
                reader.undo(os);
                CHECK_OP(os, );
                break;
            }
            // DNASequence in UGENE can contain only 1-byte symbols.
            // Forcing latin1 conversion here: the local multi-byte symbols will be lost.
            QByteArray dnaChars = buf.toLatin1();
            char* dnaCharsBuff = dnaChars.data();
            int sequenceChunkLength = TextUtils::remove(dnaCharsBuff, dnaChars.length(), TextUtils::WHITES);
            if (sequenceChunkLength > 0) {
                seqImporter.addBlock(dnaCharsBuff, sequenceChunkLength, os);
                CHECK_OP_BREAK(os);
                sequenceLen += sequenceChunkLength;
            }
            os.setProgress(reader.getProgress());
        }

        if (mergeIntoSingleSequence) {
            memoryLocker.tryAcquire(header.size());
            CHECK_OP_BREAK(os);
            headers.append(header);
            mergedMapping.append(U2Region(sequenceStart, sequenceLen));
        } else {
            if (objectsCountLimit > 0 && objects.size() >= objectsCountLimit) {
                os.setError(FastaFormat::tr("File \"%1\" contains too many sequences to be displayed. "
                                            "However, you can process these data using instruments from the menu <i>Tools -> NGS data analysis</i> "
                                            "or pipelines built with Workflow Designer.")
                                .arg(reader.getURL().getURLString()));
                break;
            }
            memoryLocker.tryAcquire(800);
            CHECK_OP_BREAK(os);
            U2Sequence seq = seqImporter.finalizeSequenceAndValidate(os);
            if (os.getError() == U2SequenceImporter::EMPTY_SEQUENCE_ERROR) {
                os.setError("");
                emptySeqNames << header;
                continue;
            }
            sequenceRef.entityRef = U2EntityRef(dbiRef, seq.id);

            // TODO parse header
            U2StringAttribute attr(seq.id, DNAInfo::FASTA_HDR, header);
            con.dbi->getAttributeDbi()->createStringAttribute(attr, os);
            CHECK_OP_BREAK(os);

            objects << new U2SequenceObject(seq.visualName, U2EntityRef(dbiRef, seq.id));
            CHECK_OP_BREAK(os);

            U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, nullptr, hints);
        }
        sequenceStart += sequenceLen;
        sequenceNumber++;
    }

    if (hints.value(DocumentFormat::STRONG_FORMAT_ACCORDANCE, QVariant(false)).toBool() && !emptySeqNames.isEmpty()) {
        os.setError(FastaFormat::tr("The file format is invalid."));
    }

    CHECK_OP_EXT(os, qDeleteAll(objects); objects.clear(), );
    bool isEmptyFileAllowed = hints.value(DocumentReadingMode_AllowEmptyFile).toBool();
    CHECK_EXT(!objects.isEmpty() || mergeIntoSingleSequence || isEmptyFileAllowed, os.setError(Document::tr("Document is empty.")), );
    SAFE_POINT(headers.size() == mergedMapping.size(), "headers <-> regions mapping failed!", );
    ioLog.trace("All sequences are processed");

    if (!emptySeqNames.isEmpty()) {
        QString warningMessage;
        warningMessage.append(FastaFormat::tr("Loaded sequences: %1. \n").arg(sequenceNumber));
        warningMessage.append(FastaFormat::tr("Skipped sequences: %1. \n").arg(emptySeqNames.size()));
        warningMessage.append(FastaFormat::tr("The following sequences are empty: \n%1").arg(emptySeqNames.join(", \n")));
        os.addWarning(warningMessage);
    }

    if (!mergeIntoSingleSequence) {
        return;
    }

    U2Sequence seq = seqImporter.finalizeSequenceAndValidate(os);
    CHECK_OP(os, );
    sequenceRef.entityRef = U2EntityRef(dbiRef, seq.id);

    U1AnnotationUtils::addAnnotations(objects, seqImporter.getCaseAnnotations(), sequenceRef, nullptr, hints);
    objects << new U2SequenceObject(seq.visualName, U2EntityRef(dbiRef, seq.id));
    objects << DocumentFormatUtils::addAnnotationsForMergedU2Sequence(sequenceRef, dbiRef, headers, mergedMapping, hints);
    if (headers.size() > 1) {
        writeLockReason = QObject::tr("Document sequences were merged");
    }
}

Document* FastaFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    QList<GObject*> objects;
    int gapSize = qBound(-1, DocumentFormatUtils::getMergeGap(hints), 1000 * 1000);

    QString lockReason;
    load(reader, dbiRef, hints, objects, gapSize, lockReason, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), nullptr);

    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects, hints, lockReason);
}

static void writeHeaderToFile(IOAdapterWriter& writer, const QString& sequenceName, U2OpStatus& os) {
    writer.write(os, FastaFormat::FASTA_HEADER_START_SYMBOL + sequenceName + '\n');
}

static void saveSequenceObject(IOAdapterWriter& writer, const U2SequenceObject* sequence, U2OpStatus& os) {
    writeHeaderToFile(writer, sequence->getSequenceName(), os);
    CHECK_OP(os, );
    qint64 sequenceLength = sequence->getSequenceLength();
    // Reading DBI line-by-line is very expensive. Read by bigger chunks and split them into lines in memory.
    qint64 maxChunkSize = 1000 * FastaFormat::FASTA_SEQUENCE_LINE_LENGTH;
    for (qint64 i = 0; i < sequenceLength; i += maxChunkSize) {
        qint64 chunkSize = qMin(maxChunkSize, sequenceLength - i);
        U2Region region(i, chunkSize);
        QByteArray chunkContent = sequence->getSequenceData(region, os);
        QList<QByteArray> lines = TextUtils::split(chunkContent, FastaFormat::FASTA_SEQUENCE_LINE_LENGTH);
        for (const QByteArray& line : qAsConst(lines)) {
            CHECK_OP(os, );
            writer.write(os, QString::fromLatin1(line));
            CHECK_OP(os, );
            writer.write(os, "\n");
            CHECK_OP(os, );
        }
    }
}

static void saveSequence(IOAdapterWriter& writer, const DNASequence& sequence, U2OpStatus& os) {
    writeHeaderToFile(writer, sequence.getName(), os);
    CHECK_OP(os, );

    const char* seq = sequence.constData();
    qint64 sequenceLength = sequence.length();
    for (qint64 i = 0; i < sequenceLength; i += FastaFormat::FASTA_SEQUENCE_LINE_LENGTH) {
        int chunkSize = (int)qMin((qint64)FastaFormat::FASTA_SEQUENCE_LINE_LENGTH, sequenceLength - i);
        writer.write(os, QString::fromLatin1(seq + i, chunkSize));
        CHECK_OP(os, );
        writer.write(os, "\n");
        CHECK_OP(os, );
    }
}

void FastaFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    QList<GObject*> objects = document->getObjects();
    for (GObject* object : qAsConst(objects)) {
        if (auto sequenceObject = dynamic_cast<U2SequenceObject*>(object)) {
            saveSequenceObject(writer, sequenceObject, os);
            CHECK_OP(os, );
        } else {
            QList<DNASequence> sequences = DocumentFormatUtils::toSequences(object);
            for (const DNASequence& sequence : qAsConst(sequences)) {
                saveSequence(writer, sequence, os);
                CHECK_OP(os, );
            }
        }
    }
}

void FastaFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::SEQUENCE), "Fasta entry storing: no sequences", );

    const QList<GObject*>& sequenceObjects = objectsMap[GObjectTypes::SEQUENCE];
    SAFE_POINT(sequenceObjects.size() == 1, "Fasta entry storing: expecting 1 sequence object", );

    auto sequenceObject = dynamic_cast<U2SequenceObject*>(sequenceObjects.first());
    SAFE_POINT(sequenceObject != nullptr, "Fasta entry storing: sequence object is null", );
    saveSequenceObject(writer, sequenceObject, os);
}

DNASequence* FastaFormat::loadTextSequence(IOAdapterReader& reader, U2OpStatus& os) {
    try {
        MemoryLocker l(os);
        CHECK_OP(os, nullptr);

        QString buf(DocumentFormat::READ_BUFF_SIZE + 1, '\0');
        skipLeadingWhitesAndComments(reader, os);
        CHECK_OP(os, nullptr);
        CHECK(!reader.atEnd(), nullptr);

        QString header = readHeader(reader, os).trimmed();
        CHECK_OP(os, nullptr);
        l.tryAcquire(header.capacity());
        CHECK_OP(os, nullptr);

        // Read sequence.
        QByteArray sequence;
        while (!os.isCoR() && !reader.atEnd()) {
            reader.readLine(os, buf, DocumentFormat::READ_BUFF_SIZE);
            CHECK_OP(os, nullptr);
            if (isCommentLine(buf)) {
                continue;  // Skip comment line.
            }
            if (isHeaderLine(buf)) {
                reader.undo(os);
                CHECK_OP(os, nullptr);
                break;
            }
            // DNASequence in UGENE can contain only 1-byte symbols.
            // Forcing latin1 conversion here: the local multi-byte symbols will be lost.
            QByteArray dnaChars = buf.toLatin1();
            char* dnaCharsBuff = dnaChars.data();
            int sequenceChunkLength = TextUtils::remove(dnaCharsBuff, dnaChars.length(), TextUtils::WHITES);
            if (sequenceChunkLength > 0) {
                l.tryAcquire(sequenceChunkLength);
                CHECK_OP(os, nullptr);
                sequence.append(dnaCharsBuff, sequenceChunkLength);
                CHECK_OP_BREAK(os);
            }
            os.setProgress(reader.getProgress());
        }
        sequence.squeeze();

        // TODO: shouldn't we detect the alphabet here instead of using the hardcoded one?
        auto alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
        SAFE_POINT(alphabet != nullptr, "Can't find built-in NUCL_DNA_EXTENDED alphabet!", nullptr);
        auto dnaSequence = new DNASequence(header, sequence, alphabet);
        if (!dnaSequence->alphabet->isCaseSensitive()) {
            TextUtils::translate(TextUtils::UPPER_CASE_MAP, const_cast<char*>(dnaSequence->seq.constData()), dnaSequence->seq.length());
        }
        return dnaSequence;
    } catch (...) {
        os.setError(L10N::outOfMemory());
        return nullptr;
    }
}

void FastaFormat::storeSequence(const DNASequence& sequence, IOAdapter* ioAdapter, U2OpStatus& os) {
    IOAdapterWriter writer(ioAdapter);
    saveSequence(writer, sequence, os);
}

void FastaFormat::storeSequence(const U2SequenceObject* sequence, IOAdapter* ioAdapter, U2OpStatus& os) {
    IOAdapterWriter writer(ioAdapter);
    saveSequenceObject(writer, sequence, os);
}

static QString skipComments(const QString& userInput, U2OpStatus& os) {
    QStringList lines = userInput.trimmed().split("\n", QString::SkipEmptyParts);
    QStringList result = lines;
    QStringList unreferenced;
    foreach (const QString& line, lines) {
        if (isHeaderLine(line)) {
            break;
        } else {
            result.removeFirst();
        }

        if (!isCommentLine(line)) {
            unreferenced << line;
        }
    }

    if (!unreferenced.isEmpty()) {
        QString seq = unreferenced.join(" ");
        os.setError(FastaFormat::tr("Unreferenced sequence in the beginning of patterns: %1").arg(seq));
    }

    return result.join("\n");
}

QList<QPair<QString, QString>> FastaFormat::getSequencesAndNamesFromUserInput(const QString& userInput, U2OpStatus& os) {
    // TODO: rework to use common FASTA parsing algorithm.
    QList<QPair<QString, QString>> result;
    if (userInput.contains(FASTA_HEADER_START_SYMBOL)) {
        QString patterns = skipComments(userInput, os);
        QStringList seqDefs = patterns.trimmed().split(FASTA_HEADER_START_SYMBOL, QString::SkipEmptyParts);

        for (const QString& seqDef : qAsConst(seqDefs)) {
            QStringList seqData = seqDef.split("\n");
            CHECK_EXT(!seqData.isEmpty(), os.setError("Invalid fasta input"), result);
            QString name = seqData.takeFirst();
            QString sequence;
            for (const QString& line : qAsConst(seqData)) {
                if (isCommentLine(line)) {
                    continue;
                }
                sequence += line;
            }
            result.append(qMakePair(name, sequence));
        }
    }

    return result;
}

}  // namespace U2
