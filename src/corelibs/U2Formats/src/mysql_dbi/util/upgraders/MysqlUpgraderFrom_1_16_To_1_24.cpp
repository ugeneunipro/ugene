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

#include <U2Core/U2AttributeUtils.h>

#include "MysqlUpgraderFrom_1_16_To_1_24.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/MysqlObjectDbi.h"
#include "mysql_dbi/util/MysqlHelpers.h"

namespace U2 {

const QString MysqlUpgraderFrom_1_16_To_1_24::META_INFO_MARKER = "##";
const QString MysqlUpgraderFrom_1_16_To_1_24::HEADER_MARKER = "#";
const QString MysqlUpgraderFrom_1_16_To_1_24::COLUMN_SEPARATOR = "\t";

MysqlUpgraderFrom_1_16_To_1_24::MysqlUpgraderFrom_1_16_To_1_24(MysqlDbi *dbi)
    : MysqlUpgrader(Version::parseVersion("1.16.0"), Version::parseVersion("1.24.0"), dbi)
{

}

void MysqlUpgraderFrom_1_16_To_1_24::upgrade(U2OpStatus &os) const {
    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    upgradeVariantDbi(os);
    CHECK_OP(os, );

    dbi->setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, versionTo.text, os);
}

void MysqlUpgraderFrom_1_16_To_1_24::upgradeVariantDbi(U2OpStatus &os) const {
    coreLog.trace("Variant DBI upgrading");

    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    QMap<U2DataId, QStringList> trackId2header;

    extractAttributes(os, trackId2header);
    CHECK_OP(os, );

    repackInfo(os, trackId2header);
    CHECK_OP(os, );

    updateScheme(os);
}

namespace {

QString convertInfo(const QString &additionalInfo, const QStringList &header) {
    StrStrMap convertedInfoMap;
    CHECK(!additionalInfo.isEmpty(), QString());
    QStringList splittedInfo = additionalInfo.split("\t", QString::SkipEmptyParts);
    CHECK(!splittedInfo.isEmpty(), QString());

    convertedInfoMap.insert(U2Variant::VCF4_QUAL, splittedInfo.takeFirst());

    if (!splittedInfo.isEmpty()) {
        convertedInfoMap.insert(U2Variant::VCF4_FILTER, splittedInfo.takeFirst());
    }

    if (!splittedInfo.isEmpty()) {
        convertedInfoMap.insert(U2Variant::VCF4_INFO, splittedInfo.takeFirst());
    }

    static const int maxVcf4MandatoryColumnNumber = 7;      // VCF4 format supposes 8 mandatory columns
    for (int i = maxVcf4MandatoryColumnNumber + 1; i < header.size(); i++) {
        convertedInfoMap.insert(header[i], splittedInfo.isEmpty() ? "." : splittedInfo.takeFirst());
    }

    if (!splittedInfo.isEmpty()) {
        // There is no possibility to split the data correctly, because it was splitted by spaces not by tabulations
        convertedInfoMap.insert(QString::number(qMax(maxVcf4MandatoryColumnNumber, header.size()) + 1), splittedInfo.join("\t"));
    }

    return StrPackUtils::packMap(convertedInfoMap);
}

}

void MysqlUpgraderFrom_1_16_To_1_24::repackInfo(U2OpStatus &os, const QMap<U2DataId, QStringList> &trackId2header) const {
    coreLog.trace("Additional info repacking");

    MysqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    const qint64 variantsCount = U2SqlQuery("SELECT count(*) from Variant", dbi->getDbRef(), os).selectInt64();

    static QString getQueryString ("SELECT id, track, additionalInfo FROM Variant");
    static QString setQueryString ("UPDATE Variant SET additionalInfo = :additionalInfo WHERE id = :id");
    U2SqlQuery getQuery(getQueryString, dbi->getDbRef(), os);
    U2SqlQuery setQuery(setQueryString, dbi->getDbRef(), os);

    QSet<U2DataId> trackIds;

    qint64 number = 0;
    while (getQuery.step()) {
        CHECK_OP(os, );
        const qint64 dbiId = getQuery.getInt64(0);
        const QString additionalInfo = getQuery.getString(2);
        const U2DataId trackId = getQuery.getDataId(1, U2Type::VariantTrack);
        trackIds << trackId;

        const QString convertedInfo = convertInfo(additionalInfo, trackId2header[trackId]);

        setQuery.bindString(":additionalInfo", convertedInfo);
        setQuery.bindInt64(":id", dbiId);
        setQuery.execute();
        CHECK_OP(os, );

        number++;
        if (number % 100 == 0) {
            coreLog.trace(QString("Variants processed: %1/%2").arg(number).arg(variantsCount));
        }
    }

    if (number % 100 != 0) {
        coreLog.trace(QString("Variants processed: %1/%2").arg(number).arg(variantsCount));
    }

    number = 0;
    foreach (const U2DataId &trackId, trackIds) {
        MysqlObjectDbi::incrementVersion(trackId, dbi->getDbRef(), os);
        CHECK_OP(os, );

        number++;
        if (number % 10 == 0) {
            coreLog.trace(QString("Object versions processed: %1/%2").arg(number).arg(trackIds.size()));
        }
    }

    if (number % 10 != 0) {
        coreLog.trace(QString("Object versions processed: %1/%2").arg(number).arg(trackIds.size()));
    }
}

void MysqlUpgraderFrom_1_16_To_1_24::extractAttributes(U2OpStatus &os, QMap<U2DataId, QStringList> &trackId2header) const {
    coreLog.trace("Attributes extracting");

    const qint64 tracksCount = U2SqlQuery("SELECT count(*) from VariantTrack", dbi->getDbRef(), os).selectInt64();
    CHECK_OP(os, );

    QScopedPointer<U2DbiIterator<U2VariantTrack> > variantTracksIterator(dbi->getVariantDbi()->getVariantTracks(TrackType_All, os));
    CHECK_OP(os, );

    qint64 number = 0;
    while (variantTracksIterator->hasNext()) {
        U2VariantTrack variantTrack = variantTracksIterator->next();
        CHECK_OP(os, );

        QString metaInfo;
        QStringList header;
        splitFileHeader(variantTrack.fileHeader, metaInfo, header);

        trackId2header.insert(variantTrack.id, header);

        addStringAttribute(os, variantTrack, U2VariantTrack::META_INFO_ATTIBUTE, metaInfo);
        CHECK_OP(os, );
        addStringAttribute(os, variantTrack, U2VariantTrack::HEADER_ATTIBUTE, StrPackUtils::packStringList(header));
        CHECK_OP(os, );

        number++;
        if (number % 10 == 0) {
            coreLog.trace(QString("Variant tracks processed: %1/%2").arg(number).arg(tracksCount));
        }
    }

    if (number % 10 != 0) {
        coreLog.trace(QString("Variant tracks processed: %1/%2").arg(number).arg(tracksCount));
    }
}

void MysqlUpgraderFrom_1_16_To_1_24::updateScheme(U2OpStatus &os) const {
    coreLog.trace("Scheme updating");
    U2SqlQuery("ALTER TABLE VariantTrack DROP COLUMN fileHeader;", dbi->getDbRef(), os).execute();
}

void MysqlUpgraderFrom_1_16_To_1_24::addStringAttribute(U2OpStatus &os, const U2VariantTrack &variantTrack, const QString &attributeName, const QString &attributeValue) const {
    CHECK(!attributeValue.isEmpty(), );
    U2StringAttribute attribute;
    U2AttributeUtils::init(attribute, variantTrack, attributeName);
    attribute.value = attributeValue;
    dbi->getAttributeDbi()->createStringAttribute(attribute, os);
}

void MysqlUpgraderFrom_1_16_To_1_24::splitFileHeader(const QString &fileHeader, QString &metaInfo, QStringList &header) {
    const QStringList lines = fileHeader.split(QRegExp("\\n\\r?"), QString::SkipEmptyParts);
    foreach (const QString &line, lines) {
        if (line.startsWith(META_INFO_MARKER)) {
            metaInfo += line + "\n";
        } else if (line.startsWith(HEADER_MARKER)) {
            header = line.split(COLUMN_SEPARATOR);
        }
    }
}

}   // namespace U2
