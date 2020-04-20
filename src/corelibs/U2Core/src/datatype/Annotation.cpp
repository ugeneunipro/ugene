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

#include "Annotation.h"

#include <QTextDocument>

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureKeys.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

const QString QUALIFIER_NAME_CIGAR = "cigar";
const QString QUALIFIER_NAME_SUBJECT = "subj_seq";

namespace U2 {

Annotation::Annotation(const U2DataId &featureId, const SharedAnnotationData &data, AnnotationGroup *parentGroup, AnnotationTableObject *parentObject)
    : U2Entity(featureId), parentObject(parentObject), data(data), group(parentGroup) {
    SAFE_POINT(NULL != parentGroup, L10N::nullPointerError("Annotation group"), );
    SAFE_POINT(NULL != parentObject, L10N::nullPointerError("Annotation table object"), );
    SAFE_POINT(hasValidId(), "Invalid DB reference", );
}

AnnotationTableObject *Annotation::getGObject() const {
    return parentObject;
}

const SharedAnnotationData &Annotation::getData() const {
    return data;
}

QString Annotation::getName() const {
    return data->name;
}

U2FeatureType Annotation::getType() const {
    return data->type;
}

void Annotation::setName(const QString &name) {
    SAFE_POINT(!name.isEmpty(), "Attempting to set an empty name for an annotation!", );
    CHECK(name != data->name, );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureName(id, name, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->name = name;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_NameChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

void Annotation::setType(U2FeatureType type) {
    CHECK(type != data->type, );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureType(id, type, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->type = type;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_TypeChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

bool Annotation::isOrder() const {
    return data->isOrder();
}

bool Annotation::isJoin() const {
    return data->isJoin();
}

bool Annotation::isBond() const {
    return data->isBond();
}

U2Strand Annotation::getStrand() const {
    return data->getStrand();
}

void Annotation::setStrand(const U2Strand &strand) {
    CHECK(strand != data->location->strand, );

    U2Location newLocation = data->location;
    newLocation->strand = strand;

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), newLocation, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = newLocation;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

U2LocationOperator Annotation::getLocationOperator() const {
    return data->getLocationOperator();
}

void Annotation::setLocationOperator(U2LocationOperator op) {
    CHECK(op != data->location->op, );

    U2Location newLocation = data->location;
    newLocation->op = op;

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), newLocation, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = newLocation;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

U2Location Annotation::getLocation() const {
    return data->location;
}

void Annotation::setLocation(const U2Location &location) {
    CHECK(*(data->location) != *location, );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), location, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = location;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

QVector<U2Region> Annotation::getRegions() const {
    return data->getRegions();
}

qint64 Annotation::getRegionsLen() const {
    qint64 len = 0;
    foreach (const U2Region &region, getRegions()) {
        len += region.length;
    }
    return len;
}

void Annotation::updateRegions(const QVector<U2Region> &regions) {
    SAFE_POINT(!regions.isEmpty(), "Attempting to assign the annotation to an empty region!", );
    CHECK(regions != data->location->regions, );

    U2Location newLocation = data->location;
    newLocation->regions = regions;

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), newLocation, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = newLocation;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

void Annotation::addLocationRegion(const U2Region &reg) {
    SAFE_POINT(!reg.isEmpty(), "Attempting to annotate an empty region!", );
    CHECK(!data->location->regions.contains(reg), );

    U2Location newLocation = data->location;
    newLocation->regions.append(reg);

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), newLocation, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = newLocation;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

QVector<U2Qualifier> Annotation::getQualifiers() const {
    return data->qualifiers;
}

void Annotation::addQualifier(const U2Qualifier &q) {
    SAFE_POINT(q.isValid(), "Invalid annotation qualifier detected!", );

    U2OpStatusImpl os;
    U2FeatureUtils::addFeatureKey(id, U2FeatureKey(q.name, q.value), parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->qualifiers.append(q);

    parentObject->setModified(true);
    QualifierModification md(AnnotationModification_QualifierAdded, this, q);
    parentObject->emit_onAnnotationsModified(md);
}

void Annotation::removeQualifier(const U2Qualifier &q) {
    SAFE_POINT(q.isValid(), "Invalid annotation qualifier detected!", );

    U2OpStatusImpl os;
    U2FeatureUtils::removeFeatureKey(id, U2FeatureKey(q.name, q.value), parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    for (int i = 0, n = data->qualifiers.size(); i < n; ++i) {
        if (data->qualifiers[i] == q) {
            data->qualifiers.remove(i);
            break;
        }
    }

    parentObject->setModified(true);
    QualifierModification md(AnnotationModification_QualifierRemoved, this, q);
    parentObject->emit_onAnnotationsModified(md);
}

bool Annotation::isCaseAnnotation() const {
    return data->caseAnnotation;
}

void Annotation::setCaseAnnotation(bool caseAnnotation) {
    CHECK(caseAnnotation != data->caseAnnotation, );

    U2OpStatusImpl os;
    if (caseAnnotation) {
        U2FeatureUtils::addFeatureKey(id, U2FeatureKey(U2FeatureKeyCase, QString()), parentObject->getEntityRef().dbiRef, os);
    } else {
        U2FeatureUtils::removeFeatureKey(id, U2FeatureKey(U2FeatureKeyCase, QString()), parentObject->getEntityRef().dbiRef, os);
    }
    SAFE_POINT_OP(os, );

    data->caseAnnotation = caseAnnotation;
}

AnnotationGroup *Annotation::getGroup() const {
    return group;
}

void Annotation::setGroup(AnnotationGroup *newGroup) {
    CHECK(newGroup != group, );
    SAFE_POINT(NULL != newGroup, L10N::nullPointerError("annotation group"), );
    SAFE_POINT(parentObject == newGroup->getGObject(), "Illegal object!", );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureParent(id, newGroup->id, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    group = newGroup;
}

void Annotation::findQualifiers(const QString &name, QList<U2Qualifier> &res) const {
    SAFE_POINT(!name.isEmpty(), "Attempting to find a qualifier having an empty name!", );

    foreach (const U2Qualifier &qual, data->qualifiers) {
        if (name == qual.name) {
            res << qual;
        }
    }
}

QString Annotation::findFirstQualifierValue(const QString &name) const {
    SAFE_POINT(!name.isEmpty(), "Attempting to find a qualifier having an empty name!", QString::null);

    foreach (const U2Qualifier &qual, data->qualifiers) {
        if (name == qual.name) {
            return qual.value;
        }
    }
    return QString::null;
}

bool Annotation::annotationLessThan(Annotation *first, Annotation *second) {
    SAFE_POINT(NULL != first && NULL != second, "Invalid annotation detected", false);

    AnnotationGroup *firstGroup = first->getGroup();
    SAFE_POINT(NULL != firstGroup, L10N::nullPointerError("annotation group"), false);
    AnnotationGroup *secondGroup = second->getGroup();
    SAFE_POINT(NULL != secondGroup, L10N::nullPointerError("annotation group"), false);

    return firstGroup->getName() < secondGroup->getName();
}

bool Annotation::annotationLessThanByRegion(Annotation *first, Annotation *second) {
    SAFE_POINT(NULL != first && NULL != second, "Invalid annotation detected", false);

    const U2Location firstLocation = first->getLocation();
    const U2Location secondLocation = second->getLocation();
    SAFE_POINT(!firstLocation->isEmpty() && !secondLocation->isEmpty(), "Invalid annotation's location detected!", false);

    const U2Region &r1 = firstLocation->regions.first();
    const U2Region &r2 = secondLocation->regions.first();
    return r1 < r2;
}

bool Annotation::isValidQualifierName(const QString &name) {
    return U2Qualifier::isValidQualifierName(name);
}

bool Annotation::isValidQualifierValue(const QString &value) {
    return U2Qualifier::isValidQualifierValue(value);
}

namespace {

const int ANNOTATION_NAME_MAX_LENGTH = 32767;

QBitArray getValidAnnotationChars() {
    QBitArray validChars = TextUtils::ALPHA_NUMS;
    validChars[' '] = true;
    validChars['`'] = true;
    validChars['~'] = true;
    validChars['!'] = true;
    validChars['@'] = true;
    validChars['#'] = true;
    validChars['$'] = true;
    validChars['%'] = true;
    validChars['^'] = true;
    validChars['&'] = true;
    validChars['*'] = true;
    validChars['('] = true;
    validChars[')'] = true;
    validChars['-'] = true;
    validChars['_'] = true;
    validChars['='] = true;
    validChars['+'] = true;
    validChars['\\'] = true;
    validChars['|'] = true;
    validChars[','] = true;
    validChars['.'] = true;
    validChars['<'] = true;
    validChars['>'] = true;
    validChars['?'] = true;
    validChars[';'] = true;
    validChars[':'] = true;
    validChars['\''] = true;
    validChars['['] = true;
    validChars[']'] = true;
    validChars['{'] = true;
    validChars['}'] = true;
    validChars['\"'] = false;
    validChars['/'] = false;
    return validChars;
}

}    // namespace

bool Annotation::isValidAnnotationName(const QString &n) {
    if (n.isEmpty() || ANNOTATION_NAME_MAX_LENGTH < n.length()) {
        return false;
    }

    static const QBitArray validChars = getValidAnnotationChars();

    QByteArray name = n.toLocal8Bit();
    if (!TextUtils::fits(validChars, name.constData(), name.size())) {
        return false;
    }
    if (' ' == name[0] || ' ' == name[name.size() - 1]) {
        return false;
    }
    return true;
}

QString Annotation::produceValidAnnotationName(const QString &name) {
    QString result = name.trimmed();
    if (result.isEmpty()) {
        return U2FeatureTypes::getVisualName(U2FeatureTypes::MiscFeature);
    }
    if (result.length() > ANNOTATION_NAME_MAX_LENGTH) {
        result = result.left(ANNOTATION_NAME_MAX_LENGTH);
    }

    static const QBitArray validChars = getValidAnnotationChars();

    for (int i = 0; i < result.size(); i++) {
        unsigned char c = result[i].toLatin1();
        if (c == '\"') {
            result[i] = '\'';
        } else if (!validChars[c]) {
            result[i] = '_';
        }
    }
    return result;
}

static QList<U2CigarToken> parseCigar(const QString &cigar) {
    QList<U2CigarToken> cigarTokens;

    QRegExp rx("(\\d+)(\\w)");

    int pos = 0;
    while (-1 != (pos = rx.indexIn(cigar, pos))) {
        if (2 != rx.captureCount()) {
            break;
        }
        int count = rx.cap(1).toInt();
        QString cigarChar = rx.cap(2);

        if (cigarChar == "M") {
            cigarTokens.append(U2CigarToken(U2CigarOp_M, count));
        } else if (cigarChar == "I") {
            cigarTokens.append(U2CigarToken(U2CigarOp_I, count));
        } else if (cigarChar == "D") {
            cigarTokens.append(U2CigarToken(U2CigarOp_D, count));
        } else if (cigarChar == "X") {
            cigarTokens.append(U2CigarToken(U2CigarOp_X, count));
        } else {
            break;
        }

        pos += rx.matchedLength();
    }

    return cigarTokens;
}

static QString getAlignmentTip(const QString &ref, const QList<U2CigarToken> &tokens, int maxVisibleSymbols) {
    QString alignmentTip;

    if (tokens.isEmpty()) {
        return ref;
    }

    int pos = 0;

    QList<int> mismatchPositions;

    foreach (const U2CigarToken &t, tokens) {
        if (U2CigarOp_M == t.op) {
            alignmentTip += ref.mid(pos, t.count);
            pos += t.count;
        } else if (t.op == U2CigarOp_X) {
            alignmentTip += ref.mid(pos, t.count);
            mismatchPositions.append(pos);
            pos += t.count;
        } else if (U2CigarOp_I == t.op) {
            // gap already present in sequence?
            pos += t.count;
        }
    }

    if (maxVisibleSymbols < alignmentTip.length()) {
        alignmentTip = alignmentTip.left(maxVisibleSymbols);
        alignmentTip += " ... ";
    }

    // make mismatches bold
    int offset = 0;
    static const int OFFSET_LEN = QString("<b></b>").length();
    foreach (int pos, mismatchPositions) {
        int newPos = pos + offset;
        if (newPos + 1 >= alignmentTip.length()) {
            break;
        }
        alignmentTip.replace(newPos, 1, QString("<b>%1</b>").arg(alignmentTip.at(newPos)));
        offset += OFFSET_LEN;
    }

    return alignmentTip;
}

QString Annotation::getQualifiersTip(const SharedAnnotationData &data, int maxRows, U2SequenceObject *seqObj, DNATranslation *complTT, DNATranslation *aminoTT) {
    SAFE_POINT(0 < maxRows, "Invalid maximum row count parameter passed!", QString());
    QString tip;

    int rows = 0;
    const qint64 QUALIFIER_VALUE_CUT = 40;

    QString cigar;
    QString ref;
    if (!data->qualifiers.isEmpty()) {
        tip += "<nobr>";
        bool first = true;
        foreach (const U2Qualifier &q, data->qualifiers) {
            if (++rows > maxRows) {
                break;
            }
            if (q.name == QUALIFIER_NAME_CIGAR) {
                cigar = q.value;
            } else if (q.name == QUALIFIER_NAME_SUBJECT) {
                ref = q.value;
                continue;
            }
            QString val = q.value;
            if (val.length() > QUALIFIER_VALUE_CUT) {
                val = val.left(QUALIFIER_VALUE_CUT) + " ...";
            }
            if (first) {
                first = false;
            } else {
                tip += "<br>";
            }
            tip += "<b>" + q.name.toHtmlEscaped() + "</b> = " + val.toHtmlEscaped();
        }
        tip += "</nobr>";
    }

    if (!cigar.isEmpty() && !ref.isEmpty()) {
        const QList<U2CigarToken> tokens = parseCigar(cigar);
        const QString alignmentTip = getAlignmentTip(ref, tokens, QUALIFIER_VALUE_CUT);
        tip += "<br><b>Reference</b> = " + alignmentTip;
        rows++;
    }

    bool canShowSeq = true;
    const int seqLen = (NULL != seqObj) ? seqObj->getSequenceLength() : 0;
    foreach (const U2Region &r, data->location->regions) {
        if (r.endPos() > seqLen) {
            canShowSeq = false;
        }
    }

    if (NULL != seqObj && rows <= maxRows && (data->location->strand.isCompementary() || complTT != nullptr) && canShowSeq) {
        QVector<U2Region> loc = data->location->regions;
        QString seqVal;
        QString aminoVal;
        bool complete = true;
        QList<RegionsPair> merged = U1AnnotationUtils::mergeAnnotatiedRegionsAroundJunctionPoint(loc, seqLen);
        bool isComplementary = data->location->strand.isCompementary() && nullptr != complTT;
        if (isComplementary) {
            std::reverse(merged.begin(), merged.end());
        }
        bool hasAnnotatiedRegionsContainJunctionPoint = seqObj->isCircular() && U1AnnotationUtils::isAnnotationContainsJunctionPoint(merged);
        foreach(const RegionsPair& pair, merged) {
            if (!seqVal.isEmpty()) {
                seqVal += "^";
            }
            if (!aminoVal.isEmpty()) {
                aminoVal += "^";
            }
            qint64 firstRegionLength = qMin<qint64>(pair.first.length, QUALIFIER_VALUE_CUT - seqVal.length());
            qint64 secondPartRegionLength = 0;
            if (firstRegionLength != pair.first.length) {
                complete = false;
            }
            U2Region firstRegion;
            U2Region secondRegion;
            if (hasAnnotatiedRegionsContainJunctionPoint && !pair.second.isEmpty()) {
                if (isComplementary) {
                    /*
                     * If the sequence is circular and the annotation is complementary the region from 0 to N should be shown first from N to 0 and the region from M to 'sequeceLength' should be shown second from 'sequeceLength' to M
                     */
                    firstRegionLength = qMin<qint64>(pair.second.length, QUALIFIER_VALUE_CUT - seqVal.length());
                    if (firstRegionLength != pair.second.length) {
                        complete = false;
                    }
                    firstRegion = U2Region((pair.second.endPos() - firstRegionLength), firstRegionLength);
                    secondPartRegionLength = qMin<qint64>(pair.first.length, QUALIFIER_VALUE_CUT - (seqVal.length() + firstRegionLength));
                    if (secondPartRegionLength != pair.first.length) {
                        complete = false;
                    }
                    secondRegion = U2Region((pair.first.endPos() - secondPartRegionLength), secondPartRegionLength);
                } else {
                    firstRegion = U2Region(pair.first.startPos, firstRegionLength);
                    secondPartRegionLength = qMin<qint64>(pair.second.length, QUALIFIER_VALUE_CUT - (seqVal.length() + firstRegion.length));
                    if (secondPartRegionLength != pair.second.length) {
                        complete = false;
                    }
                    secondRegion = U2Region(pair.second.startPos, secondPartRegionLength);
                }
            } else {
                if (isComplementary) {
                    firstRegion = U2Region((pair.first.endPos() - firstRegionLength), firstRegionLength);
                } else {
                    firstRegion = U2Region(pair.first.startPos, firstRegionLength);
                }
            }
            QByteArray first = seqObj->getSequenceData(firstRegion);
            if (isComplementary) {
                complTT->translate(first.data(), firstRegionLength);
                TextUtils::reverse(first.data(), firstRegionLength);
            }
            QByteArray second;
            if (!secondRegion.isEmpty()) {
                second = seqObj->getSequenceData(secondRegion);
                if (isComplementary) {
                    complTT->translate(second.data(), secondPartRegionLength);
                    TextUtils::reverse(second.data(), secondPartRegionLength);
                }
            }
            QByteArray resultSequenceTip = first + second;
            seqVal += QString::fromLocal8Bit(resultSequenceTip);
            if (nullptr != aminoTT) {
                const int aminoLen = aminoTT->translate(resultSequenceTip.data(), firstRegionLength + secondPartRegionLength);
                aminoVal += QString::fromLocal8Bit(resultSequenceTip, aminoLen);
            }
            if (seqVal.length() >= QUALIFIER_VALUE_CUT) {
                complete = complete && merged.last() == pair;
                break;
            }
        }
        if (!complete || seqVal.length() > QUALIFIER_VALUE_CUT) {
            seqVal = seqVal.left(QUALIFIER_VALUE_CUT) + " ...";
        }
        if (!complete || aminoVal.length() > QUALIFIER_VALUE_CUT) {
            aminoVal = aminoVal.left(QUALIFIER_VALUE_CUT) + " ...";
        }
        if (!tip.isEmpty()) {
            tip += "<br>";
        }
        SAFE_POINT(!seqVal.isEmpty(), "Empty sequence detected!", QString());
        tip += "<nobr><b>" + QObject::tr("Sequence") + "</b> = " + seqVal.toHtmlEscaped() + "</nobr>";
        rows++;

        if (rows <= maxRows && NULL != aminoTT) {
            tip += "<br>";
            tip += "<nobr><b>" + QObject::tr("Translation") + "</b> = " + aminoVal.toHtmlEscaped() + "</nobr>";
        }
    }
    return tip;
}

}    // namespace U2
