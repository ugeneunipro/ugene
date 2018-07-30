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

#ifndef _U2_FPKM_TRACKING_FORMAT_H_
#define _U2_FPKM_TRACKING_FORMAT_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2Region.h>

#include "TextDocumentFormat.h"

namespace U2 {


/** Validates a line from a file in FPKM Tracking Format */
class FpkmTrackingLineValidateFlags
{
public:
    FpkmTrackingLineValidateFlags();

    void setFlagEmptyField() { emptyField = true; }
    void setFlagIncorrectNumberOfFields() { incorrectNumberOfFields = true; }
    void setFlagIncorrectCoordinates() { incorrectCoordinates = true; }
    void setFlagEmptyTrackingId() { emptyTrackingId = true; }
    void setFlagIncorrectLength() { incorrectLength = true; }
    void setFlagIncorrectCoverage() { incorrectCoverage = true; }

    FormatDetectionScore getFormatDetectionScore();

    bool isFileInvalid() {
        return emptyField ||
            incorrectNumberOfFields ||
            incorrectCoordinates ||
            emptyTrackingId ||
            incorrectLength ||
            incorrectCoverage;
    }

    bool isEmptyField() { return emptyField; }
    bool isIncorrectNumberOfFields() { return incorrectNumberOfFields; }
    bool isIncorrectCoordinates() { return incorrectCoordinates; }
    bool isEmptyTrackingId() { return emptyTrackingId; }
    bool isIncorrectLength() { return incorrectLength; }
    bool isIncorrectCoverage() { return incorrectCoverage; }

private:
    bool emptyField;
    bool incorrectNumberOfFields; // must be equal to the number of columns in the header line
    bool incorrectCoordinates; // coordinates are parsed from locus
    bool emptyTrackingId; // must be a unique string, but it is only verified that it is not '-'
    bool incorrectLength; // must be '-' or an integer value
    bool incorrectCoverage; // must be '-' or a double value
};


struct FpkmTrackingLineData
{
    QString trackingId;
    QString classCode;
    QString nearestRefId;
    QString geneId;
    QString geneShortName;
    QString tssId;
    QString locus;
    QString seqName; // from locus
    U2Region region; // from locus
    QString length;
    QString coverage;
    QMap<QString, QString> otherFields;
};


class IOAdapter;

enum FpkmTrackingLineFieldsIndeces {FPKM_TRACKING_ID_INDEX = 0, FPKM_CLASS_CODE_INDEX = 1,
    FPKM_NEAREST_REF_ID_INDEX = 2, FPKM_GENE_ID_INDEX = 3,
    FPKM_GENE_SHORT_NAME_INDEX = 4, FPKM_TSS_ID_INDEX = 5,
    FPKM_LOCUS_INDEX = 6, FPKM_LENGTH_INDEX = 7,
    FPKM_COVERAGE_INDEX = 8, FPKM_FIELDS_FIRST_INDEX = 9};


/**
 * Description of the format from the Cufflinks manual was used:
 * http://cufflinks.cbcb.umd.edu/manual.html#fpkm_tracking_format
 */
class U2FORMATS_EXPORT FpkmTrackingFormat : public TextDocumentFormat
{
    Q_OBJECT

public:
    FpkmTrackingFormat(QObject* parent);

    virtual DocumentFormatId getFormatId() const { return BaseDocumentFormats::FPKM_TRACKING_FORMAT; }

    virtual const QString& getFormatName() const { return FORMAT_NAME; }

    virtual void storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;

    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os);

    QList<SharedAnnotationData> parseDocument(IOAdapter* io, QString& seqName, QString annotName, U2OpStatus& os);

    void load(IOAdapter* io, QList<GObject*>& objects, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os);

private:
    FpkmTrackingLineData parseAndValidateLine(QString line, QStringList columns, FpkmTrackingLineValidateFlags& status) const;

    static const QString FORMAT_NAME;

    static const QString NO_VALUE_STR;

    static const QString TRACKING_ID_COLUMN;
    static const QString CLASS_CODE_COLUMN;
    static const QString NEAREST_REF_ID_COLUMN;
    static const QString GENE_ID_COLUMN;
    static const QString GENE_SHORT_NAME_COLUMN;
    static const QString TSS_ID_COLUMN;
    static const QString LOCUS_COLUMN;
    static const QString LENGTH_COLUMN;
    static const QString COVERAGE_COLUMN;

    /**
    * Parses and validates the header line.
    * If the header is appropriate returns true and the columns list contains the columns names.
    * Otherwise returns "false".
    */
    bool parseHeader(const QString& headerLine, QStringList& columns) const;

    /**
    * Qualifiers from the FIRST FOUND annotation are used to restore names of "additional"
    * columns in the FPKM Tracking Format file (like "FPKM status", etc.).
    * Also:
    * 1) Only qualifiers that contain "FPKM" or that equal to "status" are taken into account,
    * other values are ignored.
    * 2) It is verified that a "samplename_FPKM_lo" (or "samplename_FPKM_conf_lo") column
    * goes before a "samplename_FPKM_hi" (or "samplename_FPKM_conf_hi") column.
    * The function returns the list of all columns names.
    */
    QStringList writeHeader(QList<GObject*> annotTables, Document* doc, IOAdapter* io, U2OpStatus& os);

    void addQualifierIfValuePresent(SharedAnnotationData &annotData, const QString &name, const QString &val);
};


} // namespace

#endif


