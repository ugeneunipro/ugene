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

#include "DifferentialFormat.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

static const QString SEPARATOR("\t");
static const QString LOCUS_COLUMN("locus");
static const QString LOCUS_SEP1(":");
static const QString LOCUS_SEP2("-");
static const QString CHROMOSOME("chromosome");
static const QString UNKNOWN_CHR("unknown");

DifferentialFormat::DifferentialFormat(QObject* parent)
    : TextDocumentFormat(parent, BaseDocumentFormats::DIFF, DocumentFormatFlags_W1, {"diff"}) {
    formatName = tr("Differential");
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
    formatDescription = tr("Differential format is a text-based format for"
                           " representing Cuffdiff differential output files: expression,"
                           " splicing, promoters and cds.");
}

QList<ColumnDataParser::Column> DifferentialFormat::getColumns() {
    return {
        ColumnDataParser::Column("test_id", ColumnDataParser::STRING, "-", true /*required*/),
        ColumnDataParser::Column("gene_id", ColumnDataParser::STRING, "-"),
        ColumnDataParser::Column("gene", ColumnDataParser::STRING, "-"),
        ColumnDataParser::Column(LOCUS_COLUMN, ColumnDataParser::STRING, "-", true /*required*/),
        ColumnDataParser::Column("sample_1", ColumnDataParser::STRING, "-", true /*required*/),
        ColumnDataParser::Column("sample_2", ColumnDataParser::STRING, "-", true /*required*/),
        ColumnDataParser::Column("status", ColumnDataParser::STRING, "-", true /*required*/),
        ColumnDataParser::Column("value_1", ColumnDataParser::DOUBLE, "1"),
        ColumnDataParser::Column("value_2", ColumnDataParser::DOUBLE, "1"),
        ColumnDataParser::Column("log2(fold_change)", ColumnDataParser::DOUBLE, "0"),
        ColumnDataParser::Column("sqrt(JS)", ColumnDataParser::DOUBLE, "0"),
        ColumnDataParser::Column("test_stat", ColumnDataParser::DOUBLE, "0"),
        ColumnDataParser::Column("p_value", ColumnDataParser::DOUBLE, "1"),
        ColumnDataParser::Column("q_value", ColumnDataParser::DOUBLE, "1"),
        ColumnDataParser::Column("significant", ColumnDataParser::STRING, "-", true /*required*/)};
}

QString DifferentialFormat::getAnnotationName() {
    return "differential";
}

FormatCheckResult DifferentialFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QStringList lines = dataPrefix.split("\n", Qt::SkipEmptyParts);
    CHECK(!lines.isEmpty(), FormatDetection_NotMatched);

    ColumnDataParser parser(getColumns(), SEPARATOR);
    U2OpStatusImpl os;
    parser.init(lines.takeFirst(), os);
    CHECK_OP(os, FormatDetection_NotMatched);
    CHECK(parser.getCurrentColumns().size() > 1, FormatDetection_NotMatched);

    // Check all lines. Skip the last line because it can be cut.
    for (int i = 0; i < lines.length() - 1; i++) {
        const QString& line = lines[i];
        ColumnDataParser::Iterator values = parser.parseLine(line, os);
        CHECK_OP(os, FormatDetection_NotMatched);
        bool containsLocus = false;
        for (; !values.isEnded(); values.takeString()) {
            if (values.currentName() == LOCUS_COLUMN) {
                containsLocus = true;
            }
        }
        CHECK(containsLocus, FormatDetection_NotMatched);
    }

    return FormatDetection_Matched;
}

bool DifferentialFormat::parseLocus(const QString& locus, SharedAnnotationData& data, U2OpStatus& os) {
    // locus == chr_name:start_pos-end_pos
    QString error = tr("Can not parse locus string: %1").arg(locus);

    QStringList tokens = locus.split(LOCUS_SEP1);
    CHECK_EXT(tokens.size() == 2, os.setError(error), false);
    QString name = tokens[0];
    tokens = tokens[1].split(LOCUS_SEP2);
    CHECK_EXT(tokens.size() == 2, os.setError(error), false);

    U2Region region;
    bool ok = false;
    region.startPos = tokens[0].toLongLong(&ok);
    CHECK_EXT(ok, os.setError(error), false);
    qint64 end = tokens[1].toLongLong(&ok);
    CHECK_EXT(ok, os.setError(error), false);
    CHECK_EXT(region.startPos < end, os.setError(error), false);
    region.length = end - region.startPos + 1;

    data->qualifiers << U2Qualifier(CHROMOSOME, name);
    data->location->regions << region;
    return true;
}

QList<SharedAnnotationData> DifferentialFormat::parseAnnotations(const ColumnDataParser& parser, IOAdapterReader& reader, U2OpStatus& os) {
    QList<SharedAnnotationData> anns;
    U2OpStatus2Log logOs;
    while (!reader.atEnd()) {
        QString line = reader.readLine(os, MAX_LINE_LENGTH);
        CHECK_OP(os, {});
        if (line.isEmpty()) {
            continue;
        }

        ColumnDataParser::Iterator values = parser.parseLine(line, os);
        CHECK_OP(os, anns);
        SharedAnnotationData data(new AnnotationData());
        bool locusFound = false;
        while (values.isEnded()) {
            QString value = values.look();
            QString name = values.currentName();
            if (values.currentType() == ColumnDataParser::INTEGER) {
                values.takeInt(logOs);
            } else if (values.currentType() == ColumnDataParser::DOUBLE) {
                values.takeDouble(logOs);
            } else {
                values.takeString();
            }
            if (name == LOCUS_COLUMN) {
                locusFound = parseLocus(value, data, logOs);
            } else {
                data->qualifiers << U2Qualifier(name, value);
            }
        }
        if (!locusFound) {
            continue;
        }
        data->name = getAnnotationName();
        anns << data;
    }
    return anns;
}

Document* DifferentialFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    DbiOperationsBlock opBlock(dbiRef, os);
    CHECK_OP(os, nullptr);

    QList<SharedAnnotationData> anns = parseAnnotations(reader, os);
    CHECK_OP(os, nullptr);

    QVariantMap objectHints;
    objectHints.insert(DBI_FOLDER_HINT, hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
    auto obj = new AnnotationTableObject(getAnnotationName(), dbiRef, objectHints);
    obj->addAnnotations(anns);

    return new Document(this, reader.getFactory(), reader.getURL(), dbiRef, QList<GObject*>() << obj, hints);
}

QList<SharedAnnotationData> DifferentialFormat::parseAnnotations(IOAdapterReader& reader, U2OpStatus& os) {
    ColumnDataParser parser(getColumns(), SEPARATOR);
    QString headerLine = reader.readLine(os, MAX_LINE_LENGTH);
    CHECK_OP(os, {});
    parser.init(headerLine, os);
    CHECK_OP(os, {});

    return parseAnnotations(parser, reader, os);
}

void DifferentialFormat::writeHeader(IOAdapterWriter& writer, const QList<ColumnDataParser::Column>& columns, U2OpStatus& os) {
    QString headerLine;
    for (const ColumnDataParser::Column& column : qAsConst(columns)) {
        headerLine += (headerLine.isEmpty() ? "" : SEPARATOR) + column.name;
    }
    headerLine += "\n";
    writer.write(os, headerLine);
}

QString DifferentialFormat::createLocus(const SharedAnnotationData& data, U2OpStatus& os) {
    if (data->location->isEmpty()) {
        os.setError(tr("Annotation has not regions"));
        return "";
    }
    if (data->location->regions.size() > 1) {
        os.setError(tr("Annotation has more than one region"));
        return "";
    }
    U2Region region = data->location->regions.first();

    QVector<U2Qualifier> quals;
    data->findQualifiers(CHROMOSOME, quals);
    QString chr = UNKNOWN_CHR;
    if (!quals.isEmpty()) {
        chr = quals.first().value;
    }
    return chr + LOCUS_SEP1 + QString::number(region.startPos) + LOCUS_SEP2 + QString::number(region.endPos() - 1);
}

QString DifferentialFormat::createValue(const SharedAnnotationData& data, const ColumnDataParser::Column& column, U2OpStatus& os) {
    QVector<U2Qualifier> quals;
    data->findQualifiers(column.name, quals);
    if (!quals.isEmpty()) {
        return quals.first().value;
    } else if (column.required) {
        os.setError(tr("Required value is missed: %1").arg(column.name));
        return "";
    }
    return column.defaultValue;
}

QList<ColumnDataParser::Column> DifferentialFormat::getHeaderColumns(const QList<GObject*>& annObjs, U2OpStatus& os) {
    QList<ColumnDataParser::Column> allPossibleColumns = getColumns();
    if (annObjs.isEmpty()) {
        return allPossibleColumns;
    }

    auto annObj = dynamic_cast<AnnotationTableObject*>(annObjs.first());
    if (annObj == nullptr) {
        os.setError(tr("Annotation object not found"));
        return {};
    }

    if (!annObj->hasAnnotations()) {
        return allPossibleColumns;
    }

    QList<ColumnDataParser::Column> resultColumns;
    Annotation* ann = annObj->getAnnotations().first();
    for (const ColumnDataParser::Column& column : qAsConst(allPossibleColumns)) {
        if (column.name == LOCUS_COLUMN) {
            resultColumns << column;
            continue;
        }
        QList<U2Qualifier> quals;
        ann->findQualifiers(column.name, quals);
        if (!quals.isEmpty()) {
            resultColumns << column;
        }
    }
    for (const ColumnDataParser::Column& column : qAsConst(allPossibleColumns)) {
        if (column.required && !resultColumns.contains(column)) {
            os.setError(tr("Required column is missed: %1").arg(column.name));
            return resultColumns;
        }
    }
    return resultColumns;
}

void DifferentialFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    QList<GObject*> annotationObjects = document->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    QList<ColumnDataParser::Column> columns = getHeaderColumns(annotationObjects, os);
    CHECK_OP(os, );
    writeHeader(writer, columns, os);
    CHECK_OP(os, );
    for (const GObject* obj : qAsConst(annotationObjects)) {
        auto annObj = dynamic_cast<const AnnotationTableObject*>(obj);
        SAFE_POINT(annObj != nullptr, "NULL annotation object", );
        QList<Annotation*> annotations = annObj->getAnnotations();
        for (const Annotation* ann : qAsConst(annotations)) {
            QString line;
            U2OpStatus2Log logOs;
            for (const ColumnDataParser::Column& column : qAsConst(columns)) {
                line += line.isEmpty() ? "" : SEPARATOR;
                if (column.name == LOCUS_COLUMN) {
                    line += createLocus(ann->getData(), logOs);
                } else {
                    line += createValue(ann->getData(), column, logOs);
                }
            }
            if (logOs.hasError()) {
                continue;
            }
            line += "\n";
            writer.write(os, line);
            CHECK_OP(os, );
        }
    }
}

}  // namespace U2
