/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
 
 #include <U2Core/AnnotationData.h>
 #include <U2Core/AnnotationTableObject.h>
 #include <U2Core/IOAdapterTextStream.h>
 #include <U2Core/Log.h>
 #include <U2Core/U2ObjectDbi.h>
 #include <U2Core/U2OpStatus.h>
 #include <U2Core/U2SafePoints.h>
 #include <U2Core/TextUtils.h>

#include "KrakenResultsPlainTextFormat.h"

namespace U2 {

static const QString PAIRED_READS_DELIMITER = "|:|";
static const QString LENGTHS_DELIMITER = "|";
static const QString ID_SEQ_DELIMITER = ":";
static const QString QUICK_PROCESSING_MARK = ":Q";

struct LineParseResult {
    LineParseResult() {
    }

    QPair<QString, QList<SharedAnnotationData>> left;
    QPair<QString, QList<SharedAnnotationData>> right;
};

enum ReadsType {
    ReadsType_Unknown,
    ReadsType_Single,
    ReadsType_Paired
};

KrakenResultsPlainTextFormat::KrakenResultsPlainTextFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::PLAIN_KRAKEN_RESULTS, 
                         DocumentFormatFlag_SupportStreaming | DocumentFormatFlag_CannotBeCreated, {"txt"}) {
    formatName = tr("Kraken results");
    formatDescription = tr("Kraken classification results text format");
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
}

FormatCheckResult KrakenResultsPlainTextFormat::checkRawData(const QByteArray& rawData, const GUrl& url) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    return hasBinaryData ? FormatDetection_NotMatched : checkRawTextData(QString(data), url);
}

FormatCheckResult KrakenResultsPlainTextFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QString textCopy = dataPrefix;
    QTextStream stream(&textCopy);
    QString line = stream.readLine();
    QStringList lines;
    while (!line.isNull()) {
        if (!line.isEmpty()) {
            lines.append(line);
        }        
        line = stream.readLine();
    }
    if (lines.size() > 1) {
        //last line incomplete don't check it
        lines.removeLast();
    }    
    for (const QString& line : qAsConst(lines)) {
        const QStringList words = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        // first word - 'C' or 'U'
        if ((words[0] == "C" || words[0] == "U") && words.size() > 4) {
            bool isNumber = false;
            // remove pair delimiter, fourth word - still positive number
            QString fourthWord = words[3];
            fourthWord.remove(LENGTHS_DELIMITER);
            if (fourthWord.toInt(&isNumber) > 0 && isNumber) {
                // last word should contain ':', and not ends with ':Q', results shouldn't be from quick classification
                // dot try to analyze last word in case of single line, it is incomplete
                if ((!words.last().contains(ID_SEQ_DELIMITER) || words.last().endsWith(QUICK_PROCESSING_MARK)) && lines.size() > 1) {
                    return FormatDetection_NotMatched;
                }
            } else {
                return FormatDetection_NotMatched;
            }
        } else {
            return FormatDetection_NotMatched;
        }
    }
    return FormatDetection_Matched;
}

LineParseResult parse(const QString& line, int lineNumber, ReadsType& readsInFile, U2OpStatus& os) {
    LineParseResult result;
    bool linePaired = line.contains(PAIRED_READS_DELIMITER);
    if (readsInFile == ReadsType_Unknown) {
        readsInFile = linePaired ? ReadsType_Paired : ReadsType_Single;
    } else {
        if (readsInFile == ReadsType_Paired) {
            CHECK_EXT(linePaired, os.setError(
                KrakenResultsPlainTextFormat::tr("Error on line %1, line contains paired results opposite of previous lines in file.")
                .arg(QString::number(lineNumber))), result);
        } else if (readsInFile == ReadsType_Single) {
            CHECK_EXT(!linePaired, os.setError(
                KrakenResultsPlainTextFormat::tr("Error on line %1, line contains single results opposite of previous lines in file.")
                .arg(QString::number(lineNumber))), result);
        }
    }
    const QStringList words = line.split(QRegExp("\\s+"));
    CHECK_EXT(words[0] == "C" || words[0] == "U", os.setError(
        KrakenResultsPlainTextFormat::tr("Error on line %1, 1st word should be \"C\" or \"U\".").arg(QString::number(lineNumber))), result);
    result.left.first = words[1];
    if (words[0] == "U") {
        return result;
    }
    bool conversionIsOk = false;
    int convertedWord = words[2].toInt(&conversionIsOk);
    CHECK_EXT(conversionIsOk, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 3rd word should be number.")
        .arg(QString::number(lineNumber))), result);
    CHECK_EXT(convertedWord >= 0, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 3rd word should be number greater or equal zero.")
        .arg(QString::number(lineNumber))), result);

    int leftSequenceLength = 0;
    int rightSequenceLength = 0;
    if (readsInFile == ReadsType_Single) {
        convertedWord = words[3].toInt(&conversionIsOk);
        CHECK_EXT(conversionIsOk, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th word should be number.")
            .arg(QString::number(lineNumber))), result);
        CHECK_EXT(convertedWord > 0, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th word should be number greater than zero.")
            .arg(QString::number(lineNumber))), result);
        leftSequenceLength = convertedWord;
    } else {
        result.left.first = words[1] + "_left";
        result.right.first = words[1] + "_right";
        QStringList pair = words[3].split(LENGTHS_DELIMITER);
        CHECK_EXT(pair.size() == 2, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th word not match format \"<number>|<number>\".")
            .arg(QString::number(lineNumber))), result);
        convertedWord = pair[0].toInt(&conversionIsOk);
        CHECK_EXT(conversionIsOk, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th word, first element of the pair should be a number.")
            .arg(QString::number(lineNumber))), result);
        CHECK_EXT(convertedWord > 0, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th, first element of the pair should be number greater than zero.")
            .arg(QString::number(lineNumber))), result);
        leftSequenceLength = convertedWord;
        convertedWord = pair[1].toInt(&conversionIsOk);
        CHECK_EXT(conversionIsOk, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th word, second element of the pair should be a number.")
            .arg(QString::number(lineNumber))), result);
        CHECK_EXT(convertedWord > 0, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, 4th, second element of the pair should be number greater than zero.")
            .arg(QString::number(lineNumber))), result);
        rightSequenceLength = convertedWord;
    }

    int summaryFramentLength = 0;
    int summaryFramentLengthLeft = 0;
    bool leftPart = true;
    QList<SharedAnnotationData> annotationData;
    QList<SharedAnnotationData>* currentResultPart = &(result.left.second);
    for (int wordIndex = 4; wordIndex < words.size(); wordIndex++) {
        if (words[wordIndex] == PAIRED_READS_DELIMITER) {
            currentResultPart = &(result.right.second);
            summaryFramentLengthLeft = summaryFramentLength;
            summaryFramentLength = 0;
            leftPart = false;
            continue;
        }
        const QStringList pair = words[wordIndex].split(ID_SEQ_DELIMITER);
        const int naturalWordIndex = wordIndex + 1;
        CHECK_EXT(pair.size() == 2, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, %2th word not match format \"<string_or_number>:<number>\".").arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);

        const int fragmentLength = pair[1].toInt(&conversionIsOk);
        CHECK_EXT(conversionIsOk, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, %2th word second element of the pair is not a number.").arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);
        CHECK_EXT(fragmentLength > 0, os.setError(KrakenResultsPlainTextFormat::tr("Error on line %1, %2th word second element of the pair should be number greater than zero.").arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);
        const int lengthToCheck = leftPart ? leftSequenceLength : rightSequenceLength;
        if (pair[0] != "0" && pair[0] != "unclassified") {
            SharedAnnotationData annotData(new AnnotationData);
            if (leftPart) {
                annotData->location->regions = {U2Region(summaryFramentLength, fragmentLength)};
            } else {
                annotData->location->strand = U2Strand::Complementary;
                annotData->location->regions = {U2Region(summaryFramentLengthLeft - (summaryFramentLength + fragmentLength), fragmentLength)};
            }
            annotData->name = pair[0];

            currentResultPart->append(annotData);
        }
        summaryFramentLength += fragmentLength;
        CHECK_EXT(lengthToCheck >= summaryFramentLength,
                  os.setError(KrakenResultsPlainTextFormat::tr("Summary fragment length %1 should be less or equal sequence length %2.")
                                  .arg(QString::number(summaryFramentLength))
                                  .arg(QString::number(lengthToCheck))),
                  result);
    }
    return result;
}

Document* KrakenResultsPlainTextFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap&, U2OpStatus& os) {
    ReadsType readsInFile = ReadsType_Unknown;
    QMultiMap<QString, QList<SharedAnnotationData>> annotationDataForTables;
    int lineNumber = 1;
    QMultiMap<QString, QList<SharedAnnotationData>> annotationsMap;
    while (!os.isCoR() && !reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).simplified();
        CHECK_OP(os, nullptr);
        LineParseResult result = parse(line,lineNumber, readsInFile, os);
        CHECK_OP(os, nullptr);
        os.setProgress(reader.getProgress());
        CHECK_EXT_CONTINUE(!result.left.second.isEmpty(), ioLog.details(tr("Sequence %1 skipped, because no classified result found").arg(result.left.first)));
        annotationsMap.insert(result.left.first, result.left.second);
        if (readsInFile == ReadsType_Paired) {
            CHECK_EXT_CONTINUE(!result.right.second.isEmpty(), 
                               ioLog.details(tr("Sequence %1 skipped, because no classified result found").arg(result.right.first)));
            annotationsMap.insert(result.right.first, result.right.second);
        }
    }
    
    QMap<AnnotationTableObject*, QMap<QString, QList<SharedAnnotationData>>> annTable2Annotations;
    QMultiMap<QString, QList<SharedAnnotationData>>::const_iterator iter = annotationsMap.constBegin();
    QList<AnnotationTableObject*> objects;
    while (iter != annotationsMap.constEnd()) {
        const QString& sequenceName = iter.key();
        QString annotTableName = sequenceName + FEATURES_TAG;
        AnnotationTableObject* annotTable = nullptr;
        for (GObject* table : qAsConst(objects)) {
            CHECK_EXT_BREAK(table->getGObjectName() == annotTableName, 
                            ioLog.details(tr("Two or more classification results with same sequence name \"%1\" were found."
                                             "They will be merged in one annotation table").arg(annotTableName)));
        }
        if (annotTable == nullptr) {
            annotTable = new AnnotationTableObject(annotTableName, dbiRef, {{DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER}});
            objects.append(annotTable);
        }
        const QList<SharedAnnotationData>& annotList = iter.value();
        for (const SharedAnnotationData& annotData : qAsConst(annotList)) {
            annTable2Annotations[annotTable]["Classified fragments"].append(annotData);
        }
        iter++;
    }

    QList<AnnotationTableObject*> annTable2AnnotationsKeys = annTable2Annotations.keys();
    for (AnnotationTableObject* ato : qAsConst(annTable2AnnotationsKeys)) {
        const QStringList keys = annTable2Annotations[ato].keys();
        for (const QString& groupName : qAsConst(keys)) {
            ato->addAnnotations(annTable2Annotations[ato][groupName], groupName);
        }
    }    
    CHECK_OP_EXT(os, qDeleteAll(objects), nullptr);
    QList<GObject*> gobjects;
    for (GObject* ob : objects) {
        gobjects.append(ob);
    }
    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, gobjects);
}

}
