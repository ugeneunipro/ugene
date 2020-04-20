/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QScopedArrayPointer>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

#include "DocumentFormatUtils.h"
#include "BedFormat.h"

namespace U2{

//-------------------------------------------------------------------
//  BEDLineValidateFlags
//-------------------------------------------------------------------
BEDLineValidateFlags::BEDLineValidateFlags()
    : incorrectNumberOfFields(false),
      emptyFields(false),
      incorrectCoordinates(false),
      incorrectScore(false),
      incorrectStrand(false),
      incorrectThickCoordinates(false),
      incorrectItemRgb(false),
      incorrectBlocks(false),
      hasTrackLine(false)
{
}

FormatDetectionScore BEDLineValidateFlags::getFormatDetectionScore()
{
    if (incorrectNumberOfFields || emptyFields || incorrectCoordinates) {
        return FormatDetection_NotMatched;
    }

    if (incorrectScore || incorrectStrand || incorrectBlocks || !hasTrackLine) {
        return FormatDetection_LowSimilarity;
    }

    if (incorrectThickCoordinates || incorrectItemRgb) {
        return FormatDetection_HighSimilarity;
    }

    return FormatDetection_Matched;
}


//-------------------------------------------------------------------
//  BedFormat
//-------------------------------------------------------------------
//Names of supported qualifier names
namespace {
    const QString TRACK_NAME_QUALIFIER_NAME = "track_name";
    const QString TRACK_DESCR_QUALIFIER_NAME = "track_description";
    const QString CHROM_QUALIFIER_NAME = "chrom";
    const QString ANNOT_QUALIFIER_NAME = "name";
    const QString SCORE_QUALIFIER_NAME = "score";
    const QString STRAND_QUALIFIER_NAME = "strand";
    const QString THICK_START_QUALIFIER_NAME = "thick_start";
    const QString THICK_END_QUALIFIER_NAME = "thick_end";
    const QString ITEM_RGB_QUALIFIER_NAME = "item_rgb";
    const QString BLOCK_COUNT_QUALIFIER_NAME = "block_count";
    const QString BLOCK_SIZES_QULAIFIER_NAME = "block_sizes";
    const QString BLOCK_STARTS_QUALIFIER_NAME = "block_starts";
}

BedFormat::BedFormat(QObject* p)
    : TextDocumentFormat(p, BaseDocumentFormats::BED, DocumentFormatFlag_SupportWriting, QStringList("bed"))
{
    formatName = tr("BED");
    formatDescription = tr("The BED (Browser Extensible Data) format was developed by UCSC for displaying transcript structures in the genome browser.");
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
}


Document* BedFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os)
{
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);
    QList<GObject*> objects;

    load(io, objects, dbiRef, os, fs);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects);
    return doc;
}

void BedFormat::load(IOAdapter* io, QList<GObject*>& objects, const U2DbiRef& dbiRef, U2OpStatus& os, const QVariantMap& fs) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, );
    Q_UNUSED(opBlock);

    QString defaultAnnotName = "misc_feature";
    BedFormatParser parser(io, defaultAnnotName, os);
    const QHash<QString, QList<SharedAnnotationData> >& annotationsHash = parser.parseDocument();
    CHECK_OP(os, );
    const int objectsCountLimit = fs.contains(DocumentReadingMode_MaxObjectsInDoc) ? fs[DocumentReadingMode_MaxObjectsInDoc].toInt() : -1;

    foreach (const QString &sequenceName, annotationsHash.keys()) {
        const QString annotTableName = sequenceName + FEATURES_TAG;
        AnnotationTableObject *annotTable = NULL;
        foreach (GObject* object, objects) {
            if (object->getGObjectName() == annotTableName) {
                annotTable = dynamic_cast<AnnotationTableObject *>(object);
            }
        }
        if (!annotTable) {
            if (objectsCountLimit > 0 && objects.size() >= objectsCountLimit) {
                os.setError(tr("File \"%1\" contains too many annotation tables to be displayed. "
                    "However, you can process these data using pipelines built with Workflow Designer.").arg(io->getURL().getURLString()));
                break;
            }
            QVariantMap hints;
            hints.insert(DBI_FOLDER_HINT, fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
            annotTable = new AnnotationTableObject(annotTableName, dbiRef, hints);
            objects.append(annotTable);
        }

        // Assume that the group name is the same as the annotation name
        QString groupName = defaultAnnotName;
        if (AnnotationGroup::isValidGroupName(groupName, false)) {
            groupName = "Group"; // or set this name if the annotation name is not appropriate
        }

        const QList<SharedAnnotationData> &annotations = annotationsHash.value(sequenceName);
        annotTable->addAnnotations(annotations, groupName);
    }
}

/** Validate the values: they must be integer and within the bound of the region */
bool validateThickCoordinates(const QString& thickStartStr, const QString& thickEndStr)
{
    if (thickStartStr.isEmpty() || thickEndStr.isEmpty()) {
        return false;
    }

    bool thickStartIsInt = thickStartStr.toInt(&thickStartIsInt);
    bool thickEndIsInt = thickEndStr.toInt(&thickEndIsInt);
    if (!thickStartIsInt || !thickEndIsInt) {
            return false;
    }

    return true;
}


/**
* Validate that the color is valid
* If color is "0", then it is not set, otherwise it has format r,g,b
* If color is valid, corresponding QColor is set.
*/
bool validateAnnotationColor(const QString& itemRgbStr, QColor& annotColor)
{
    if ("0" == itemRgbStr) {
        return true;
    }

    QStringList rgbValues = itemRgbStr.split(",");
    if (3 != rgbValues.count()) {
        return false;
    }

    bool convertionStatusIsOk;

    int red = rgbValues[0].toInt(&convertionStatusIsOk);
    if (!convertionStatusIsOk) {
        return false;
    }

    int green = rgbValues[1].toInt(&convertionStatusIsOk);
    if (!convertionStatusIsOk) {
        return false;
    }

    int blue = rgbValues[2].toInt(&convertionStatusIsOk);
    if (!convertionStatusIsOk) {
        return false;
    }

    QColor color(red, green, blue);
    if (!color.isValid()) {
        return false;
    }

    annotColor = color;
    return true;
}


/**
 * Validate blocks (exons):
 * blockCount is the number of blocks.
 * blockSizes is a comma-separated list of the block sizes. The number of items must correspond to blockCount.
 * blockStarts is a comma-separated list of the block starts. All of the blockStart positions are calculated
 * relative to chromStart. The number of items must correspond to blockCount.
 * Note that the validated/calculated exons are not currently remembered.
 */
bool validateBlocks(const QString& blockCountStr, const QString& blockSizesStr, const QString& blockStartsStr, const U2Region& region)
{
    bool blockCountIsInt;
    int blockCount = blockCountStr.toInt(&blockCountIsInt);
    if (!blockCountIsInt || (blockCount == 0)) {
        return false;
    }

    // Skipping empty parts because values can have e.g. the following format: "567, 488," (comma at the end)
    QStringList blockSizesStrValues = blockSizesStr.split(",", QString::SkipEmptyParts);
    if (blockSizesStrValues.count() != blockCount) {
        return false;
    }

    QStringList blockStartsStrValues = blockStartsStr.split(",", QString::SkipEmptyParts);
    if (blockStartsStrValues.count() != blockCount) {
        return false;
    }

    bool conversionIsOk;
    QVector<int> blockSizes;
    QVector<int> blockStarts;
    for (int i = 0; i < blockCount; ++i) {
        blockSizesStrValues[i].toInt(&conversionIsOk);
        if (false == conversionIsOk) {
            return false;
        }

        int start = blockStartsStrValues[i].toInt(&conversionIsOk);
        if (false == conversionIsOk) {
            return false;
        }

        if (start > region.length) {
            return false;
        }
    }

    return true;
}


FormatCheckResult BedFormat::checkRawTextData(const QByteArray& rawData, const GUrl& /* = GUrl */) const
{
    const char* data = rawData.constData();
    int size = rawData.size();

    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    if (hasBinaryData) {
        return FormatDetection_NotMatched;
    }

    QString dataStr(rawData);
    QStringList fileLines = dataStr.split("\n");
    BEDLineValidateFlags validationStatus;

    int numToIterate;
    int HUGE_DATA = 65536;
    if (size < HUGE_DATA) {
        numToIterate = fileLines.size();
    }
    else {
        // Skip the last line as it can be incomplete
        numToIterate = fileLines.size() - 1;
    }

    bool trackLineDetected = false; // A line that starts with "track" keyword should be present
    int numberOfFieldsPerLine = 0;
    bool firstAnnotLine = true;
    for (int i = 0; i < numToIterate; ++i) {
        if (!fileLines[i].trimmed().isEmpty()) { // e.g. the last line in file can be empty

            QString line = fileLines[i];
            // Skip the header
            if (line.startsWith("browser")) {
                continue;
            }
            if (line.startsWith("track")) {
                trackLineDetected = true;
                continue;
            }

            // Validate other lines
            if (trackLineDetected) {

                // The number of fields per line is detected from the first line
                if (firstAnnotLine) {
                    firstAnnotLine = false;
                    numberOfFieldsPerLine = line.split("\t").count();
                    // There must be at least "chrom", "chromStart" and "chromEnd" fields
                    if (numberOfFieldsPerLine < 3) {
                        return FormatDetection_NotMatched;
                    }
                }

                // Parse a line
                BedFormatParser::parseAndValidateLine(line, numberOfFieldsPerLine, validationStatus);
            }
        }
    }
    validationStatus.hasTrackLine = trackLineDetected;

    return validationStatus.getFormatDetectionScore();
}

QList<SharedAnnotationData> BedFormat::getAnnotData(IOAdapter *io, U2OpStatus &os) {
    BedFormat bedFormat(NULL);
    QString annotName = "misc_feature";
    QList<SharedAnnotationData> res;
    BedFormatParser parser(io, annotName, os);
    const QHash<QString, QList<SharedAnnotationData> > &resHash
        = parser.parseDocument();
    CHECK_OP(os, res);
    foreach (const QString &seqName, resHash.keys()){
        res.append(resHash.value(seqName));
    }
    return res;
}

/**
 * Gets an attribute value in one of the following formats:
 *   1) name="value"
 *   2) name=value[SPACE CHARACTER]
 * If the value is found or the attribute is absent, returns true.
 * If the format of the value is incorrect, returns false.
 */
bool getAttributeValue(const QString& line, const QString& attrName, QString& attrValue)
{
    QString attrStr= attrName + "=";
    int attrIndex = line.indexOf(attrStr);

    if (-1 == attrIndex) {
        // The attribute is not found
        return true;
    }

    int attrBeginIndex = attrIndex + attrStr.size();

    if (line.size() == attrBeginIndex) {
        // Format is incorrect: attribute has name and assignment sign, but not the value
        return false;
    }

    bool parenthesisAreUsed;
    int attrEndIndex;
    if (line[attrBeginIndex] == '\"') {
        parenthesisAreUsed = true;
        attrEndIndex = line.indexOf("\"", attrBeginIndex + 1);
    }
    else {
        parenthesisAreUsed = false;
        attrEndIndex = line.indexOf(QRegExp("\\s"), attrIndex);
        if (-1 == attrEndIndex) {
            attrEndIndex = line.size();
        }
    }

    if (-1 != attrEndIndex) {
        if (parenthesisAreUsed) {
            attrBeginIndex++;
        }
        attrValue = line.mid(attrBeginIndex, attrEndIndex - attrBeginIndex);
        return true;
    }
    else {
        // Format is incorrect: there is no matching end character for the attribute
        return false;
    }
}


/** Get name and description from the track line */
bool parseTrackLine(const QString& trackLine, QString& trackName, QString& trackDescr)
{
    SAFE_POINT(trackLine.startsWith("track "), "Internal error while parsing track header line of a BED file:"
        " the line doesn't starts with 'track'!", false);

    bool attrFormatStatus = getAttributeValue(trackLine, "name", trackName);
    if (!attrFormatStatus) {
        return false;
    }

    attrFormatStatus = getAttributeValue(trackLine, "description", trackDescr);

    return attrFormatStatus;
}


void BedFormat::storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os)
{
    SAFE_POINT(NULL != doc, "Internal error: NULL document was provided to BEDFormat::storeDocument!",);
    SAFE_POINT(NULL != io, "Internal error: NULL IO adapter was provided to BEDFormat::storeDocument!",);

    ioLog.trace(tr("Starting BED saving: '%1'").arg(doc->getURLString()));

    QList<GObject*> annotTables = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);

    QByteArray lineData;

    int fieldsNumberPerLine = 0;
    bool firstLine = true;

    foreach (GObject* annotTableGObject, annotTables) {
        AnnotationTableObject* annotTable = qobject_cast<AnnotationTableObject *>(annotTableGObject);
        SAFE_POINT_EXT(annotTable != NULL, os.setError(tr("Can not convert GObject to AnnotationTableObject")),);

        QString chromName;
        QList<GObjectRelation> relations = annotTable->findRelatedObjectsByType(GObjectTypes::SEQUENCE);
        if (relations.size() == 1) {
            chromName = relations.first().ref.objName;
        } else {
            chromName = annotTable->getGObjectName();
            if (chromName.endsWith(QString(FEATURES_TAG))) {
                chromName.chop(QString(FEATURES_TAG).size());
            }
        }
        chromName.replace(' ', '_');
        if (chromName.isEmpty()) {
            ioLog.trace(tr("Can not detect chromosome name. 'Chr' name will be used."));
            chromName = "chr";
        }

        QList<Annotation *> annotationsList = annotTable->getAnnotations();

        foreach (Annotation *annot, annotationsList) {
            QString annotName = annot->getName();
            if (annotName == U1AnnotationUtils::lowerCaseAnnotationName ||
                annotName == U1AnnotationUtils::upperCaseAnnotationName)
            {
                continue;
            }

            QStringList lineFields;
            QVector<U2Region> annotRegions = annot->getRegions();
            if (annotRegions.size() > 1) {
                coreLog.info(tr("You are trying to save joined annotation to BED format! The joining will be lost"));
            }

            foreach (const U2Region& region, annotRegions) {
                lineFields << chromName;

                // chromStart and chromEnd
                lineFields << QString::number(region.startPos);
                lineFields << QString::number(region.endPos());

                QString nameQualValue = annot->findFirstQualifierValue(ANNOT_QUALIFIER_NAME);
                QString scoreQualValue = annot->findFirstQualifierValue(SCORE_QUALIFIER_NAME);
                QString strandQualValue = annot->findFirstQualifierValue(STRAND_QUALIFIER_NAME);
                QString thickStartQualValue = annot->findFirstQualifierValue(THICK_START_QUALIFIER_NAME);
                QString thickEndQualValue = annot->findFirstQualifierValue(THICK_END_QUALIFIER_NAME);
                QString itemRgbQualValue = annot->findFirstQualifierValue(ITEM_RGB_QUALIFIER_NAME);
                QString blockCountQualValue = annot->findFirstQualifierValue(BLOCK_COUNT_QUALIFIER_NAME);
                QString blockSizesQualValue = annot->findFirstQualifierValue(BLOCK_SIZES_QULAIFIER_NAME);
                QString blockStartsQualValue = annot->findFirstQualifierValue(BLOCK_STARTS_QUALIFIER_NAME);

                // Write the header, detect the number of optional fields from the first annotation
                if (firstLine) {
                    firstLine = false;
                    // Header
                    QString trackNameQualValue = annot->findFirstQualifierValue(TRACK_NAME_QUALIFIER_NAME);
                    QString trackDescrQualValue = annot->findFirstQualifierValue(TRACK_DESCR_QUALIFIER_NAME);

                    if (! (trackNameQualValue.isEmpty() || trackDescrQualValue.isEmpty())) {
                        QString headerStr = QString("track name=\"%1\" description=\"%2\"\n")
                            .arg(trackNameQualValue).arg(trackDescrQualValue);
                        QByteArray header = headerStr.toLatin1();
                        qint64 len = io->writeBlock(header);
                        if (len != header.size()) {
                            os.setError(L10N::errorWritingFile(doc->getURLString()));
                            return;
                        }
                    }

                    // Number of optional fields
                    // Note that the order of the optional fields is binding:
                    // lower-numbered fields must always be populated
                    // if higher-numbered fields are used
                    if (nameQualValue.isEmpty()) {
                        fieldsNumberPerLine = 3; // No default value, skip all optional fields
                    }
                    else {
                        fieldsNumberPerLine = 4;

                        // If score and strand qualifiers are not present, but further qualifiers are present,
                        // the line is filled with values "0" and "+" (or "-" depending on the annotation strand) at these positions
                        if (!scoreQualValue.isEmpty()) {
                            fieldsNumberPerLine = 5;
                        }
                        if (!strandQualValue.isEmpty()) {
                            fieldsNumberPerLine = 6;
                        }

                        // If thick coordinates qualifier are not present, the annotation
                        // region's coordinates are used
                        if (!thickStartQualValue.isEmpty()) {
                            if (!thickEndQualValue.isEmpty()) {
                                fieldsNumberPerLine = 8;
                            }
                            else {
                                os.setError(tr("BED saving error: incorrect thick coordinates"
                                    " in the first annotation!"));
                                return;
                            }
                        }

                        // Red color (255, 0, 0) is used by default for itemRgb
                        if (!itemRgbQualValue.isEmpty()) {
                            fieldsNumberPerLine = 9;
                        }

                        if (!blockCountQualValue.isEmpty()) {
                            if (blockStartsQualValue.isEmpty() ||
                                blockSizesQualValue.isEmpty()) {
                                    os.setError(tr("BED saving error: incorrect block fields"
                                               " in the first annotation!"));
                                    return;
                            }
                            else {
                                fieldsNumberPerLine = 12;
                            }
                        }
                    }
                    ioLog.trace(tr("BED saving: detected %1 fields per line"
                        " for file '%2'").arg(fieldsNumberPerLine).arg(doc->getURLString()));
                }

                // Append the required number of fields to the line
                if (fieldsNumberPerLine >= 4) {
                    if (nameQualValue.isEmpty()) {
                        os.setError(tr("BED saving error: an annotation is expected to have '%1'"
                                       " qualifier, but it is absent! Skipping the annotation.").arg(ANNOT_QUALIFIER_NAME));
                        return;
                    }
                    else {
                        lineFields << nameQualValue;
                    }
                }

                if (fieldsNumberPerLine >= 5) {
                    if (scoreQualValue.isEmpty()) {
                        lineFields << "0";
                    }
                    else {
                        lineFields << scoreQualValue;
                    }
                }

                if (fieldsNumberPerLine >= 6) {
                    if (strandQualValue.isEmpty()) {
                        U2Strand strand = annot->getStrand();
                        if (strand == U2Strand::Complementary) {
                            lineFields << "-";
                        }
                        else {
                            lineFields << "+";
                        }
                    }
                    else {
                        lineFields << strandQualValue;
                    }
                }

                if (fieldsNumberPerLine >= 8) {
                    if (thickStartQualValue.isEmpty() ||
                        thickEndQualValue.isEmpty()) {
                            // Write chromStart and chromEnd coordinates
                            lineFields << QString::number(region.startPos);
                            lineFields << QString::number(region.endPos());
                    }
                    else {
                        lineFields << thickStartQualValue;
                        lineFields << thickEndQualValue;
                    }
                }

                if (fieldsNumberPerLine >= 9) {
                    if (itemRgbQualValue.isEmpty()) {
                        lineFields << "255, 0, 0";
                    }
                    else {
                        lineFields << itemRgbQualValue;
                    }
                }

                if (fieldsNumberPerLine >= 12) {
                    if (blockCountQualValue.isEmpty() ||
                        blockStartsQualValue.isEmpty() ||
                        blockSizesQualValue.isEmpty())
                    {
                        os.setError(tr("BED saving error: an annotation is expected to have the block"
                                       " qualifiers! Skipping the annotation."));
                        return;
                    }
                    else {
                        lineFields << blockCountQualValue;
                        lineFields << blockSizesQualValue;
                        lineFields << blockStartsQualValue;
                    }
                }

                // Write the line
                lineData = lineFields.join("\t").toLatin1() + "\n";
                qint64 len = io->writeBlock(lineData);
                if (len != lineData.size()) {
                    os.setError(L10N::errorWritingFile(doc->getURLString()));
                    return;
                }
                lineFields.clear();
            }
        }
    }

    ioLog.trace(tr("Finished BED saving: '%1'").arg(doc->getURLString()));
}
//-------------------------------------------------------------------
//  BedFormatParser
//-------------------------------------------------------------------
const int BedFormatParser::BufferSize = 1024 * 4; // 4 Kb
const int BedFormatParser::MinimumColumnsNumber = 3; // "3" as there must be at least "chrom", "chromStart" and "chromEnd" fields

BedFormatParser::BedFormatParser(IOAdapter *io, const QString &defaultAnnotName, U2OpStatus &os)
    : io(io), os(os), defaultAnnotName(defaultAnnotName), buff(new char[BufferSize]), lineNumber(1), fileIsValid(true), noHeader(false){
}

QHash<QString, QList<SharedAnnotationData> > BedFormatParser::parseDocument() {
    QString seqName;
    QHash<QString, QList<SharedAnnotationData> > resultHash;
    QList<SharedAnnotationData> result;
    QString trackName;
    QString trackDescr;

    parseHeader(trackName, trackDescr);

    // Read other lines
    int numOfFieldsPerLine = 0;

    //we have already red the line if there is no header
    if (!noHeader){
        readLine();
    }

    while (curLine.length() > 0) {
        // Parse and validate the line
        BEDLineValidateFlags validationStatus;

        if (curLine.startsWith("#")){//skip comments
            os.setProgress(io->getProgress());
            readLine();
            continue;
        }
        if (1 == lineNumber) {
            numOfFieldsPerLine = curLine.split("\t").count();
            if (numOfFieldsPerLine < MinimumColumnsNumber) {
                os.setError(BedFormat::tr("BED parsing error: unexpected number of fields in the first annotations line!"));
                return resultHash;
            }
        }
        BedLineData bedLineData = parseAndValidateLine(curLine, numOfFieldsPerLine, validationStatus);
        bool isValidLine = checkAnnotationParsingErrors(validationStatus, bedLineData);

        // Check that an annotation can be created
        if (!isValidLine) {
            moveToNextLine();
            continue;
        }

        // If file is invalid, but can be parsed an error is written to the log,
        // all details are written to the trace log.
        if (validationStatus.isFileInvalid() && !noHeader) {
            fileIsValid = false;
        }

        if (!seqName.isEmpty()) {
            if (bedLineData.seqName != seqName) {
                addToResults(resultHash, result, seqName);
                seqName = bedLineData.seqName;
            }
        }
        else {
            seqName = bedLineData.seqName;
        }

        createAnnotation(bedLineData, result, trackName, trackDescr);

        moveToNextLine();
    }

    if (false == fileIsValid) {
        ioLog.info("BED parsing warning: one or more errors occurred while parsing the input file,"
            " see TRACE log for details!");
    }
    if (result.isEmpty()) {
        foreach(const QString& warning, os.getWarnings()) {
            ioLog.error(warning);
        }
        os.setError(BedFormat::tr("The file does not contain valid annotations!"));
    }
    else {
        addToResults(resultHash, result, seqName);
    }
    return resultHash;
}

#define CHECK_FIELD(INDEX) \
    if (fields.size()<INDEX || INDEX<0) { \
        status.incorrectNumberOfFields = true; \
        return parsedData; \
            }

BedLineData BedFormatParser::parseAndValidateLine(const QString& line, int numOfFields, BEDLineValidateFlags& status)
{
    BedLineData parsedData;

    // All fields are separated by a single tab
    QStringList fields = line.split("\t");

    // Verify that the line has the expected number of fields and they are not empty
    // Do not continue to validate the line if it is incorrect
    if (numOfFields != fields.count()) {
        status.incorrectNumberOfFields = true;
        return parsedData;
    }

    foreach(QString field, fields) {
        if (field.trimmed().isEmpty()) {
            status.emptyFields = true;
            return parsedData;
        }
    }

    // Coordinates
    // "start" can be zero, "end" is not included into the region
    bool startIsInt;
    bool endIsInt;
    CHECK_FIELD(BED_CHROM_START_INDEX);
    CHECK_FIELD(BED_CHROM_END_INDEX);
    qint64 start = fields[BED_CHROM_START_INDEX].toLongLong(&startIsInt);
    qint64 end = fields[BED_CHROM_END_INDEX].toLongLong(&endIsInt);
    if (!startIsInt || !endIsInt || (start < 0) || (start >= end)) {
        status.incorrectCoordinates = true;
        return parsedData;
    }

    // Fill in the data and continue validation even if a value is incorrect
    CHECK_FIELD(BED_CHROM_NAME_INDEX);
    parsedData.seqName = fields[BED_CHROM_NAME_INDEX];
    parsedData.region = U2Region(start, end - start);

    // Annotation name
    if (numOfFields > BED_ANNOT_NAME_INDEX) {
        CHECK_FIELD(BED_ANNOT_NAME_INDEX);
        parsedData.additionalFields[ANNOT_QUALIFIER_NAME] = fields[BED_ANNOT_NAME_INDEX];
    }

    // Score
    if (numOfFields > BED_SCORE_INDEX) {
        CHECK_FIELD(BED_SCORE_INDEX);
        QString scoreStr = fields[BED_SCORE_INDEX];
        parsedData.additionalFields[SCORE_QUALIFIER_NAME] = scoreStr;

        // Validate the value: it should be an integer value between 0 and 1000
        // UPDATE: for MACS peaks its wrong, so validation is removed
        bool scoreIsOk;
        scoreStr.toInt(&scoreIsOk);
        if (!scoreIsOk) {
            scoreStr.toDouble(&scoreIsOk);
        }
        if (!scoreIsOk) {
            status.incorrectScore = true;
        }
    }

    // Strand (either '+' or '-')
    if (numOfFields > BED_STRAND_INDEX) {
        CHECK_FIELD(BED_STRAND_INDEX);
        QString strandStr = fields[BED_STRAND_INDEX];
        parsedData.additionalFields[STRAND_QUALIFIER_NAME] = strandStr;

        if ("+" != strandStr &&
            "-" != strandStr) {
            status.incorrectStrand = true;
        }
    }

    // Thick coordinates
    if (numOfFields > BED_THICK_START_INDEX) {
        CHECK_FIELD(BED_THICK_START_INDEX);
        QString thickStartStr = fields[BED_THICK_START_INDEX];
        QString thickEndStr;

        // If thick start is set, thick end must also be set
        if (numOfFields <= BED_THICK_END_INDEX) {
            status.incorrectThickCoordinates = true;
        }
        else {
            CHECK_FIELD(BED_THICK_END_INDEX);
            thickEndStr = fields[BED_THICK_END_INDEX];
        }
        if (false == validateThickCoordinates(thickStartStr, thickEndStr)) {
            status.incorrectThickCoordinates = true;
        }

        parsedData.additionalFields[THICK_START_QUALIFIER_NAME] = thickStartStr;
        parsedData.additionalFields[THICK_END_QUALIFIER_NAME] = thickEndStr;
    }

    // Annotation color
    if (numOfFields > BED_ITEM_RGB_INDEX) {
        CHECK_FIELD(BED_ITEM_RGB_INDEX);
        QString itemRgbStr = fields[BED_ITEM_RGB_INDEX];
        parsedData.additionalFields[ITEM_RGB_QUALIFIER_NAME] = itemRgbStr;

        if (false == validateAnnotationColor(itemRgbStr, parsedData.annotColor)) {
            status.incorrectItemRgb = true;
        }
    }

    // Blocks (i.e. exons) parameters
    if (numOfFields > BED_BLOCK_COUNT_INDEX) {
        CHECK_FIELD(BED_BLOCK_COUNT_INDEX);
        QString blockCountStr = fields[BED_BLOCK_COUNT_INDEX];
        QString blockSizesStr;
        QString blockStartsStr;

        // If they are present, then all three value must be present in a line
        if (numOfFields <= BED_BLOCK_SIZES_INDEX) {
            status.incorrectBlocks = true;
        }
        else {
            CHECK_FIELD(BED_BLOCK_SIZES_INDEX);
            blockSizesStr = fields[BED_BLOCK_SIZES_INDEX];

            if (numOfFields <= BED_BLOCK_STARTS_INDEX) {
                status.incorrectBlocks = true;
            }
            else {
                CHECK_FIELD(BED_BLOCK_STARTS_INDEX);
                blockStartsStr = fields[BED_BLOCK_STARTS_INDEX];
            }
        }

        parsedData.additionalFields[BLOCK_COUNT_QUALIFIER_NAME] = blockCountStr;
        parsedData.additionalFields[BLOCK_SIZES_QULAIFIER_NAME] = blockSizesStr;
        parsedData.additionalFields[BLOCK_STARTS_QUALIFIER_NAME] = blockStartsStr;

        // Validate the values
        if (false == validateBlocks(blockCountStr, blockSizesStr, blockStartsStr, parsedData.region)) {
            status.incorrectBlocks = true;
        }
    }

    return parsedData;
}

void BedFormatParser::parseHeader(QString& trackName, QString& trackDescr) {
    // Parse and validate the header: ignore lines with "browser"
    // Search the 'track' line and get parameters from it
    bool headerLine = true;
    while (headerLine && readLine() > 0) {
        if (curLine.startsWith("#")){ //skip comments
            continue;
        }
        if (curLine.startsWith("browser")) {
            continue;
        }
        else if (curLine.startsWith("track")) {
            if (false == parseTrackLine(curLine, trackName, trackDescr)) {
                fileIsValid = false;
                ioLog.trace(BedFormat::tr("BED parsing error: incorrect format of the 'track' header line!"));
            }
            break; // Stop parsing the header when 'track' line has been detected
        }
        else {
            noHeader = true;
            break;
        }
    }
}


void BedFormatParser::createAnnotation(const BedLineData& bedLineData, QList<SharedAnnotationData>& result, QString& trackName, QString& trackDescr) {
    // Create the annotation
    SharedAnnotationData annotData(new AnnotationData());
    annotData->name = bedLineData.additionalFields[ANNOT_QUALIFIER_NAME].isEmpty()
        ? defaultAnnotName
        : bedLineData.additionalFields[ANNOT_QUALIFIER_NAME];
    annotData->location->regions << bedLineData.region;

    // Add qualifiers
    foreach(QString qualifierName, bedLineData.additionalFields.keys()) {
        if (!bedLineData.additionalFields.value(qualifierName).isEmpty()) {
            annotData->qualifiers.push_back(
                U2Qualifier(qualifierName, bedLineData.additionalFields.value(qualifierName)));
        }
    }

    // Add a qualifier with the sequence name
    annotData->qualifiers.push_back(U2Qualifier(CHROM_QUALIFIER_NAME, bedLineData.seqName));

    // Additionally, verify the strand information
    if (bedLineData.additionalFields.keys().contains(STRAND_QUALIFIER_NAME)) {
        if ("-" == bedLineData.additionalFields.value(STRAND_QUALIFIER_NAME)) {
            annotData->setStrand(U2Strand::Complementary);
        }
    }

    // Add track information to the qualifiers
    if (!trackName.isEmpty()) {
        annotData->qualifiers.push_back(U2Qualifier(TRACK_NAME_QUALIFIER_NAME, trackName));
    }

    if (!trackDescr.isEmpty()) {
        annotData->qualifiers.push_back(U2Qualifier(TRACK_DESCR_QUALIFIER_NAME, trackDescr));
    }

    // Append the result
    result.append(annotData);
}

void BedFormatParser::addToResults(QHash<QString, QList<SharedAnnotationData> > & resHash, QList<SharedAnnotationData>& result, const QString& seqName){
    QHash<QString, QList<SharedAnnotationData> >::iterator i = resHash.find(seqName);
    if (i != resHash.end()) {
        i.value().append(result);
    }
    else{
        resHash.insert(seqName, result);
    }
    result.clear();
}

namespace {
const int maxStringLength = 100;
QString getAbridgedString(const QString& value) {
    QString resultString = value.left(maxStringLength);
    resultString += (value.length() > 100) ? "..." : "";
    return resultString;
}
}

bool BedFormatParser::checkAnnotationParsingErrors(const BEDLineValidateFlags& validationStatus, const BedLineData& bedLineData) {
    // If there were some errors during parsing the output, write it to the log
    if (validationStatus.incorrectNumberOfFields) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect number of fields at line %1!").arg(lineNumber));
        return false;
    }
    if (validationStatus.emptyFields) {
        os.addWarning(BedFormat::tr("BED parsing error: a field at line %1 is empty!").arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectCoordinates) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect coordinates at line %1!").arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectScore) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect score value '%1'"
            " at line %2!").arg(getAbridgedString(bedLineData.additionalFields[SCORE_QUALIFIER_NAME])).arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectStrand) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect strand value '%1'"
            " at line %2!").arg(getAbridgedString(bedLineData.additionalFields[STRAND_QUALIFIER_NAME])).arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectThickCoordinates) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect thick coordinates at line %1!").arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectItemRgb) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect itemRgb value '%1'"
            " at line %2!").arg(getAbridgedString(bedLineData.additionalFields[ITEM_RGB_QUALIFIER_NAME])).arg(lineNumber));
        return false;
    }
    if (validationStatus.incorrectBlocks) {
        os.addWarning(BedFormat::tr("BED parsing error: incorrect value of the block parameters"
            " at line %1!").arg(lineNumber));
        return false;
    }
    return true;
}

int BedFormatParser::readLine() {
    int len = 0;
    curLine.clear();
    do {
        len = io->readLine(buff.data(), BufferSize - 1);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), -1);

        buff.data()[len] = '\0';
        curLine.append(QString(buff.data()));
    } while (len == BufferSize - 1);
    return curLine.length();
}

void BedFormatParser::moveToNextLine() {
    lineNumber++;
    os.setProgress(io->getProgress());
    readLine();
}


} // namespace U2
