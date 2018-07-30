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

#include <QScopedArrayPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include "FpkmTrackingFormat.h"

namespace U2 {

//-------------------------------------------------------------------
// FpkmTrackingLineValidateFlags
//-------------------------------------------------------------------
FpkmTrackingLineValidateFlags::FpkmTrackingLineValidateFlags()
     : emptyField(false),
       incorrectNumberOfFields(false),
       incorrectCoordinates(false),
       emptyTrackingId(false),
       incorrectLength(false),
       incorrectCoverage(false)
{
}


FormatDetectionScore FpkmTrackingLineValidateFlags::getFormatDetectionScore()
{
    if (emptyField || incorrectNumberOfFields || incorrectCoordinates) {
        return FormatDetection_NotMatched;
    }

    if (emptyTrackingId) {
        return FormatDetection_HighSimilarity;
    }

    if (incorrectLength || incorrectCoverage) {
        return FormatDetection_VeryHighSimilarity;
    }

    return FormatDetection_Matched;
}


//-------------------------------------------------------------------
//  FPKMTrackingFormat
//-------------------------------------------------------------------
const QString FpkmTrackingFormat::FORMAT_NAME = QObject::tr("FPKM Tracking Format");

const QString FpkmTrackingFormat::NO_VALUE_STR = "-";

const QString FpkmTrackingFormat::TRACKING_ID_COLUMN = "tracking_id";
const QString FpkmTrackingFormat::CLASS_CODE_COLUMN = "class_code";
const QString FpkmTrackingFormat::NEAREST_REF_ID_COLUMN = "nearest_ref_id";
const QString FpkmTrackingFormat::GENE_ID_COLUMN = "gene_id";
const QString FpkmTrackingFormat::GENE_SHORT_NAME_COLUMN = "gene_short_name";
const QString FpkmTrackingFormat::TSS_ID_COLUMN = "tss_id";
const QString FpkmTrackingFormat::LOCUS_COLUMN = "locus";
const QString FpkmTrackingFormat::LENGTH_COLUMN = "length";
const QString FpkmTrackingFormat::COVERAGE_COLUMN = "coverage";


FpkmTrackingFormat::FpkmTrackingFormat(QObject* parent)
    : TextDocumentFormat(parent, DocumentFormatFlag_SupportWriting, QStringList("fpkm_tracking"))
{
    formatDescription = tr("The FPKM (fragments per kilobase of exon model per million mapped fragments)"
        " Tracking Format is a native Cufflinks format to output estimated expression values.");

    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
}


Document* FpkmTrackingFormat::loadTextDocument(IOAdapter* io, const U2DbiRef&  dbiRef, const QVariantMap& hints, U2OpStatus& os)
{
    CHECK_EXT(io != NULL && io->isOpen(), os.setError(L10N::badArgument("IO adapter")), NULL);
    QList<GObject*> objects;

    load(io, objects, dbiRef, hints, os);
    CHECK_OP_EXT(os, qDeleteAll(objects), NULL);

    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects);
    return doc;
}


int readFpkmTrLine(QString &buffer, IOAdapter* io, QScopedArrayPointer<char> &charbuff) {
    int len;
    buffer.clear();
    do {
        len = io->readLine(charbuff.data(), DocumentFormat::READ_BUFF_SIZE - 1);
        charbuff.data()[len] = '\0';
        buffer.append(QString(charbuff.data()));
    } while (DocumentFormat::READ_BUFF_SIZE - 1 == len);
    return buffer.length();
}

void FpkmTrackingFormat::addQualifierIfValuePresent(SharedAnnotationData &annotData, const QString &name, const QString &val) {
    if (NO_VALUE_STR != val) {
        U2Qualifier qual(name, val);
        SAFE_POINT(qual.isValid(), tr("Internal error: qualifier with name '%1' and '%2' can't be added").arg(name).arg(val), );
        annotData->qualifiers.push_back(qual);
    }
}

QList<SharedAnnotationData> FpkmTrackingFormat::parseDocument(IOAdapter* io, QString& seqName, QString annotName, U2OpStatus &os) {
    QList<SharedAnnotationData> result;

    int length;
    QScopedArrayPointer<char> buff(new char[READ_BUFF_SIZE]);
    QString qstrbuf;

    // Validate the header
    length = readFpkmTrLine(qstrbuf, io, buff);
    if (0 == length) {
        return result;
    }
    QStringList columnsNames;
    parseHeader(qstrbuf, columnsNames);

    // Validate all other lines
    bool fileIsValid = true;
    int lineNumber = 1;

    while ((length = readFpkmTrLine(qstrbuf, io, buff)) > 0) {
        // Parse and validate the line
        FpkmTrackingLineValidateFlags validationStatus;
        FpkmTrackingLineData fpkmTrLineData = parseAndValidateLine(qstrbuf, columnsNames, validationStatus);

        // Check that an annotation can be created
        if (validationStatus.isIncorrectNumberOfFields()) {
            os.setError(tr("FPKM Tracking Format parsing error: incorrect number of fields at line %1!").arg(lineNumber));
            return result;
        }

        if (validationStatus.isEmptyField()) {
            os.setError(tr("FPKM Tracking Format parsing error: a field at line %1 is empty!").arg(lineNumber));
            return result;
        }

        if (validationStatus.isIncorrectCoordinates()) {
            os.setError(tr("FPKM Tracking Format parsing error: incorrect coordinates at line %1!").arg(lineNumber));
        }

        // If file is invalid, but can be parsed an error is written to the log,
        // all details are written to the trace log.
        if (validationStatus.isFileInvalid()) {
            fileIsValid = false;
        }

        // Verify the sequence name (error doesn't occur, just warning)
        if (!seqName.isEmpty()) {
            if (fpkmTrLineData.seqName != seqName) {
                ioLog.trace(tr("FPKM Tracking Format parsing error: different sequence names were detected"
                    " in an input file. Sequence name '%1' is used.").arg(seqName));
            }
        }
        else {
            seqName = fpkmTrLineData.seqName;
        }

        // Create the annotation
        SharedAnnotationData annotData(new AnnotationData);
        annotData->name = annotName;
        annotData->location->regions << fpkmTrLineData.region;

        // Add qualifiers
        if (validationStatus.isEmptyTrackingId()) {
            // Write the error to the log, but open the file
            ioLog.trace(tr("FPKM Tracking Format parsing error: tracking ID"
                " value is empty at line %1!").arg(lineNumber));
        } else {
            annotData->qualifiers.push_back(
                U2Qualifier(TRACKING_ID_COLUMN, fpkmTrLineData.trackingId));
        }

        annotData->qualifiers.push_back(U2Qualifier(LOCUS_COLUMN, fpkmTrLineData.locus));

        addQualifierIfValuePresent(annotData, CLASS_CODE_COLUMN, fpkmTrLineData.classCode);
        addQualifierIfValuePresent(annotData, NEAREST_REF_ID_COLUMN, fpkmTrLineData.nearestRefId);
        addQualifierIfValuePresent(annotData, GENE_ID_COLUMN, fpkmTrLineData.geneId);
        addQualifierIfValuePresent(annotData, GENE_SHORT_NAME_COLUMN, fpkmTrLineData.geneShortName);
        addQualifierIfValuePresent(annotData, TSS_ID_COLUMN, fpkmTrLineData.tssId);
        addQualifierIfValuePresent(annotData, LENGTH_COLUMN, fpkmTrLineData.length);
        addQualifierIfValuePresent(annotData, COVERAGE_COLUMN, fpkmTrLineData.coverage);

        foreach (QString column, fpkmTrLineData.otherFields.keys()) {
            addQualifierIfValuePresent(annotData, column, fpkmTrLineData.otherFields.value(column));
        }

        // Additional warnings
        if (validationStatus.isIncorrectLength()) {
            ioLog.trace(tr("FPKM Tracking Format parsing error: incorrect"
                " length value at line %1!").arg(lineNumber));
        }

        if (validationStatus.isIncorrectCoverage()) {
            ioLog.trace(tr("FPKM Tracking Format parsing error: incorrect"
                " coverage value at line %1!").arg(lineNumber));
        }

        // Append the result
        result.append(annotData);

        // Move to the next line
        lineNumber++;
    }

    if (false == fileIsValid) {
        ioLog.error("FPKM Tracking Format parsing error: one or more errors occurred while parsing the input file,"
            " see TRACE log for details!");
    }

    return result;
}

void FpkmTrackingFormat::load(IOAdapter* io, QList<GObject*>& objects, const U2DbiRef& dbiRef, const QVariantMap &hints, U2OpStatus& os)
{
    QString sequenceName;
    QString annotName = "misc_feature";
    QList<SharedAnnotationData> annotations = parseDocument(io, sequenceName, annotName, os);
    QMap<AnnotationTableObject *, QMap<QString, QList<SharedAnnotationData> > > annTable2anns;

    foreach (const SharedAnnotationData &annotData, annotations) {
        QString annotTableName = sequenceName + FEATURES_TAG;
        AnnotationTableObject *annotTable = NULL;
        foreach (GObject* object, objects) {
            if (object->getGObjectName() == annotTableName) {
                annotTable = dynamic_cast<AnnotationTableObject *>(object);
            }
        }
        if (!annotTable) {
            QVariantMap objectHints;
            objectHints.insert(DBI_FOLDER_HINT, hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
            annotTable = new AnnotationTableObject(annotTableName, dbiRef, objectHints);
            objects.append(annotTable);
        }

        // Assume that the group name is the same as the annotation name
        QString groupName = annotName;
        if (!AnnotationGroup::isValidGroupName(groupName, false)) {
            groupName = "Group"; // or set this name if the annotation name is not appropriate
        }

        annTable2anns[annotTable][groupName].append(annotData);
    }

    foreach (AnnotationTableObject *ato, annTable2anns.keys()) {
        foreach (const QString &groupName, annTable2anns[ato].keys()) {
            ato->addAnnotations(annTable2anns[ato][groupName], groupName);
        }
    }
}

bool FpkmTrackingFormat::parseHeader(const QString& headerLine, QStringList& columns) const
{
    // All fields are separated by TAB
    QStringList fields = headerLine.split("\t");

    columns << TRACKING_ID_COLUMN
        << CLASS_CODE_COLUMN
        << NEAREST_REF_ID_COLUMN
        << GENE_ID_COLUMN
        << GENE_SHORT_NAME_COLUMN
        << TSS_ID_COLUMN
        << LOCUS_COLUMN
        << LENGTH_COLUMN
        << COVERAGE_COLUMN;

    int columnsWithFixedNamesCount = columns.size();

    for (int i = 0; i < fields.size(); ++i) {
        if (i < columnsWithFixedNamesCount) {
            // Fields "tracking_id", ..., "coverage"
            if (columns[i] != fields[i]) {
                return false;
            }
        }
        else {
            if (!fields[i].trimmed().isEmpty()) {
                // Commonly, the fields could be: "FPKM", "FPKM_lo", "FPKM_hi", "FPKM_status"
                // But also could be "status", "q0_FPKM", "q0_FPKM_lo", ... "qN_FPKM", ... (for different samples)
                columns << fields[i];
            }
        }
    }
    return true;
}


/** Locus have the following format: "<chromosome or genome name>:<start>-<end>" */
bool parseLocus(QString locus, QString& seqName, U2Region& region)
{
    int lastColonCharIndex = locus.lastIndexOf(':');
    if (-1 == lastColonCharIndex) {
        return false;
    }

    seqName = locus.left(lastColonCharIndex);

    QString coordinatesStr = locus.mid(lastColonCharIndex + 1); // index is the next after ':'
    QStringList coordinates = coordinatesStr.split('-');
    if (2 != coordinates.size()) {
        return false;
    }

    bool startIsInt;
    bool endIsInt;
    int start = coordinates[0].toInt(&startIsInt);
    int end = coordinates[1].toInt(&endIsInt);
    if (!startIsInt || !endIsInt || (start < 1) || (start > end)) {
        return false;
    }

    region = U2Region(start - 1, end - start + 1);
    return true;
}


FpkmTrackingLineData FpkmTrackingFormat::parseAndValidateLine(QString line, QStringList columns, FpkmTrackingLineValidateFlags& status) const
{
    FpkmTrackingLineData parsedData;

    // All fields are separated by TAB
    QStringList fields = line.split("\t");

    // Number of fields in the line must be equal to the columns number in the header
    if (fields.size() != columns.size()) {
        status.setFlagIncorrectNumberOfFields();
        return parsedData;
    }

    // Fields must not be empty
    foreach (QString field, fields) {
        if (field.trimmed().isEmpty()) {
            status.setFlagEmptyField();
            return parsedData;
        }
    }

    // Coordinates
    parsedData.locus = fields[FPKM_LOCUS_INDEX];

    if (!parseLocus(fields[FPKM_LOCUS_INDEX], parsedData.seqName, parsedData.region)) {
        status.setFlagIncorrectCoordinates();
        return parsedData;
    }

    // Fill in the data and continue validation even if a value is incorrect
    parsedData.trackingId = fields[FPKM_TRACKING_ID_INDEX];
    parsedData.classCode = fields[FPKM_CLASS_CODE_INDEX];
    parsedData.nearestRefId = fields[FPKM_NEAREST_REF_ID_INDEX];
    parsedData.geneId = fields[FPKM_GENE_ID_INDEX];
    parsedData.geneShortName = fields[FPKM_GENE_SHORT_NAME_INDEX];
    parsedData.tssId = fields[FPKM_TSS_ID_INDEX];
    parsedData.length = fields[FPKM_LENGTH_INDEX];
    parsedData.coverage = fields[FPKM_COVERAGE_INDEX];

    // If there are additional fields, store their values
    for (int i = FPKM_FIELDS_FIRST_INDEX; i < fields.size(); ++i) {
        QString columnName = columns[i];
        parsedData.otherFields[columnName] = fields[i];
    }

    // Validate some values
    // Tracking ID
    if (NO_VALUE_STR == parsedData.trackingId) {
        status.setFlagEmptyTrackingId();
    }

    // Length
    if (NO_VALUE_STR != parsedData.length) {
        bool lengthIsInt;
        parsedData.length.toInt(&lengthIsInt);
        if (!lengthIsInt) {
            status.setFlagIncorrectLength();
        }
    }

    // Coverage
    if (NO_VALUE_STR != parsedData.coverage) {
        bool coverageIsDouble;
        parsedData.coverage.toDouble(&coverageIsDouble);
        if (!coverageIsDouble) {
            status.setFlagIncorrectCoverage();
        }
    }

    return parsedData;
}

FormatCheckResult FpkmTrackingFormat::checkRawTextData(const QByteArray& rawData, const GUrl& /* = GUrl */) const
{
    const char* data = rawData.constData();
    int size = rawData.size();

    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    if (hasBinaryData) {
        return FormatDetection_NotMatched;
    }
    QString dataStr(rawData);
    QStringList fileLines = dataStr.split("\n");

    if (fileLines.isEmpty()) {
        return FormatDetection_NotMatched;
    }

    QString header = fileLines[0];

    FpkmTrackingLineValidateFlags validationStatus;
    QStringList columnsNames;
    if (!parseHeader(header, columnsNames)) {
        return FormatDetection_NotMatched;
    }
    else {
        int numToIterate;
        int HUGE_DATA = 65536;
        if (size < HUGE_DATA) {
            numToIterate = fileLines.size();
        }
        else {
            // Skip the last line as it can be incomplete
            numToIterate = fileLines.size() - 1;
        }

        for (int i = 1; i < numToIterate; ++i) {
            if (!fileLines[i].isEmpty()) {
                parseAndValidateLine(fileLines[i], columnsNames, validationStatus);
            }
        }
    }

    return validationStatus.getFormatDetectionScore();
}


QStringList FpkmTrackingFormat::writeHeader(QList<GObject*> annotTables, Document* doc, IOAdapter* io, U2OpStatus& os)
{
    QStringList columns;
    columns << TRACKING_ID_COLUMN
        << CLASS_CODE_COLUMN
        << NEAREST_REF_ID_COLUMN
        << GENE_ID_COLUMN
        << GENE_SHORT_NAME_COLUMN
        << TSS_ID_COLUMN
        << LOCUS_COLUMN
        << LENGTH_COLUMN
        << COVERAGE_COLUMN;

    foreach (GObject* annotTable, annotTables) {
        AnnotationTableObject *annTable = dynamic_cast<AnnotationTableObject *>(annotTable);
        QList<Annotation *> annotationsList = annTable->getAnnotations();

        foreach (Annotation *annot, annotationsList) {
            QString annotName = annot->getName();
            if (annotName == U1AnnotationUtils::lowerCaseAnnotationName || annotName == U1AnnotationUtils::upperCaseAnnotationName) {
                continue;
            }

            QVector<U2Qualifier> annotQualifiers = annot->getQualifiers();
            foreach (U2Qualifier qualifier, annotQualifiers) {
                if (!columns.contains(qualifier.name)) {
                    QString qualName = qualifier.name;
                    if (qualName == "status" || qualName.contains("FPKM", Qt::CaseInsensitive)) {
                        // Additionally, column "samplename_FPKM_lo" should go before column "samplename_FPKM_hi"
                        if (qualName.contains("FPKM_conf_lo") || qualName.contains("FPKM_lo")) {

                            // Try to replace both variants
                            QString fpkmHiForQualName = qualName;
                            fpkmHiForQualName.replace("FPKM_conf_lo", "FPKM_conf_hi");
                            fpkmHiForQualName.replace("FPKM_lo", "FPKM_hi");

                            int hiColumnIndex = columns.indexOf(fpkmHiForQualName);
                            if (-1 != hiColumnIndex) {
                                // So, insert the "samplename_FPKM_low" before
                                columns.insert(hiColumnIndex, qualName);
                            }
                            else {
                                // Otherwise append the new column to the end
                                columns << qualName;
                            }
                        }
                        else {
                            columns << qualName;
                        }
                    }
                    else {
                        ioLog.trace(tr("Skipped qualifier '%1' while saving a FPKM header.").arg(qualName));
                    }
                }
            }

            // Write the line
            QByteArray lineData = columns.join("\t").toLatin1() + "\n";
            qint64 len = io->writeBlock(lineData);
            if (len != lineData.size()) {
                os.setError(L10N::errorWritingFile(doc->getURL()));
            }

            return columns;
        }
    }

    return columns;
}


void FpkmTrackingFormat::storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os)
{
    SAFE_POINT(NULL != doc, "Internal error: NULL Document during saving a FPKM Tracking Format file!",);
    SAFE_POINT(NULL != io, "Internal error: NULL IOAdapter during saving a FPKM Tracking Format file!",);

    bool noErrorsDuringStoring = true;
    QList<GObject*> annotTables = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    if (annotTables.isEmpty()) {
        return;
    }

    // Write the header
    QStringList columns = writeHeader(annotTables, doc, io, os);
    CHECK_OP(os,);

    // Go through all annotations, writing each to the file
    QByteArray lineData;

    foreach (GObject* annotTable, annotTables) {
        // Get the associated sequence name (to restore or verify locus)
        QString seqName;
        QList<GObjectRelation> rels = annotTable->findRelatedObjectsByRole(ObjectRole_Sequence);
        if (!rels.isEmpty()) {
            const GObjectRelation& rel = rels.first();
            seqName = rel.ref.objName;
        }

        // Get the annotations
        AnnotationTableObject *annTable = dynamic_cast<AnnotationTableObject *>(annotTable);
        QList<Annotation *> annotationsList = annTable->getAnnotations();

        foreach (Annotation *annot, annotationsList) {
            QString annotName = annot->getName();
            if (annotName == U1AnnotationUtils::lowerCaseAnnotationName ||
                annotName == U1AnnotationUtils::upperCaseAnnotationName)
            {
                continue;
            }

            QVector<U2Region> annotRegions = annot->getRegions();

            foreach (const U2Region& region, annotRegions) {

                // Fill in the values from the annotation qualifiers
                QStringList lineFields;
                foreach (const QString &columnName, columns) {
                    QString columnValue = annot->findFirstQualifierValue(columnName);

                    // Also, validate some fields
                    if ((TRACKING_ID_COLUMN == columnName) && (columnValue.isEmpty())) {
                        ioLog.trace(tr("FPKM Tracking Format saving error: tracking ID"
                            " shouldn't be empty!"));
                        noErrorsDuringStoring = false;
                    }

                    if (LOCUS_COLUMN == columnName) {
                        // If there is no "locus" qualifier, restore the column value
                        if (columnValue.isEmpty()) {
                            if (seqName.isEmpty()) {
                                columnValue = "unknown_genome"; // use some name
                            }
                            else {
                                columnValue = seqName;
                            }

                            columnValue += ":";
                            columnValue += region.startPos;
                            columnValue += "-";
                            columnValue += region.endPos();
                        }
                        // Otherwise verify the qualifier
                        else {
                            QString seqNameFromLocusQual; // Currently, do not verify a sequence name in locus!
                            U2Region regionFromLocusQual;
                            if (!parseLocus(columnValue, seqNameFromLocusQual, regionFromLocusQual)) {
                                ioLog.trace(tr("FPKM Tracking Format saving error: failed"
                                    " to parse locus qualifier '%1', writing it"
                                    " to the output file anyway!").arg(columnValue));
                                noErrorsDuringStoring = false;
                            }

                            if (regionFromLocusQual != region) {
                                ioLog.trace(tr("FPKM Tracking Format saving error: an annotation"
                                    " region (%1, %2) differs from the information stored in the 'locus'"
                                    " qualifier (%3, %4). Writing the 'locus' qualifier to output!")
                                    .arg(QString::number(region.startPos)
                                    .arg(QString::number(region.endPos())
                                    .arg(QString::number(regionFromLocusQual.startPos))
                                    .arg(QString::number(regionFromLocusQual.endPos())))));
                                noErrorsDuringStoring = false;
                            }
                        }
                    }

                    // Use an appropriate string when there is no value
                    if (columnValue.isEmpty()) {
                        columnValue = NO_VALUE_STR;
                    }

                    // Append the value
                    lineFields << columnValue;
                }

                // Write the line
                lineData = lineFields.join("\t").toLatin1() + "\n";
                qint64 len = io->writeBlock(lineData);
                if (len != lineData.size()) {
                    os.setError(L10N::errorWritingFile(doc->getURL()));
                    return;
                }

            }
        }
    }

    if (!noErrorsDuringStoring) {
        ioLog.error(tr("FPKM Tracking Format saving error: one or more errors occurred while saving a file,"
            " see TRACE log for details!"));
    }
}

} // namespace U2
