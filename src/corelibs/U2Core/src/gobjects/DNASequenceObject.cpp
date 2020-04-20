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

#include <QApplication>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2AttributeUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "DNASequenceObject.h"
#include "GObjectTypes.h"

namespace U2 {

#define NO_LENGTH_CONSTRAINT -1
U2SequenceObjectConstraints::U2SequenceObjectConstraints(QObject* p)
    : GObjectConstraints(GObjectTypes::SEQUENCE, p), sequenceSize(NO_LENGTH_CONSTRAINT) {}

//////////////////////////////////////////////////////////////////////////
// U2SequenceObject

U2SequenceObject::U2SequenceObject(const QString& name, const U2EntityRef& seqRef, const QVariantMap& hintsMap)
    : GObject(GObjectTypes::SEQUENCE, name, hintsMap), cachedAlphabet(NULL), cachedLength(-1), cachedCircular(TriState_Unknown) {
    entityRef = seqRef;
}

bool U2SequenceObject::checkConstraints(const GObjectConstraints* c) const {
    const U2SequenceObjectConstraints* dnac = qobject_cast<const U2SequenceObjectConstraints*>(c);
    SAFE_POINT(dnac != NULL, "Not a U2SequenceObjectConstraints!", false);

    if (dnac->sequenceSize != NO_LENGTH_CONSTRAINT) {
        qint64 seqLen = getSequenceLength();
        if (seqLen != dnac->sequenceSize) {
            return false;
        }
    }
    if (dnac->alphabetType != DNAAlphabet_RAW) {
        const DNAAlphabet* dalphabet = getAlphabet();
        SAFE_POINT(dalphabet != NULL, "U2SequenceObject::no alphabet", false);
        DNAAlphabetType aType = dalphabet->getType();
        if (dnac->alphabetType != aType) {
            return false;
        }
    }
    return true;
}


qint64 U2SequenceObject::getSequenceLength() const {
    if (cachedLength == -1) {
        updateCachedValues();
    }
    return cachedLength;
}

QString U2SequenceObject::getSequenceName() const {
    if (cachedName.isEmpty()) {
        updateCachedValues();
    }
    return cachedName;
}

#define FETCH_SEQUENCE(seqGet, seq, entityRef) \
    if (!seqGet) { \
        U2OpStatus2Log os; \
        DbiConnection con(entityRef.dbiRef, os); \
        CHECK_OP(os, DNASequence()); \
        seq = con.dbi->getSequenceDbi()->getSequenceObject(entityRef.entityId, os); \
        CHECK_OP(os, DNASequence()); \
        seqGet = true; \
        }

DNASequence U2SequenceObject::getSequence(const U2Region &region, U2OpStatus& os) const {
    U2Sequence seq;
    bool seqGet = false;

    const DNAAlphabet* alpha = cachedAlphabet;
    if (!cachedAlphabet) {
        FETCH_SEQUENCE(seqGet, seq, entityRef);
        cachedAlphabet = alpha = U2AlphabetUtils::getById(seq.alphabet);
    }
    QString seqName = cachedName;
    if (cachedName.isEmpty()) {
        FETCH_SEQUENCE(seqGet, seq, entityRef);
        seqName = cachedName = seq.visualName;
    }
    DNASequence res(seqName, getSequenceData(region, os), alpha);
    CHECK_OP(os, DNASequence());

    if (cachedCircular == TriState_Unknown) {
        FETCH_SEQUENCE(seqGet, seq, entityRef);
        cachedCircular = seq.circular ? TriState_Yes : TriState_No;
    }
    res.circular = (cachedCircular == TriState_Yes);

    res.quality = getQuality();
    res.info = getSequenceInfo();
    return res;
}

DNASequence U2SequenceObject::getWholeSequence(U2OpStatus& os) const {
    return getSequence(U2_REGION_MAX, os);
}

QByteArray U2SequenceObject::getWholeSequenceData(U2OpStatus& os) const {
    return getSequenceData(U2_REGION_MAX, os);
}

bool U2SequenceObject::isCircular() const {
    if (cachedCircular == TriState_Unknown) {
        updateCachedValues();
    }
    return cachedCircular == TriState_Yes;
}

const DNAAlphabet* U2SequenceObject::getAlphabet() const {
    if (cachedAlphabet == NULL) {
        updateCachedValues();
    }
    return cachedAlphabet;
}

void U2SequenceObject::setWholeSequence(const DNASequence& seq) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QVariantMap hints;
    con.dbi->getSequenceDbi()->updateSequenceData(entityRef.entityId, U2_REGION_MAX, seq.seq, hints, os);
    CHECK_OP(os, );
    cachedLastAccessedRegion = QPair<U2Region, QByteArray>();
    if (!seq.quality.isEmpty()) {
        setQuality(seq.quality);
    }
    cachedLength = -1;
    setModified(true);
    emit si_sequenceChanged();
}

QByteArray U2SequenceObject::getSequenceData(const U2Region& r) const {
    U2OpStatus2Log os;
    const QByteArray res = getSequenceData(r, os);
    SAFE_POINT_OP(os, QByteArray());
    return res;
}

QByteArray U2SequenceObject::getSequenceData(const U2Region& r, U2OpStatus& os) const {
    if (!cachedLastAccessedRegion.first.contains(r)) {
        DbiConnection con(entityRef.dbiRef, os);
        CHECK_OP(os, QByteArray());
        const qint64 requestedRegionLength = r.startPos + r.length < cachedLength - 1 ? r.length + 1 : r.length;

        const U2Region requestingRegion(r.startPos, requestedRegionLength);
        const QByteArray res = con.dbi->getSequenceDbi()->getSequenceData(entityRef.entityId, requestingRegion, os);
        CHECK_OP(os, QByteArray());

        cachedLastAccessedRegion.first = requestingRegion;
        cachedLastAccessedRegion.second = res;
    }
    return cachedLastAccessedRegion.second.mid(r.startPos - cachedLastAccessedRegion.first.startPos, r.length);
}

bool U2SequenceObject::isValidDbiObject(U2OpStatus &os) {
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, false);
    U2Sequence s = con.dbi->getSequenceDbi()->getSequenceObject(entityRef.entityId, os);
    CHECK_OP(os, false);
    if (U2AlphabetUtils::getById(s.alphabet) == NULL) {
        os.setError(tr("Internal error, sequence alphabet id '%1' is invalid").arg(s.alphabet.id));
        return false;
    }
    return true;
}

void U2SequenceObject::replaceRegion(const U2Region& region, const DNASequence& seq, U2OpStatus& os) {
    replaceRegion(entityRef.entityId, region, seq, os);
}

void U2SequenceObject::replaceRegion(const U2DataId &masterId, const U2Region &region, const DNASequence &seq, U2OpStatus &os) {
    // seq.alphabet == NULL - for tests.
    CHECK_EXT(seq.alphabet == getAlphabet() || seq.seq.isEmpty() || seq.alphabet == NULL,
        os.setError(tr("Modified sequence & region have different alphabet")), );

    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QVariantMap hints;
    con.dbi->getSequenceDbi()->updateSequenceData(masterId, entityRef.entityId, region, seq.seq, hints, os);
    cachedLength = -1;
    if (region.intersects(cachedLastAccessedRegion.first)) {
        cachedLastAccessedRegion = QPair<U2Region, QByteArray>();
    }
    setModified(true);
    emit si_sequenceChanged();
}

void U2SequenceObject::removeRegion(U2OpStatus &os, const U2Region &region) {
    replaceRegion(region, DNASequence(), os);
}

GObject * U2SequenceObject::clone(const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hints) const {
    DbiOperationsBlock opBlock(dbiRef, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, NULL);

    DbiConnection srcCon(this->entityRef.dbiRef, os);
    CHECK_OP(os, NULL);
    DbiConnection dstCon(dbiRef, true, os);
    CHECK_OP(os, NULL);

    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);
    const QString &dstFolder = gHints.get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    U2Sequence seq = U2SequenceUtils::copySequence(entityRef, dbiRef, dstFolder, os);
    CHECK_OP(os, NULL);

    U2SequenceObject* res = new U2SequenceObject(seq.visualName, U2EntityRef(dbiRef, seq.id), gHints.getMap());

    U2AttributeUtils::copyObjectAttributes(entityRef.entityId, seq.id, srcCon.dbi->getAttributeDbi(), dstCon.dbi->getAttributeDbi(), os);
    CHECK_OP(os, NULL);

    return res;
}

void U2SequenceObject::setCircular(bool isCircular) {
    TriState newVal = isCircular ? TriState_Yes : TriState_No;
    if (newVal == cachedCircular) {
        return;
    }
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    U2Sequence u2seq = con.dbi->getSequenceDbi()->getSequenceObject(entityRef.entityId, os);
    CHECK_OP(os, );
    u2seq.circular = isCircular;
    con.dbi->getSequenceDbi()->updateSequenceObject(u2seq, os);
    CHECK_OP(os, );
    cachedCircular = newVal;
    setModified(true);
    emit si_sequenceCircularStateChanged();
}

void U2SequenceObject::setQuality(const DNAQuality& q) {
    U2SequenceUtils::setQuality(entityRef, q);
}

DNAQuality U2SequenceObject::getQuality() const {
    //TODO: may be remove redundant checks
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    DNAQuality res;
    QList<U2DataId> idQualList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::FASTQ_QUAL_CODES, os);
    CHECK_OP(os, res);
    QList<U2DataId> idQualTypeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, DNAInfo::FASTQ_QUAL_TYPE, os);
    CHECK_OP(os, res);
    if (!idQualList.isEmpty() && !idQualTypeList.isEmpty()) {
        res.qualCodes = con.dbi->getAttributeDbi()->getByteArrayAttribute(idQualList.first(), os).value;
        CHECK_OP(os, DNAQuality());
        res.type = (DNAQualityType)con.dbi->getAttributeDbi()->getIntegerAttribute(idQualTypeList.first(), os).value;
        CHECK_OP(os, DNAQuality());
    }
    return res;
}

void U2SequenceObject::setSequenceInfo(const QVariantMap& info) {
    U2OpStatus2Log os;
    U2SequenceUtils::setSequenceInfo(os, entityRef, info);
    CHECK_OP(os, );
}

QVariantMap U2SequenceObject::getSequenceInfo() const {
    U2OpStatus2Log os;
    QVariantMap variantMap = U2SequenceUtils::getSequenceInfo(os, entityRef, getSequenceName());
    CHECK_OP(os, QVariantMap());

    return variantMap;
}

QString U2SequenceObject::getStringAttribute(const QString& seqAttr) const {
    return getSequenceInfo().value(seqAttr).toString();
}

void U2SequenceObject::setStringAttribute(const QString& newStringAttributeValue, const QString& type) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> oldStringAttributeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, type, os);
    CHECK_OP(os, );
    if (!oldStringAttributeList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(oldStringAttributeList.first(), os);
        CHECK_OP(os, );
    }
    U2StringAttribute newStringAttribute(entityRef.entityId, type, newStringAttributeValue);
    con.dbi->getAttributeDbi()->createStringAttribute(newStringAttribute, os);
    CHECK_OP(os, );
}

qint64 U2SequenceObject::getIntegerAttribute(const QString& seqAttr) const {
    return getSequenceInfo().value(seqAttr).toInt();
}

void U2SequenceObject::setIntegerAttribute(int newIntegerAttributeValue, const QString& type) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> oldIntegerAttributeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, type, os);
    CHECK_OP(os, );
    if (!oldIntegerAttributeList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(oldIntegerAttributeList.first(), os);
        CHECK_OP(os, );
    }
    U2IntegerAttribute newIntegerAttribute(entityRef.entityId, type, newIntegerAttributeValue);
    con.dbi->getAttributeDbi()->createIntegerAttribute(newIntegerAttribute, os);
    CHECK_OP(os, );
}

double U2SequenceObject::getRealAttribute(const QString& seqAttr) const {
    return getSequenceInfo().value(seqAttr).toReal();
}

void U2SequenceObject::setRealAttribute(double newRealAttributeValue, const QString& type) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> oldRealAttributeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, type, os);
    CHECK_OP(os, );
    if (!oldRealAttributeList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(oldRealAttributeList.first(), os);
        CHECK_OP(os, );
    }
    U2RealAttribute newRealAttribute(entityRef.entityId, type, newRealAttributeValue);
    con.dbi->getAttributeDbi()->createRealAttribute(newRealAttribute, os);
    CHECK_OP(os, );
}

QByteArray U2SequenceObject::getByteArrayAttribute(const QString& seqAttr) const {
    return getSequenceInfo().value(seqAttr).toByteArray();
}

void U2SequenceObject::setByteArrayAttribute(const QByteArray& newByteArrayAttributeValue, const QString& type) {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );
    QList<U2DataId> oldByteArrayAttributeList = con.dbi->getAttributeDbi()->getObjectAttributes(entityRef.entityId, type, os);
    CHECK_OP(os, );
    if (!oldByteArrayAttributeList.isEmpty()) {
        con.dbi->getAttributeDbi()->removeObjectAttributes(oldByteArrayAttributeList.first(), os);
        CHECK_OP(os, );
    }
    U2ByteArrayAttribute newByteArrayAttribute(entityRef.entityId, type, newByteArrayAttributeValue);
    con.dbi->getAttributeDbi()->createByteArrayAttribute(newByteArrayAttribute, os);
}

void U2SequenceObject::updateCachedValues() const {
    U2OpStatus2Log os;
    DbiConnection con(entityRef.dbiRef, os);
    CHECK_OP(os, );

    U2Sequence seq = con.dbi->getSequenceDbi()->getSequenceObject(entityRef.entityId, os);
    CHECK_OP(os, );

    cachedLength = seq.length;
    cachedName = seq.visualName;
    cachedAlphabet = U2AlphabetUtils::getById(seq.alphabet);
    cachedCircular = seq.circular ? TriState_Yes : TriState_No;

    SAFE_POINT(cachedAlphabet != NULL, "Invalid sequence alphabet", );
}

void U2SequenceObject::sl_resetDataCaches() {
    cachedLastAccessedRegion = QPair<U2Region, QByteArray>();
    cachedLength = -1;
}

void U2SequenceObject::setGObjectName(const QString &newName) {
    CHECK(cachedName != newName, );

    GObject::setGObjectName(newName);
    cachedName = GObject::getGObjectName();
}

} //namespace
