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

KrakenResultsPlainTextFormat::KrakenResultsPlainTextFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::PLAIN_KRAKEN_RESULTS, 
                         DocumentFormatFlag_SupportStreaming | DocumentFormatFlag_CannotBeCreated, {"txt"}) {
    formatName = tr("Kraken results");
    formatDescription = tr("Kraken classification results text format");
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
}

FormatCheckResult KrakenResultsPlainTextFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    const QStringList words = dataPrefix.split(QRegExp("\\s+"));
    //first word - 'C' or 'U'
    if ((words[0] == "C" || words[0] == "U") && words.size() > 5) {
        bool isNumber = false;
        //fourth word - positive number
        if (words[3].toInt(&isNumber) > 0 && isNumber) {
            bool classificationCoordinatesFound = false;
            //all words - not contain ':Q'
            for (const QString& word : qAsConst(words)) {
                if (word.contains(":Q")) {
                    return FormatDetection_NotMatched; 
                }
                if (word.contains(":") && classificationCoordinatesFound == false) {
                    classificationCoordinatesFound = true;
                }
            }
            return classificationCoordinatesFound ? FormatDetection_Matched : FormatDetection_NotMatched;
        }
    }
    return FormatDetection_NotMatched;
}

Document* KrakenResultsPlainTextFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap&, U2OpStatus& os) {
    QMultiMap<QString, QList<SharedAnnotationData>> annotationDataForTables;
    int lineNumber = 1;
    QMultiMap<QString, QList<SharedAnnotationData>> annotationsMap;
    while (!os.isCoR() && !reader.atEnd()) {
        QString line = reader.readLine(os, DocumentFormat::READ_BUFF_SIZE).simplified();
        CHECK_OP(os, nullptr);
        QPair<QString, QList<SharedAnnotationData>> result = parse(line,lineNumber, os);
        CHECK_OP(os, nullptr);
        os.setProgress(reader.getProgress());
        CHECK_EXT_CONTINUE(!result.second.isEmpty(), ioLog.details(tr("Sequence %1 skipped, because no classified result found").arg(result.first)));
        annotationsMap.insert(result.first, result.second);
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

QPair<QString, QList<SharedAnnotationData>> KrakenResultsPlainTextFormat::parse(const QString& line, int lineNumber, U2OpStatus& os) {
    QPair<QString, QList<SharedAnnotationData>> result;
    const QStringList words = line.split(QRegExp("\\s+"));
    CHECK_EXT(words[0] == "C" || words[0] == "U", os.setError(tr("Error on line %1, 1st word should be \"C\" or \"U\".")
              .arg(QString::number(lineNumber))), result);

    bool conversionIsOk = false;
    int convertedWord = words[2].toInt(&conversionIsOk);
    CHECK_EXT(conversionIsOk, os.setError(tr("Error on line %1, 3rd word should be number.").arg(QString::number(lineNumber))), result);
    CHECK_EXT(convertedWord >= 0, os.setError(tr("Error on line %1, 3rd word should be number greater or equal zero.")
              .arg(QString::number(lineNumber))), result);
    
    result.first = words[1];

    convertedWord = words[3].toInt(&conversionIsOk);
    CHECK_EXT(conversionIsOk, os.setError(tr("Error on line %1, 4th word should be number.").arg(QString::number(lineNumber))), result);
    CHECK_EXT(convertedWord > 0, os.setError(tr("Error on line %1, 4th word should be number greater than zero.")
              .arg(QString::number(lineNumber))), result);
    const int sequenceLength  = convertedWord;
    int summaryFramentLength = 0;
    QList <SharedAnnotationData> annotationData;
    for (int wordIndex = 4; wordIndex < words.size(); wordIndex++) {
        const QStringList pair = words[wordIndex].split(":");
        const int naturalWordIndex = wordIndex + 1;
        CHECK_EXT(pair.size() == 2, os.setError(tr("Error on line %1, %2th word not match format \"<string_or_number>:<number>\".")
                                                .arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);

        const int fragmentLength = pair[1].toInt(&conversionIsOk);
        CHECK_EXT(conversionIsOk, os.setError(tr("Error on line %1, %2th word second element of the pair is not a number.")
                                                .arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);
        CHECK_EXT(fragmentLength > 0, os.setError(tr("Error on line %1, %2th word second element of the pair should be number greater than zero.")
                                                .arg(QString::number(lineNumber)).arg(QString::number(naturalWordIndex))), result);
        if (pair[0] != "0" && pair[0] != "unclassified") {
            SharedAnnotationData annotData(new AnnotationData);
            annotData->name = pair[0];
            annotData->location->regions = {U2Region(summaryFramentLength, fragmentLength)};
            result.second.append(annotData);
        }
        summaryFramentLength += fragmentLength;
        CHECK_EXT(sequenceLength >= summaryFramentLength,
                  os.setError(tr("Summary fragment length %1 should be less or equal sequence length %2.")
                                  .arg(QString::number(summaryFramentLength))
                                  .arg(QString::number(sequenceLength))),
                  result);
    }
    return result;
}

}
