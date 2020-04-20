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

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
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

#include "AbstractVariationFormat.h"

namespace U2 {

const QString AbstractVariationFormat::META_INFO_START = "##";
const QString AbstractVariationFormat::HEADER_START = "#";
const QString AbstractVariationFormat::COLUMNS_SEPARATOR = "\t";

QList<U2Variant> splitVariants(const U2Variant& v, const QList<QString>& altAllel){
    QList<U2Variant> res;

    foreach(const QString& alt, altAllel){
        U2Variant var = v;

        var.obsData = alt.toLatin1();

        res.append(var);
    }

    return res;
}


AbstractVariationFormat::AbstractVariationFormat(QObject *p, const DocumentFormatId& id, const QStringList &fileExts, bool _isSupportHeader)
    : TextDocumentFormat(p, id, DocumentFormatFlags_SW, fileExts),
      isSupportHeader(_isSupportHeader),
      maxColumnNumber(0)
{
    supportedObjectTypes += GObjectTypes::VARIANT_TRACK;
    formatDescription = tr("SNP formats are used to store single-nucleotide polymorphism data");
    indexing = AbstractVariationFormat::ZeroBased;
}

namespace {
const int LOCAL_READ_BUFF_SIZE = 10 * 1024; // 10 Kb

inline QByteArray readLine(IOAdapter *io, char *buffer, int bufferSize, U2OpStatus& os) {
    QByteArray result;
    bool terminatorFound = false;
    do {
        qint64 length = io->readLine(buffer, bufferSize, &terminatorFound);
        CHECK_EXT(!io->hasError(), os.setError(io->errorString()), QByteArray());
        CHECK(-1 != length, result);

        result += QByteArray(buffer, length);
    } while (!terminatorFound && !io->isEof());
    return result;
}

void addStringAttribute(U2OpStatus &os, U2Dbi *dbi, const U2VariantTrack &variantTrack, const QString &name, const QString &value) {
    CHECK(!value.isEmpty(), );
    U2StringAttribute attribute;
    U2AttributeUtils::init(attribute, variantTrack, name);
    attribute.value = value;
    dbi->getAttributeDbi()->createStringAttribute(attribute, os);
}

}

#define CHR_PREFIX "chr"

Document *AbstractVariationFormat::loadTextDocument(IOAdapter *io, const U2DbiRef &dbiRef, const QVariantMap &fs, U2OpStatus &os) {
    DbiConnection con(dbiRef, os);
    SAFE_POINT_OP(os, NULL);
    U2Dbi *dbi = con.dbi;

    SAFE_POINT(dbi->getVariantDbi() , "Variant DBI is NULL!", NULL);
    SAFE_POINT(io, "IO adapter is NULL!",  NULL);
    SAFE_POINT(io->isOpen(), QString("IO adapter is not open %1").arg(io->getURL().getURLString()), NULL);

    QByteArray readBuff(LOCAL_READ_BUFF_SIZE + 1, 0);
    char* buff = readBuff.data();

    SplitAlleles splitting = fs.contains(DocumentReadingMode_SplitVariationAlleles)? AbstractVariationFormat::Split : AbstractVariationFormat::NoSplit;

    //TODO: load snps with chunks of fixed size to avoid memory consumption
    QMap<QString, QList<U2Variant> > snpsMap;

    QString metaInfo;
    QStringList header;

    int lineNumber = 0;
    do {
        os.setProgress(io->getProgress());
        QString line = readLine(io, buff, LOCAL_READ_BUFF_SIZE, os);
        CHECK_OP(os, NULL);

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

        if (columns.size() < maxColumnNumber) {
            os.addWarning(tr("Line %1: There are too few columns in this line. The line was skipped.").arg(lineNumber));
            continue;
        }

        QList<QString> altAllele;

        U2Variant v;
        QString seqName;

        for (int columnNumber = 0; columnNumber < columns.size(); columnNumber++) {
            const ColumnRole columnRole = columnRoles.value(columnNumber, ColumnRole_Unknown);
            const QString &columnData = columns[columnNumber];
            switch (columnRole) {
            case ColumnRole_ChromosomeId:
                seqName = columnData;
                break;
            case ColumnRole_StartPos:
                v.startPos = columnData.toInt();
                if (indexing == AbstractVariationFormat::OneBased){
                    v.startPos -= 1;
                }
                break;
            case ColumnRole_EndPos:
                v.endPos = columnData.toInt();
                if (indexing == AbstractVariationFormat::OneBased){
                    v.endPos -= 1;
                }
                break;
            case ColumnRole_RefData:
                v.refData = columnData.toLatin1();
                break;
            case ColumnRole_ObsData:
                if (splitting == AbstractVariationFormat::Split){
                    altAllele = columnData.trimmed().split(',');
                }else{
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
                assert(0);
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

        if (splitting == AbstractVariationFormat::Split){
            const QList<U2Variant>& allelVariants = splitVariants(v, altAllele);
            if (altAllele.isEmpty()){
                continue;
            }
            snpsMap[seqName].append(allelVariants);
        }else{
            snpsMap[seqName].append(v);
        }



    } while (!io->isEof());
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), NULL);

    GAutoDeleteList<GObject> objects;
    QSet<QString> names;
    const QString folder = fs.value(DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    //create empty track
    if (snpsMap.isEmpty()){
        U2VariantTrack track;
        track.sequenceName = "unknown";
        dbi->getVariantDbi()->createVariantTrack(track, TrackType_All, folder, os);
        CHECK_OP(os, NULL);

        addStringAttribute(os, dbi, track, U2VariantTrack::META_INFO_ATTIBUTE, metaInfo);
        CHECK_OP(os, NULL);
        addStringAttribute(os, dbi, track, U2VariantTrack::HEADER_ATTIBUTE, StrPackUtils::packStringList(header));
        CHECK_OP(os, NULL);

        U2EntityRef trackRef(dbiRef, track.id);
        QString objName = TextUtils::variate(track.sequenceName, "_", names);
        names.insert(objName);
        VariantTrackObject *trackObj = new VariantTrackObject(objName, trackRef);
        objects.qlist << trackObj;
    }

    foreach (const QString &seqName, snpsMap.keys().toSet()) {
        U2VariantTrack track;
        track.visualName = "Variant track";
        track.sequenceName = seqName;
        dbi->getVariantDbi()->createVariantTrack(track, TrackType_All, folder, os);
        CHECK_OP(os, NULL);

        addStringAttribute(os, dbi, track, U2VariantTrack::META_INFO_ATTIBUTE, metaInfo);
        CHECK_OP(os, NULL);
        addStringAttribute(os, dbi, track, U2VariantTrack::HEADER_ATTIBUTE, StrPackUtils::packStringList(header));
        CHECK_OP(os, NULL);

        const QList<U2Variant>& vars = snpsMap.value(seqName);
        BufferedDbiIterator<U2Variant> bufIter(vars);
        dbi->getVariantDbi()->addVariantsToTrack(track, &bufIter, os);
        CHECK_OP(os, NULL);

        U2EntityRef trackRef(dbiRef, track.id);
        QString objName = TextUtils::variate(track.sequenceName, "_", names);
        names.insert(objName);
        VariantTrackObject *trackObj = new VariantTrackObject(objName, trackRef);
        objects.qlist << trackObj;
    }

    QString lockReason;
    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, objects.qlist, fs, lockReason);
    objects.qlist.clear();
    return doc;
}

FormatCheckResult AbstractVariationFormat::checkRawTextData(const QByteArray &dataPrefix, const GUrl &) const {
    QStringList lines = QString(dataPrefix).split("\n");
    int idx = 0;
    int mismatchesNumber = 0;
    int cellsNumber = 0;
    foreach (const QString &l, lines) {
        bool skipLastLine = (1 != lines.size()) && (idx == lines.size()-1);
        if (skipLastLine) {
            continue;
        }

        QString line = l.trimmed();
        idx++;
        if (line.startsWith(META_INFO_START)) {
            bool isFormatMatched = line.contains("format=" + formatName);
            if(isFormatMatched) {
                return FormatDetection_Matched;
            }
            continue;
        }

        QStringList cols = line.split(COLUMNS_SEPARATOR, QString::SkipEmptyParts);
        if (!this->checkFormatByColumnCount(cols.size())) {
            return FormatDetection_NotMatched;
        }

        for(int columnNumber = 0; columnNumber < cols.size(); columnNumber++) {
            cellsNumber++;
            ColumnRole role = columnRoles.value(columnNumber, ColumnRole_Unknown);
            QString col = cols.at(columnNumber);
            bool isCorrect = !col.isEmpty();
            if(!isCorrect) {
                mismatchesNumber++;
                continue;
            }
            QRegExp wordExp("\\D+");
            switch(role) {
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
            if(!isCorrect) {
                mismatchesNumber++;
            }
        }

    }
    if (0 == idx) {
        return FormatDetection_NotMatched;
    }
    if(cellsNumber > 0 && 0 == mismatchesNumber) {
        return FormatDetection_Matched;
    }
    return FormatDetection_AverageSimilarity;
}

void AbstractVariationFormat::storeDocument(Document *doc, IOAdapter *io, U2OpStatus &os) {
    const QList<GObject *> variantTrackObjects = doc->findGObjectByType(GObjectTypes::VARIANT_TRACK);
    if(!variantTrackObjects.isEmpty()) {
        storeHeader(variantTrackObjects.first(), io, os);
    }

    foreach (GObject *obj, variantTrackObjects) {
        VariantTrackObject *trackObj = qobject_cast<VariantTrackObject *>(obj);
        SAFE_POINT_EXT(NULL != trackObj, os.setError("Can't cast GObject to VariantTrackObject"), );
        storeTrack(io, trackObj, os);
    }
}

void AbstractVariationFormat::storeEntry(IOAdapter *io, const QMap< GObjectType, QList<GObject*> > &objectsMap, U2OpStatus &os) {
    SAFE_POINT(objectsMap.contains(GObjectTypes::VARIANT_TRACK), "Variation entry storing: no variations", );
    const QList<GObject*> &vars = objectsMap[GObjectTypes::VARIANT_TRACK];
    SAFE_POINT(1 == vars.size(), "Variation entry storing: variation objects count error", );

    VariantTrackObject *trackObj = dynamic_cast<VariantTrackObject*>(vars.first());
    SAFE_POINT(NULL != trackObj, "Variation entry storing: NULL variation object", );

    storeTrack(io, trackObj, os);
}

void AbstractVariationFormat::storeTrack(IOAdapter *io, const VariantTrackObject *trackObj, U2OpStatus &os) {
    CHECK(NULL != trackObj, );
    U2VariantTrack track = trackObj->getVariantTrack(os);
    CHECK_OP(os, );
    QScopedPointer<U2DbiIterator<U2Variant> > varsIter(trackObj->getVariants(U2_REGION_MAX, os));
    CHECK_OP(os, );

    const QStringList header = getHeader(trackObj, os);
    CHECK_OP(os, );

    QByteArray snpString;
    while (varsIter->hasNext()){
        U2Variant variant = varsIter->next();

        snpString.clear();
        for (int columnNumber = 0; columnNumber <= maxColumnNumber; columnNumber++) {
            if (columnNumber != 0) {
                snpString += COLUMNS_SEPARATOR;
            }

            ColumnRole role = columnRoles.value(columnNumber, ColumnRole_Unknown);
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
                const QString columnTitle = columnNumber < header.size() ? header[columnNumber] : QString::number(columnNumber);
                snpString += variant.additionalInfo.value(columnTitle, ".");
                break;
            }
            default:
                coreLog.trace("Warning: unknown column role (%, line %, column %)");
                break;
            }
        }

        for (int i = maxColumnNumber + 1; i < header.size(); i++) {
            snpString += COLUMNS_SEPARATOR + variant.additionalInfo.value(header[i], ".").toLatin1();
        }

        for (int i = qMax(maxColumnNumber + 1, header.size()); i <= maxColumnNumber + variant.additionalInfo.size(); i++) {
            if (!variant.additionalInfo.contains(QString::number(i))) {
                break;
            }
            snpString += COLUMNS_SEPARATOR + variant.additionalInfo[QString::number(i)].toLatin1();
        }

        snpString += "\n";
        io->writeBlock(snpString);
    }
}

void AbstractVariationFormat::storeHeader(GObject *obj, IOAdapter *io, U2OpStatus &os) {
    CHECK(isSupportHeader, );
    SAFE_POINT_EXT(NULL != obj, os.setError("NULL object"), );

    SAFE_POINT_EXT(GObjectTypes::VARIANT_TRACK == obj->getGObjectType(), os.setError("Invalid GObjectType"), );

    VariantTrackObject *trackObj = qobject_cast<VariantTrackObject*>(obj);
    SAFE_POINT_EXT(NULL != trackObj, os.setError("Can't cast GObject to VariantTrackObject"), );

    const QString metaInfo = getMetaInfo(trackObj, os);
    CHECK_OP(os, );
    if (!metaInfo.isEmpty()) {
        io->writeBlock(metaInfo.toLatin1());
    }

    const QStringList header = getHeader(trackObj, os);
    CHECK_OP(os, );
    if (!header.isEmpty()) {
        io->writeBlock(header.join(COLUMNS_SEPARATOR).toLatin1() + "\n");
    }
}

QString AbstractVariationFormat::getMetaInfo(const VariantTrackObject *variantTrackObject, U2OpStatus &os) {
    DbiConnection connection(variantTrackObject->getEntityRef().dbiRef, os);
    CHECK_OP(os, "");
    return U2AttributeUtils::findStringAttribute(connection.dbi->getAttributeDbi(), variantTrackObject->getEntityRef().entityId, U2VariantTrack::META_INFO_ATTIBUTE, os).value;
}

QStringList AbstractVariationFormat::getHeader(const VariantTrackObject *variantTrackObject, U2OpStatus &os) {
    DbiConnection connection(variantTrackObject->getEntityRef().dbiRef, os);
    CHECK_OP(os, QStringList());
    const QString packedHeader = U2AttributeUtils::findStringAttribute(connection.dbi->getAttributeDbi(), variantTrackObject->getEntityRef().entityId, U2VariantTrack::HEADER_ATTIBUTE, os).value;
    return StrPackUtils::unpackStringList(packedHeader);
}

} // U2
