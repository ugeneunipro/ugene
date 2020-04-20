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

#ifndef _U2_GTF_FORMAT_H_
#define _U2_GTF_FORMAT_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2Region.h>

#include "TextDocumentFormat.h"

namespace U2 {

/** Validates a line from a GTF file */
class GTFLineValidateFlags
{
public:
    GTFLineValidateFlags();

    void setFlagIncorrectNumberOfFields() { incorrectNumberOfFields = true; }
    void setFlagEmptyField() { emptyField = true; }
    void setFlagIncorrectCoordinates() { incorrectCoordinates = true; }
    void setFlagIncorrectScore() { incorrectScore = true; }
    void setFlagIncorrectStrand() { incorrectStrand = true; }
    void setIncorrectFrame() { incorrectFrame = true; }
    void setFlagNoGeneIdAttribute() { noGeneIdAttribute = true; }
    void setFlagNoTrascriptIdAttribute() { noTranscriptIdAttribute = true; }
    void setFlagIncorrectFormatOfAttributes() { incorrectFormatOfAttributes = true; }

    FormatDetectionScore getFormatDetectionScore();

    bool isFileInvalid() {
        return incorrectNumberOfFields ||
            emptyField ||
            incorrectCoordinates ||
            incorrectScore ||
            incorrectStrand ||
            incorrectFrame ||
            incorrectFormatOfAttributes;
    }

    bool isIncorrectNumberOfFields() { return incorrectNumberOfFields; }
    bool isEmptyField() { return emptyField; }
    bool isIncorrectCoordinates() { return incorrectCoordinates; }
    bool isIncorrectScore() { return incorrectScore; }
    bool isIncorrectStrand() { return incorrectStrand; }
    bool isIncorrectFrame() { return incorrectFrame; }
    bool isGeneIdAbsent() { return noGeneIdAttribute; }
    bool isTranscriptIdAbsent() { return noTranscriptIdAttribute; }
    bool isIncorrectFormatOfAttributes() { return incorrectFormatOfAttributes; }

private:
    bool incorrectNumberOfFields; // There should be 9 fields
    bool emptyField; // Each field shouldn't be empty or shouldn't consist of white spaces
    bool incorrectCoordinates; // Start and end should be integer, start should be <= end
    bool incorrectScore; // Should be float, or integer, or a dot('.'), i.e. empty
    bool incorrectStrand; // Should be '+', '-', or '.'
    bool incorrectFrame; // Should be 0, or 1, or 2, or '.'
    bool noGeneIdAttribute; // "gene_id" attribute is required
    bool noTranscriptIdAttribute; // "transcript_id" attribute is required
    bool incorrectFormatOfAttributes; // Attributes must end in a semicolon which must then be separated
                                      // from the start of any subsequent attribute by exactly one space
                                      // character (NOT a tab character). Textual attributes should be
                                      // surrounded by double quotes.
};


struct GTFLineData
{
    QString seqName;
    QString source;
    QString feature;
    U2Region region;
    QString score;
    QString strand;
    QString frame;
    QMap<QString, QString> attributes;
};


enum GTFLineFieldsIndeces {GTF_SEQ_NAME_INDEX = 0, GTF_SOURCE_INDEX = 1, GTF_FEATURE_INDEX = 2,
    GTF_START_INDEX = 3, GTF_END_INDEX = 4, GTF_SCORE_INDEX = 5, GTF_STRAND_INDEX = 6,
    GTF_FRAME_INDEX = 7, GTF_ATTRIBUTES_INDEX = 8};


class IOAdapter;

/**
 * The following GTF specification was used: http://mblab.wustl.edu/GTF22.html
 * Unlike GFF, we assume that there is no embedded sequence in a GTF file, only annotations (not joined)
 */
class U2FORMATS_EXPORT GTFFormat : public TextDocumentFormat {
    Q_OBJECT

public:
    GTFFormat(QObject* parent);

    virtual void storeDocument(Document* doc, IOAdapter* io, U2OpStatus& os);

protected:
    virtual FormatCheckResult checkRawTextData(const QByteArray& rawData, const GUrl& = GUrl()) const;

    virtual Document* loadTextDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os);

private:
    GTFLineData parseAndValidateLine(QString line, GTFLineValidateFlags& status) const;

    /**
    * A common method for parsing and validating an input GTF file.
    * It is used during loading the file or just getting the annotations data from it.
    */
    QMap<QString, QList<SharedAnnotationData> > parseDocument(IOAdapter* io, U2OpStatus& os);

    void load(IOAdapter* io, QList<GObject*>& objects, const U2DbiRef& dbiRef, const QVariantMap &hints, U2OpStatus& os);

    static const int FIELDS_COUNT_IN_EACH_LINE;
    static const QString NO_VALUE_STR;

    static const QString CHROMOSOME;
    static const QString SOURCE_QUALIFIER_NAME;
    static const QString SCORE_QUALIFIER_NAME;
    static const QString STRAND_QUALIFIER_NAME; // e.g. to detect if a value was in the original GTF file
                                                // (when it is rewritten)
    static const QString FRAME_QUALIFIER_NAME;
    static const QString GENE_ID_QUALIFIER_NAME;
    static const QString TRANSCRIPT_ID_QUALIFIER_NAME;
};


} // namespace

#endif
