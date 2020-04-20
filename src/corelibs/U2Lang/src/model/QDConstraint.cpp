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

#include <U2Lang/BaseTypes.h>
#include "QDConstraint.h"

namespace U2 {

const QDConstraintType QDConstraintTypes::DISTANCE("QD_Distance_Constraint");

QDDistanceConstraint::QDDistanceConstraint(const QList<QDSchemeUnit*>& _units, QDDistanceType type, int min, int max)
: QDConstraint(_units, QDConstraintTypes::DISTANCE), distType(type) {
    assert(_units.size()==2);
    cfg = new QDParameters;

    Descriptor mind(QDConstraintController::MIN_LEN_ATTR, QObject::tr("Min distance"), QObject::tr("Minimum distance"));
    Descriptor maxd(QDConstraintController::MAX_LEN_ATTR, QObject::tr("Max distance"), QObject::tr("Maximum distance"));

    Attribute* minAttr = new Attribute(mind, BaseTypes::NUM_TYPE(), true, QVariant(min));
    Attribute* maxAttr = new Attribute(maxd, BaseTypes::NUM_TYPE(), true, QVariant(max));

    cfg->addParameter(minAttr->getId(), minAttr);
    cfg->addParameter(maxAttr->getId(), maxAttr);
}

QDDistanceConstraint::~QDDistanceConstraint() {
    delete cfg;
}

int QDDistanceConstraint::getMin() const {
    return cfg->getParameter(QDConstraintController::MIN_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

int QDDistanceConstraint::getMax() const {
    return cfg->getParameter(QDConstraintController::MAX_LEN_ATTR)->getAttributeValueWithoutScript<int>();
}

void QDDistanceConstraint::setMin(int min) {
    cfg->setParameter(QDConstraintController::MIN_LEN_ATTR, qVariantFromValue(min));
}

void QDDistanceConstraint::setMax(int max) {
    cfg->setParameter(QDConstraintController::MAX_LEN_ATTR, qVariantFromValue(max));
}

QString QDDistanceConstraint::getText(QDSchemeUnit*, QDSchemeUnit*) const {
    int minVal = getMin();
    int maxVal = getMax();
    if(minVal==maxVal) {
        return QString("%1 bp").arg(QString::number(minVal));
    }
    return QString("%1..%2 bp").arg(QString::number(minVal)).arg(QString::number(maxVal));
}

void QDDistanceConstraint::invert() {
    units.swap(0,1);
    setMin(-getMax());
    setMax(-getMin());
    distType = QDConstraintController::getInvertedType(distType);
}

//QDConstraintController
//////////////////////////////////////////////////////////////////////////
const QString QDConstraintController::DISTANCE_CONSTRAINT_EL("distance");
const QString QDConstraintController::TYPE_ATTR("type");
const QString QDConstraintController::MIN_LEN_ATTR("min");
const QString QDConstraintController::MAX_LEN_ATTR("max");
const QString QDConstraintController::SRC_ATTR("src");
const QString QDConstraintController::DST_ATTR("dst");

QDDistanceType QDConstraintController::getInvertedType(QDDistanceType type) {
    if (type==E2E) {
        return S2S;
    }
    if (type==S2S) {
        return E2E;
    }
    return type;
}

bool QDConstraintController::match(QDConstraint* c, const QDResultUnit& r1, const QDResultUnit& r2, bool complement) {
    QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(c);
    assert(dc);

    const U2Region& reg1 = r1->region;
    const U2Region& reg2 = r2->region;

    const QDDistanceType& dist = dc->distanceType();

    int min = dc->getMin();
    int max = dc->getMax();

    if(dc->getSource()==r1->owner) {
        if (complement) {
            return match(reg2, reg1, getInvertedType(dist), min, max);
        } else {
            return match(reg1, reg2, dist, min, max);
        }
    } else {
        assert(dc->getSource()==r2->owner);
        if (complement) {
            return match(reg1, reg2, getInvertedType(dist), min, max);
        } else {
            return match(reg2, reg1, dist, min, max);
        }
    }
}

bool QDConstraintController::match(const U2Region& srcReg,
                                   const U2Region& dstReg,
                                   const QDDistanceType& type,
                                   int min,
                                   int max)
{
    switch (type)
    {
    case E2S:
        {
            int srcEnds = srcReg.endPos();
            int dstStarts = dstReg.startPos;
            int distance = dstStarts - srcEnds;
            if(distance<=max && distance>=min) {
                return true;
            }
        }
        return false;
    case S2E:
        {
            int srcStarts = srcReg.startPos;
            int dstEnds = dstReg.endPos();
            int distance = dstEnds - srcStarts;
            if(distance<=max && distance>=min) {
                return true;
            }
        }
        return false;
    case S2S:
        {
            int srcStarts = srcReg.startPos;
            int dstStarts = dstReg.startPos;
            int distance = dstStarts - srcStarts;
            if(distance<=max && distance>=min) {
                return true;
            }
        }
        return false;
    case E2E:
        {
            int srcEnds = srcReg.endPos();
            int dstEnds = dstReg.endPos();
            int distance = dstEnds - srcEnds;
            if(distance<=max && distance>=min) {
                return true;
            }
        }
        return false;
    }
    return false;
}

U2Region QDConstraintController::matchLocation(QDDistanceConstraint* dc, const QDResultUnit& r, bool complement) {
    int start = 0;
    int end = 0;

    QDSchemeUnit* src = NULL;
    QDSchemeUnit* dst = NULL;
    QDDistanceType type = dc->distanceType();
    int minDist = dc->getMin();
    int maxDist = dc->getMax();

    if (complement) {
        src = dc->getDestination();
        dst = dc->getSource();
        type = getInvertedType(type);
    } else {
        src = dc->getSource();
        dst = dc->getDestination();
    }

    if (src==r->owner) {
        int len = dst->getActor()->getMaxResultLen();
        switch (type)
        {
        case E2S:
            start = r->region.endPos() + minDist;
            end = r->region.endPos() + maxDist + len;
            break;
        case E2E:
            end = r->region.endPos() + maxDist;
            start = r->region.endPos() + minDist - len;
            break;
        case S2S:
            start = r->region.startPos + minDist;
            end = r->region.startPos + maxDist + len;
            break;
        case S2E:
            end = r->region.startPos + maxDist;
            start = r->region.startPos + minDist - len;
            break;
        }
    } else {
        assert(dst==r->owner);
        int len = src->getActor()->getMaxResultLen();
        switch (type)
        {
        case E2S:
            start = r->region.startPos - maxDist - len;
            end = r->region.startPos - minDist;
            break;
        case E2E:
            end = r->region.endPos() - minDist;
            start = r->region.endPos() - maxDist - len;
            break;
        case S2S:
            start = r->region.startPos - maxDist;
            end = r->region.startPos - minDist + len;
            break;
        case S2E:
            start = r->region.endPos() - maxDist;
            end = r->region.endPos() - minDist + len;
            break;
        }
    }

    return U2Region(start, end-start);
}

}//namespace
