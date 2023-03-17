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

#include "Annotation.h"

#include <QTextDocument>

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2FeatureKeys.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>

const QString QUALIFIER_NAME_CIGAR = "cigar";
const QString QUALIFIER_NAME_SUBJECT = "subj_seq";

namespace U2 {

Annotation::Annotation(const U2DataId& featureId, const SharedAnnotationData& data, AnnotationGroup* parentGroup, AnnotationTableObject* parentObject)
    : U2Entity(featureId), parentObject(parentObject), data(data), group(parentGroup) {
    SAFE_POINT(parentGroup != nullptr, L10N::nullPointerError("Annotation group"), );
    SAFE_POINT(parentObject != nullptr, L10N::nullPointerError("Annotation table object"), );
    SAFE_POINT(hasValidId(), "Invalid DB reference", );
}

AnnotationTableObject* Annotation::getGObject() const {
    return parentObject;
}

const SharedAnnotationData& Annotation::getData() const {
    return data;
}

QString Annotation::getName() const {
    return data->name;
}

U2FeatureType Annotation::getType() const {
    return data->type;
}

void Annotation::setName(const QString& name) {
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

void Annotation::setStrand(const U2Strand& strand) {
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

void Annotation::setLocation(const U2Location& location) {
    CHECK(*(data->location) != *location, );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureLocation(id, parentObject->getRootFeatureId(), location, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->location = location;

    parentObject->setModified(true);
    AnnotationModification md(AnnotationModification_LocationChanged, this);
    parentObject->emit_onAnnotationsModified(md);
}

const QVector<U2Region>& Annotation::getRegions() const {
    return data->getRegions();
}

qint64 Annotation::getRegionsLen() const {
    qint64 len = 0;
    foreach (const U2Region& region, getRegions()) {
        len += region.length;
    }
    return len;
}

void Annotation::updateRegions(const QVector<U2Region>& regions) {
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

void Annotation::addLocationRegion(const U2Region& reg) {
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

void Annotation::addQualifier(const U2Qualifier& q) {
    SAFE_POINT(q.isValid(), "Invalid annotation qualifier detected!", );

    U2OpStatusImpl os;
    U2FeatureUtils::addFeatureKey(id, U2FeatureKey(q.name, q.value), parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    data->qualifiers.append(q);

    parentObject->setModified(true);
    QualifierModification md(AnnotationModification_QualifierAdded, this, q);
    parentObject->emit_onAnnotationsModified(md);
}

void Annotation::removeQualifier(const U2Qualifier& q) {
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

AnnotationGroup* Annotation::getGroup() const {
    return group;
}

void Annotation::setGroup(AnnotationGroup* newGroup) {
    CHECK(newGroup != group, );
    SAFE_POINT(newGroup != nullptr, L10N::nullPointerError("annotation group"), );
    SAFE_POINT(parentObject == newGroup->getGObject(), "Illegal object!", );

    U2OpStatusImpl os;
    U2FeatureUtils::updateFeatureParent(id, newGroup->id, parentObject->getEntityRef().dbiRef, os);
    SAFE_POINT_OP(os, );

    group = newGroup;
}

void Annotation::findQualifiers(const QString& name, QList<U2Qualifier>& res) const {
    SAFE_POINT(!name.isEmpty(), "Attempting to find a qualifier having an empty name!", );

    foreach (const U2Qualifier& qual, data->qualifiers) {
        if (name == qual.name) {
            res << qual;
        }
    }
}

QString Annotation::findFirstQualifierValue(const QString& name) const {
    SAFE_POINT(!name.isEmpty(), "Attempting to find a qualifier having an empty name!", QString());

    foreach (const U2Qualifier& qual, data->qualifiers) {
        if (name == qual.name) {
            return qual.value;
        }
    }
    return QString();
}

bool Annotation::annotationLessThan(Annotation* first, Annotation* second) {
    SAFE_POINT(first != nullptr && nullptr != second, "Invalid annotation detected", false);

    AnnotationGroup* firstGroup = first->getGroup();
    SAFE_POINT(firstGroup != nullptr, L10N::nullPointerError("annotation group"), false);
    AnnotationGroup* secondGroup = second->getGroup();
    SAFE_POINT(secondGroup != nullptr, L10N::nullPointerError("annotation group"), false);

    return firstGroup->getName() < secondGroup->getName();
}

bool Annotation::annotationLessThanByRegion(Annotation* first, Annotation* second) {
    SAFE_POINT(first != nullptr && second != nullptr, "Invalid annotation detected", false);

    const U2Location& firstLocation = first->getLocation();
    const U2Location& secondLocation = second->getLocation();
    SAFE_POINT(!firstLocation->isEmpty() && !secondLocation->isEmpty(), "Invalid annotation's location detected!", false);

    const U2Region& r1 = firstLocation->regions.first();
    const U2Region& r2 = secondLocation->regions.first();
    return r1 < r2;
}

bool Annotation::isValidQualifierName(const QString& name) {
    return U2Qualifier::isValidQualifierName(name);
}

bool Annotation::isValidQualifierValue(const QString& value) {
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

}  // namespace

bool Annotation::isValidAnnotationName(const QString& n) {
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

QString Annotation::produceValidAnnotationName(const QString& name) {
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

static QList<U2CigarToken> parseCigar(const QString& cigar) {
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

static QString getAlignmentTip(const QString& ref, const QList<U2CigarToken>& tokens, int maxVisibleSymbols) {
    QString alignmentTip;

    if (tokens.isEmpty()) {
        return ref;
    }

    int cigarPos = 0;
    QList<int> mismatchPositions;
    for (const U2CigarToken& t : qAsConst(tokens)) {
        if (t.op == U2CigarOp_M) {
            alignmentTip += ref.midRef(cigarPos, t.count);
            cigarPos += t.count;
        } else if (t.op == U2CigarOp_X) {
            alignmentTip += ref.midRef(cigarPos, t.count);
            mismatchPositions.append(cigarPos);
            cigarPos += t.count;
        } else if (t.op == U2CigarOp_I) {
            // gap already present in sequence?
            cigarPos += t.count;
        }
    }

    if (maxVisibleSymbols < alignmentTip.length()) {
        alignmentTip = alignmentTip.left(maxVisibleSymbols);
        alignmentTip += " ... ";
    }

    // make mismatches bold
    int offset = 0;
    static const int OFFSET_LEN = QString("<b></b>").length();
    for (int mismatchPos : qAsConst(mismatchPositions)) {
        int newPos = mismatchPos + offset;
        if (newPos + 1 >= alignmentTip.length()) {
            break;
        }
        alignmentTip.replace(newPos, 1, QString("<b>%1</b>").arg(alignmentTip.at(newPos)));
        offset += OFFSET_LEN;
    }

    return alignmentTip;
}

QString Annotation::getQualifiersTip(const SharedAnnotationData& data, int maxRows, U2SequenceObject* seqObj, DNATranslation* complTT, DNATranslation* aminoTT) {
    SAFE_POINT(0 < maxRows, "Invalid maximum row count parameter passed!", QString());
    QString tip;

    int rows = 0;
    static constexpr qint64 MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP = 40;

    QString cigar;
    QString ref;
    if (!data->qualifiers.isEmpty()) {
        tip += "<nobr>";
        bool first = true;
        foreach (const U2Qualifier& q, data->qualifiers) {
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
            if (val.length() > MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP) {
                val = val.left(MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP) + " ...";
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
        const QString alignmentTip = getAlignmentTip(ref, tokens, MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP);
        tip += "<br><b>Reference</b> = " + alignmentTip;
        rows++;
    }

    qint64 sequenceLength = seqObj != nullptr ? seqObj->getSequenceLength() : 0;

    // Sequence is shown only if sequence object is present and location is valid.
    QVector<U2Region> regions = data->location->regions;
    U2Region wholeSequenceRegion(0, sequenceLength);
    bool showSequence = seqObj != nullptr && rows <= maxRows && (!data->location->strand.isComplementary() || complTT != nullptr) &&
                        std::all_of(regions.begin(), regions.end(), [&wholeSequenceRegion](const U2Region& region) {
                            return region.length > 0 && wholeSequenceRegion.contains(region);
                        });
    if (showSequence) {
        QVector<U2Region> tooltipRegions = data->location->strand.isComplementary()
                                               ? U2Region::tailOf(regions, MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP)
                                               : U2Region::headOf(regions, MAX_QUALIFIER_VALUE_LENGTH_IN_TOOLTIP);
        auto effectiveComplTT = data->location->strand.isComplementary() ? complTT : nullptr;
        const U2EntityRef& sequenceRef = seqObj->getEntityRef();
        U2OpStatus2Log os;
        QByteArray seqVal = U2SequenceUtils::extractRegions(sequenceRef, tooltipRegions, effectiveComplTT, nullptr, data->isJoin(), os).join("^");
        QByteArray aminoVal = os.hasError() || aminoTT == nullptr
                                  ? ""
                                  : U2SequenceUtils::extractRegions(sequenceRef, tooltipRegions, effectiveComplTT, aminoTT, data->isJoin(), os).join("^");
        if (!os.hasError() && seqVal.length() > 0) {
            if (!tip.isEmpty()) {
                tip += "<br>";
            }

            bool isTruncated = tooltipRegions != regions;
            if (isTruncated) {
                seqVal += " ...";
            }
            tip += "<nobr><b>" + QObject::tr("Sequence") + "</b> = " + QString::fromLocal8Bit(seqVal).toHtmlEscaped() + "</nobr>";
            rows++;

            if (rows <= maxRows && !aminoVal.isEmpty()) {
                if (isTruncated) {
                    aminoVal += " ...";
                }
                tip += "<br>";
                tip += "<nobr><b>" + QObject::tr("Translation") + "</b> = " + QString::fromLocal8Bit(aminoVal).toHtmlEscaped() + "</nobr>";
            }
        }
    }
    return tip;
}

}  // namespace U2
