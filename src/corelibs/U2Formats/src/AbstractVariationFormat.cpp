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

#include "AbstractVariationFormat.h"

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>
#include <U2Core/U2Variant.h>
#include <U2Core/U2VariantDbi.h>
#include <U2Core/VariantTrackObject.h>

namespace U2 {

const QString AbstractVariationFormat::META_INFO_START = "##";
const QString AbstractVariationFormat::HEADER_START = "#";
const QString AbstractVariationFormat::COLUMNS_SEPARATOR = "\t";

static QList<U2Variant> splitVariants(const U2Variant& variant, const QList<QString>& altAlleles) {
    QList<U2Variant> variantList;
    for (const QString& allele : qAsConst(altAlleles)) {
        U2Variant clonedVariant = variant;
        clonedVariant.obsData = allele.toLatin1();
        variantList.append(clonedVariant);
    }
    return variantList;
}

AbstractVariationFormat::AbstractVariationFormat(QObject* p, const DocumentFormatId& id, const QStringList& fileExtensions, bool _isSupportHeader)
    : TextDocumentFormat(p, id, DocumentFormatFlags_SW, fileExtensions),
      isSupportHeader(_isSupportHeader) {
    supportedObjectTypes += GObjectTypes::VARIANT_TRACK;
    formatDescription = tr("SNP formats are used to store single-nucleotide polymorphism data");
    indexing = AbstractVariationFormat::ZeroBased;
}

static void addStringAttribute(U2OpStatus& os, U2Dbi* dbi, const U2VariantTrack& variantTrack, const QString& name, const QString& value) {
    CHECK(!value.isEmpty(), );
    U2StringAttribute attribute;
    U2AttributeUtils::init(attribute, variantTrack, name);
    attribute.value = value;
    dbi->getAttributeDbi()->createStringAttribute(attribute, os);
}

static constexpr const char* CHR_PREFIX = "chr";

static constexpr int MAX_LINE_LENGTH = 10 * 1024;  // 10 Kb

Document* AbstractVariationFormat::loadTextDocument(IOAdapterReader& reader, const U2DbiRef& dbiRef, const QVariantMap& hints, U2OpStatus& os) {
    DbiConnection con(dbiRef, os);
    SAFE_POINT_OP(os, nullptr);

    U2Dbi* dbi = con.dbi;
    SAFE_POINT(dbi->getVariantDbi(), "Variant DBI is NULL!", nullptr);

    SplitAlleles splitting = hints.contains(DocumentReadingMode_SplitVariationAlleles)
                                 ? AbstractVariationFormat::Split
                                 : AbstractVariationFormat::NoSplit;

    // TODO: load snps with chunks of fixed size to avoid memory consumption.
    QMap<QString, QList<U2Variant>> snpsMap;

    QString metaInfo;
    QStringList header;

    int lineNumber = 0;
    do {
        os.setProgress(reader.getProgress());
        QString line = reader.readLine(os, MAX_LINE_LENGTH);
        CHECK_OP(os, nullptr);

        lineNumber++;
        if (line.isEmpty()) {
            continue;
        }

        if (line.startsWith(META_INFO_START)) {
            metaInfo += line + "\n";
            continue;
        }

        if (line.startsWith(HEADER_START)) {
            header = line.split(COLUMNS_SEPARATOR);
            continue;
        }

        QStringList columns = line.split(COLUMNS_SEPARATOR);

        if (columns.size() < maxColumnIndex) {
            os.addWarning(tr("Line %1: There are too few columns in this line. The line was skipped.").arg(lineNumber));
            continue;
        }

        QList<QString> altAllele;

        U2Variant v;
        QString seqName;

        for (int columnNumber = 0; columnNumber < columns.size(); columnNumber++) {
            ColumnRole columnRole = columnRoles.value(columnNumber, ColumnRole_Unknown);
            const QString& columnData = columns[columnNumber];
            switch (columnRole) {
                case ColumnRole_ChromosomeId:
                    seqName = columnData;
                    break;
                case ColumnRole_StartPos:
                    v.startPos = columnData.toInt();
                    if (indexing == AbstractVariationFormat::OneBased) {
                        v.startPos -= 1;
                    }
                    break;
                case ColumnRole_EndPos:
                    v.endPos = columnData.toInt();
                    if (indexing == AbstractVariationFormat::OneBased) {
                        v.endPos -= 1;
                    }
                    break;
                case ColumnRole_RefData:
                    v.refData = columnData.toLatin1();
                    break;
                case ColumnRole_ObsData:
                    if (splitting == AbstractVariationFormat::Split) {
                        altAllele = columnData.trimmed().split(',');
                    } else {
                        v.obsData = columnData.toLatin1();
                    }
                    break;
                case ColumnRole_PublicId:
                    v.publicId = columnData.toLatin1();
                    break;
                case ColumnRole_Info:
                    v.additionalInfo.insert(U2Variant::VCF4_INFO, columnData);
                    break;
                case ColumnRole_Unknown:
                    v.additionalInfo.insert(columnNumber < header.size() ? header[columnNumber] : QString::number(columnNumber), columnData);
                    break;
                default:
                    coreLog.trace(QString("Warning: unknown column role %1 (line %2, column %3)").arg(columnRole).arg(line).arg(columnNumber));
                    break;
            }
        }

        if (!columnRoles.values().contains(ColumnRole_EndPos)) {
            v.endPos = v.startPos + v.refData.size() - 1;
        }

        if (v.publicId.isEmpty()) {
            QString prefix = seqName.contains(CHR_PREFIX) ? seqName : seqName.prepend(CHR_PREFIX);
            v.publicId = QString("%1v%2").arg(prefix).arg(snpsMap[seqName].count() + 1).toLatin1();
        }

        if (splitting == AbstractVariationFormat::Split) {
            const QList<U2Variant>& alleleVariants = splitVariants(v, altAllele);
            if (altAllele.isEmpty()) {
                continue;
            }
            snpsMap[seqName].append(alleleVariants);
        } else {
            snpsMap[seqName].append(v);
        }

    } while (!reader.atEnd());
    CHECK(!os.hasError(), nullptr);

    GAutoDeleteList<GObject> objects;
    QSet<QString> names;
    QString folder = hints.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    // Create an empty track.
    if (snpsMap.isEmpty()) {
        snpsMap.insert("unknown", {});
    }

    QList<QString> seqNames = snpsMap.keys();
    for (const QString& seqName : qAsConst(seqNames)) {
        U2VariantTrack track;
        track.visualName = "Variant track";
        track.sequenceName = seqName;
        dbi->getVariantDbi()->createVariantTrack(track, TrackType_All, folder, os);
        CHECK_OP(os, nullptr);

        addStringAttribute(os, dbi, track, U2VariantTrack::META_INFO_ATTRIBUTE, metaInfo);
        CHECK_OP(os, nullptr);

        addStringAttribute(os, dbi, track, U2VariantTrack::HEADER_ATTRIBUTE, StrPackUtils::packStringList(header));
        CHECK_OP(os, nullptr);

        const QList<U2Variant>& variantList = snpsMap.value(seqName);
        if (!variantList.isEmpty()) {
            BufferedDbiIterator<U2Variant> bufIter(variantList);
            dbi->getVariantDbi()->addVariantsToTrack(track, &bufIter, os);
            CHECK_OP(os, nullptr);
        }

        U2EntityRef trackRef(dbiRef, track.id);
        QString objName = TextUtils::variate(track.sequenceName, "_", names);
        names.insert(objName);
        objects.qlist << new VariantTrackObject(objName, trackRef);
    }

    QString lockReason;
    auto doc = new Document(this, reader.getFactory(), reader.getURL(), dbiRef, objects.qlist, hints, lockReason);
    objects.qlist.clear();
    return doc;
}

FormatCheckResult AbstractVariationFormat::checkRawTextData(const QString& dataPrefix, const GUrl&) const {
    QStringList lines = dataPrefix.split("\n");
    int idx = 0;
    int mismatchesNumber = 0;
    int cellsNumber = 0;
    QRegExp wordExp("\\D+");
    for (const QString& originalLine : qAsConst(lines)) {
        bool skipLastLine = lines.size() != 1 && idx == lines.size() - 1;
        if (skipLastLine) {
            continue;
        }

        QString line = originalLine.trimmed();
        idx++;
        if (line.startsWith(META_INFO_START)) {
            bool isFormatMatched = line.contains("format=" + formatName);
            if (isFormatMatched) {
                return FormatDetection_Matched;
            }
            continue;
        }

        QStringList cols = line.split(COLUMNS_SEPARATOR, QString::SkipEmptyParts);
        if (!this->checkFormatByColumnCount(cols.size())) {
            return FormatDetection_NotMatched;
        }

        for (int columnNumber = 0; columnNumber < cols.size(); columnNumber++) {
            cellsNumber++;
            ColumnRole role = columnRoles.value(columnNumber, ColumnRole_Unknown);
            QString col = cols.at(columnNumber);
            bool isCorrect = !col.isEmpty();
            if (!isCorrect) {
                mismatchesNumber++;
                continue;
            }
            switch (role) {
                case ColumnRole_StartPos:
                    col.toInt(&isCorrect);
                    break;
                case ColumnRole_EndPos:
                    col.toInt(&isCorrect);
                    break;
                case ColumnRole_RefData:
                    isCorrect = wordExp.exactMatch(col);
                    break;
                case ColumnRole_ObsData:
                    isCorrect = wordExp.exactMatch(col);
                    break;
                default:
                    break;
            }
            if (!isCorrect) {
                mismatchesNumber++;
            }
        }
    }
    if (idx == 0) {
        return FormatDetection_NotMatched;
    }
    if (cellsNumber > 0 && 0 == mismatchesNumber) {
        return FormatDetection_Matched;
    }
    return FormatDetection_AverageSimilarity;
}

void AbstractVariationFormat::storeTextDocument(IOAdapterWriter& writer, Document* document, U2OpStatus& os) {
    QList<GObject*> variantTrackObjects = document->findGObjectByType(GObjectTypes::VARIANT_TRACK);
    if (!variantTrackObjects.isEmpty()) {
        auto variantTrackObject = qobject_cast<VariantTrackObject*>(variantTrackObjects.first());
        SAFE_POINT_EXT(variantTrackObject != nullptr, os.setError(L10N::internalError("Not a VariantTrackObject")), );
        storeHeader(variantTrackObject, writer, os);
    }

    for (GObject* obj : qAsConst(variantTrackObjects)) {
        auto trackObject = qobject_cast<VariantTrackObject*>(obj);
        SAFE_POINT_EXT(trackObject != nullptr, os.setError(L10N::internalError("not a VariantTrackObject")), );
        storeTrack(writer, trackObject, os);
    }
}

void AbstractVariationFormat::storeTextEntry(IOAdapterWriter& writer, const QMap<GObjectType, QList<GObject*>>& objectsMap, U2OpStatus& os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::VARIANT_TRACK), "Variation entry storing: no variations", );

    const QList<GObject*>& vars = objectsMap[GObjectTypes::VARIANT_TRACK];
    SAFE_POINT(vars.size() == 1, "Variation entry storing: variation objects count error", );

    auto trackObject = dynamic_cast<VariantTrackObject*>(vars.first());
    SAFE_POINT(trackObject != nullptr, "Variation entry storing: NULL variation object", );

    storeTrack(writer, trackObject, os);
}

void AbstractVariationFormat::storeTrack(IOAdapterWriter& writer, const VariantTrackObject* trackObj, U2OpStatus& os) const {
    CHECK(trackObj != nullptr, );

    U2VariantTrack track = trackObj->getVariantTrack(os);
    CHECK_OP(os, );

    QScopedPointer<U2DbiIterator<U2Variant>> varsIter(trackObj->getVariants(U2_REGION_MAX, os));
    CHECK_OP(os, );

    QStringList header = getHeader(trackObj, os);
    CHECK_OP(os, );

    QByteArray snpString;
    while (varsIter->hasNext()) {
        U2Variant variant = varsIter->next();

        snpString.clear();
        for (int columnIndex = 0; columnIndex <= maxColumnIndex; columnIndex++) {
            if (columnIndex != 0) {
                snpString += COLUMNS_SEPARATOR;
            }

            ColumnRole role = columnRoles.value(columnIndex, ColumnRole_Unknown);
            switch (role) {
                case ColumnRole_ChromosomeId:
                    snpString += track.sequenceName;
                    break;
                case ColumnRole_StartPos:
                    switch (indexing) {
                        case AbstractVariationFormat::OneBased:
                            snpString += QByteArray::number(variant.startPos + 1);
                            break;
                        case AbstractVariationFormat::ZeroBased:
                            snpString += QByteArray::number(variant.startPos);
                            break;
                        default:
                            assert(0);
                    }
                    break;
                case ColumnRole_EndPos:
                    switch (indexing) {
                        case AbstractVariationFormat::OneBased:
                            snpString += QByteArray::number(variant.endPos + 1);
                            break;
                        case AbstractVariationFormat::ZeroBased:
                            snpString += QByteArray::number(variant.endPos);
                            break;
                        default:
                            assert(0);
                    }
                    break;
                case ColumnRole_RefData:
                    snpString += variant.refData;
                    break;
                case ColumnRole_ObsData:
                    snpString += variant.obsData;
                    break;
                case ColumnRole_PublicId:
                    snpString += variant.publicId;
                    break;
                case ColumnRole_Info:
                    snpString += variant.additionalInfo.value(U2Variant::VCF4_INFO, ".");
                    break;
                case ColumnRole_Unknown: {
                    const QString columnTitle = columnIndex < header.size() ? header[columnIndex] : QString::number(columnIndex);
                    snpString += variant.additionalInfo.value(columnTitle, ".");
                    break;
                }
                default:
                    coreLog.trace(QString("Warning: unknown column role: %1").arg(role));
                    break;
            }
        }

        if (!useOnlyBaseColumns) {
            for (int i = maxColumnIndex + 1; i < header.size(); i++) {
                snpString += COLUMNS_SEPARATOR + variant.additionalInfo.value(header[i], ".").toLatin1();
            }

            for (int i = qMax(maxColumnIndex + 1, header.size()); i <= maxColumnIndex + variant.additionalInfo.size(); i++) {
                if (!variant.additionalInfo.contains(QString::number(i))) {
                    break;
                }
                snpString += COLUMNS_SEPARATOR + variant.additionalInfo[QString::number(i)].toLatin1();
            }
        }

        snpString += "\n";
        writer.write(os, snpString);
    }
}

void AbstractVariationFormat::storeHeader(const VariantTrackObject* trackObject, IOAdapterWriter& writer, U2OpStatus& os) const {
    CHECK(isSupportHeader, );
    SAFE_POINT_EXT(trackObject != nullptr, os.setError(L10N::internalError("storeHeader got null object")), );

    QString metaInfo = getMetaInfo(trackObject, os);
    CHECK_OP(os, );
    if (!metaInfo.isEmpty()) {
        writer.write(os, metaInfo);
        CHECK_OP(os, );
    }

    QStringList header = getHeader(trackObject, os);
    CHECK_OP(os, );
    if (!header.isEmpty()) {
        writer.write(os, header.join(COLUMNS_SEPARATOR) + "\n");
        CHECK_OP(os, );
    }
}

QString AbstractVariationFormat::getMetaInfo(const VariantTrackObject* trackObject, U2OpStatus& os) {
    U2StringAttribute attribute = U2AttributeUtils::findStringAttribute(trackObject, U2VariantTrack::META_INFO_ATTRIBUTE, os);
    CHECK_OP(os, "");
    return attribute.value;
}

QStringList AbstractVariationFormat::getHeader(const VariantTrackObject* trackObject, U2OpStatus& os) {
    U2StringAttribute attribute = U2AttributeUtils::findStringAttribute(trackObject, U2VariantTrack::HEADER_ATTRIBUTE, os);
    CHECK_OP(os, {});
    return StrPackUtils::unpackStringList(attribute.value);
}

}  // namespace U2
