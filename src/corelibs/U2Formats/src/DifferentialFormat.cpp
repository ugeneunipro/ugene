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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DifferentialFormat.h"

namespace U2 {

static const QString SEPARATOR("\t");
static const QString LOCUS_COLUMN("locus");
static const QString LOCUS_SEP1(":");
static const QString LOCUS_SEP2("-");
static const QString CHROMOSOME("chromosome");
static const QString UNKNOWN_CHR("unknown");

DifferentialFormat::DifferentialFormat(QObject *parent)
: TextDocumentFormat(parent, BaseDocumentFormats::DIFF, DocumentFormatFlags_W1, QStringList()<<"diff")
{
    formatName = tr("Differential");
    supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
    formatDescription = tr("Differential format is a text-based format for"
        " representing Cuffdiff differential output files: expression,"
        " splicing, promoters and cds.");
}

QList<ColumnDataParser::Column> DifferentialFormat::getColumns() const {
    return QList<ColumnDataParser::Column>()
        << ColumnDataParser::Column("test_id", ColumnDataParser::STRING, "-", true /*required*/)
        << ColumnDataParser::Column("gene_id", ColumnDataParser::STRING, "-")
        << ColumnDataParser::Column("gene", ColumnDataParser::STRING, "-")
        << ColumnDataParser::Column(LOCUS_COLUMN, ColumnDataParser::STRING, "-", true /*required*/)
        << ColumnDataParser::Column("sample_1", ColumnDataParser::STRING, "-", true /*required*/)
        << ColumnDataParser::Column("sample_2", ColumnDataParser::STRING, "-", true /*required*/)
        << ColumnDataParser::Column("status", ColumnDataParser::STRING, "-", true /*required*/)
        << ColumnDataParser::Column("value_1", ColumnDataParser::DOUBLE, "1")
        << ColumnDataParser::Column("value_2", ColumnDataParser::DOUBLE, "1")
        << ColumnDataParser::Column("log2(fold_change)", ColumnDataParser::DOUBLE, "0")
        << ColumnDataParser::Column("sqrt(JS)", ColumnDataParser::DOUBLE, "0")
        << ColumnDataParser::Column("test_stat", ColumnDataParser::DOUBLE, "0")
        << ColumnDataParser::Column("p_value", ColumnDataParser::DOUBLE, "1")
        << ColumnDataParser::Column("q_value", ColumnDataParser::DOUBLE, "1")
        << ColumnDataParser::Column("significant", ColumnDataParser::STRING, "-", true /*required*/);
}

QString DifferentialFormat::getAnnotationName() const {
    return "differential";
}

FormatCheckResult DifferentialFormat::checkRawTextData(const QByteArray &rawData,
    const GUrl &) const {
    QStringList lines = QString(rawData).split("\n", QString::SkipEmptyParts);
    CHECK(lines.size() > 0, FormatDetection_NotMatched);

    ColumnDataParser parser(getColumns(), SEPARATOR);
    U2OpStatusImpl os;
    parser.init(lines.takeFirst(), os);
    CHECK_OP(os, FormatDetection_NotMatched);
    CHECK(parser.getCurrentColumns().size() > 1, FormatDetection_NotMatched);

    for (QStringList::iterator i=lines.begin(); i!=lines.end();) {
        // skip the last line because it can be cut
        QStringList::iterator current(i);
        ++i;
        if (i == lines.end()) {
            break;
        }
        // parse line
        ColumnDataParser::Iterator values = parser.parseLine(*current, os);
        CHECK_OP(os, FormatDetection_NotMatched);
        bool containsLocus = false;
        for (; !values.isEnded(); values.takeString()) {
            if (LOCUS_COLUMN == values.currentName()) {
                containsLocus = true;
            }
        }
        CHECK(containsLocus, FormatDetection_NotMatched);
    }

    return FormatDetection_Matched;
}

const int DifferentialFormat::BUFFER_SIZE = 4194304;
QString DifferentialFormat::readLine(IOAdapter *io, QByteArray &buffer, U2OpStatus &os) {
    bool eol = false;
    qint64 size = io->readLine(buffer.data(), BUFFER_SIZE, &eol);
    CHECK_EXT(eol || io->isEof(), os.setError("Line is too long"), QString());
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), QString());

    return buffer.left(size).trimmed();
}

bool DifferentialFormat::parseLocus(const QString &locus, SharedAnnotationData &data, U2OpStatus &os) {
    // locus == chr_name:start_pos-end_pos
    QString name;
    U2Region region;
    {
        QString error = QString("Can not parse locus string: %1").arg(locus);
        QStringList tokens = locus.split(LOCUS_SEP1);
        CHECK_EXT(2 == tokens.size(), os.setError(error), false);
        name = tokens[0];
        tokens = tokens[1].split(LOCUS_SEP2);
        CHECK_EXT(2 == tokens.size(), os.setError(error), false);
        bool ok = false;
        region.startPos = tokens[0].toLongLong(&ok);
        CHECK_EXT(ok, os.setError(error), false);
        qint64 end = tokens[1].toLongLong(&ok);
        CHECK_EXT(ok, os.setError(error), false);
        CHECK_EXT(region.startPos < end, os.setError(error), false);
        region.length = end - region.startPos + 1;
    }
    data->qualifiers << U2Qualifier(CHROMOSOME, name);
    data->location->regions << region;
    return true;
}

QList<SharedAnnotationData> DifferentialFormat::parseAnnotations(const ColumnDataParser &parser, IOAdapter *io, QByteArray &buffer, U2OpStatus &os) {
    QList<SharedAnnotationData> anns;
    U2OpStatus2Log logOs;
    while (!io->isEof()) {
        QString line = readLine(io, buffer, os);
        if (line.isEmpty()) {
            continue;
        }

        ColumnDataParser::Iterator values = parser.parseLine(line, os);
        CHECK_OP(os, anns);
        SharedAnnotationData data(new AnnotationData);
        bool locusFound = false;
        while (values.isEnded()) {
            QString value = values.look();
            QString name = values.currentName();
            if (ColumnDataParser::INTEGER == values.currentType()) {
                values.takeInt(logOs);
            } else if (ColumnDataParser::DOUBLE == values.currentType()) {
                values.takeDouble(logOs);
            } else {
                values.takeString();
            }
            if (LOCUS_COLUMN == name) {
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

Document * DifferentialFormat::loadTextDocument(IOAdapter *io, const U2DbiRef &targetDb, const QVariantMap &hints, U2OpStatus &os) {
    DbiOperationsBlock opBlock(targetDb, os);
    CHECK_OP(os, NULL);
    Q_UNUSED(opBlock);

    QList<SharedAnnotationData> anns = parseAnnotations(io, os);
    CHECK_OP(os, NULL);

    QVariantMap objectHints;
    objectHints.insert(DBI_FOLDER_HINT, hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER));
    AnnotationTableObject *obj = new AnnotationTableObject(getAnnotationName(), targetDb, objectHints);
    obj->addAnnotations(anns);

    return new Document(this, io->getFactory(), io->getURL(), targetDb, QList<GObject *>() << obj, hints, "");
}

QList<SharedAnnotationData> DifferentialFormat::parseAnnotations(IOAdapter *io, U2OpStatus &os) {
    ColumnDataParser parser(getColumns(), SEPARATOR);
    QByteArray buffer(BUFFER_SIZE + 1, 0);

    QString headerLine = readLine(io, buffer, os);
    CHECK_OP(os, QList<SharedAnnotationData>());
    parser.init(headerLine, os);
    CHECK_OP(os, QList<SharedAnnotationData>());

    return parseAnnotations(parser, io, buffer, os);
}

void DifferentialFormat::writeHeader(IOAdapter *io, const QList<ColumnDataParser::Column> &columns) {
    bool first = true;
    QString headerLine;
    foreach (const ColumnDataParser::Column c, columns) {
        headerLine += (first ? "": SEPARATOR) + c.name;
        first = false;
    }
    headerLine += "\n";
    io->writeBlock(headerLine.toLatin1());
}

QString DifferentialFormat::createLocus(const SharedAnnotationData &data, U2OpStatus &os) {
    if (data->location->isEmpty()) {
        os.setError("Annotation has not regions");
        return "";
    }
    if (data->location->regions.size() > 1) {
        os.setError("Annotation has more than one region");
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

QString DifferentialFormat::createValue(const SharedAnnotationData &data, const ColumnDataParser::Column &column, U2OpStatus &os) {
    QVector<U2Qualifier> quals;
    data->findQualifiers(column.name, quals);
    if (!quals.isEmpty()) {
        return quals.first().value;
    } else if (column.required) {
        os.setError(QString("Required value is missed: %1").arg(column.name));
        return "";
    }
    return column.defaultValue;
}

QList<ColumnDataParser::Column> DifferentialFormat::getHeaderColumns(const QList<GObject *> &annObjs, U2OpStatus &os) {
    QList<ColumnDataParser::Column> result;
    if (annObjs.isEmpty()) {
        return getColumns();
    }

    AnnotationTableObject *annObj = dynamic_cast<AnnotationTableObject *>(annObjs.first());
    if (NULL == annObj) {
        os.setError("NULL annotation object");
        return result;
    }

    if (!annObj->hasAnnotations()) {
        return getColumns();
    }

    Annotation *ann = annObj->getAnnotations().first();
    foreach (const ColumnDataParser::Column &c, getColumns()) {
        if (LOCUS_COLUMN == c.name) {
            result << c;
            continue;
        }
        QList<U2Qualifier> quals;
        ann->findQualifiers(c.name, quals);
        if (!quals.isEmpty()) {
            result << c;
        }
    }
    foreach (const ColumnDataParser::Column &c, getColumns()) {
        if (c.required && !result.contains(c)) {
            os.setError(tr("Required column is missed: %1").arg(c.name));
            return result;
        }
    }
    return result;
}

void DifferentialFormat::storeDocument(Document *d, IOAdapter *io, U2OpStatus &os) {
    QList<GObject*> anns = d->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    QList<ColumnDataParser::Column> columns = getHeaderColumns(anns, os);
    CHECK_OP(os,);
    writeHeader(io, columns);
    foreach (GObject *obj, anns) {
        AnnotationTableObject *annObj = dynamic_cast<AnnotationTableObject *>(obj);
        SAFE_POINT(NULL != annObj, "NULL annotation object",);
        foreach (Annotation *ann, annObj->getAnnotations()) {
            bool first = true;
            QString line;
            U2OpStatus2Log logOs;
            foreach (const ColumnDataParser::Column c, columns) {
                line += (first ? "": SEPARATOR);
                first = false;
                if (LOCUS_COLUMN == c.name) {
                    line += createLocus(ann->getData(), logOs);
                } else {
                    line += createValue(ann->getData(), c, logOs);
                }
            }
            if (logOs.hasError()) {
                continue;
            }
            line += "\n";
            io->writeBlock(line.toLatin1());
        }
    }
}

} // U2
