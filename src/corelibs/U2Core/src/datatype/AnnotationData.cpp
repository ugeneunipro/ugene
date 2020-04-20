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

#include "AnnotationData.h"

namespace U2 {

AnnotationData::AnnotationData()
    : location(new U2LocationData()), caseAnnotation(false), type(U2FeatureTypes::MiscFeature) {

}

bool AnnotationData::isJoin() const {
    return location->isJoin();
}

bool AnnotationData::isOrder() const {
    return location->isOrder();
}

bool AnnotationData::isBond() const {
    return location->isBond();
}

U2Strand AnnotationData::getStrand() const {
    return location->strand;
}

void AnnotationData::setStrand(U2Strand s) {
    location->strand = s;
}

U2LocationOperator AnnotationData::getLocationOperator() const {
    return location->op;
}

void AnnotationData::setLocationOperator(U2LocationOperator o) {
    location->op = o;
}

const QVector<U2Region> & AnnotationData::getRegions() const {
    return location->regions;
}

AnnotationData & AnnotationData::operator =(const AnnotationData &a) {
    type = a.type;
    name = a.name;
    location = a.location;
    qualifiers = a.qualifiers;
    caseAnnotation = a.caseAnnotation;
    return *this;
}

bool AnnotationData::operator== (const AnnotationData &other) const {
    if (this->type != other.type) {
        return false;
    }

    if (this->name != other.name) {
        return false;
    }

    if (this->qualifiers.size() != other.qualifiers.size()) {
        return false;
    }
    foreach(const U2Qualifier &q, this->qualifiers) {
        if (!other.qualifiers.contains(q)) {
            return false;
        }
    }

    if (this->location != other.location) {
        return false;
    }

    return true;
}

bool AnnotationData::operator !=(const AnnotationData &other) const {
    return !(*this == other);
}

void AnnotationData::removeAllQualifiers(const QString &name, QStringList &values) {
    for (int i = qualifiers.size(); --i >= 0;) {
        const U2Qualifier &q = qualifiers.at(i);
        if (q.name == name) {
            values.append(q.value);
            qualifiers.remove(i);
        }
    }
}

void AnnotationData::findQualifiers(const QString &name, QVector<U2Qualifier> &res) const {
    foreach(const U2Qualifier &q, qualifiers) {
        if (q.name == name) {
            res.append(q);
        }
    }
}

QString AnnotationData::findFirstQualifierValue(const QString &name) const {
    foreach(const U2Qualifier &q, qualifiers) {
        if (q.name == name) {
            return q.value;
        }
    }
    return QString::null;
}

bool AnnotationData::operator <(const AnnotationData &other) const {
    if (getRegions().isEmpty()) {
        return true;
    }
    if (other.getRegions().isEmpty()) {
        return false;
    }
    return getRegions().first() < other.getRegions().first();
}

QDataStream & operator>>(QDataStream &dataStream, TriState &state) {
    int st;
    dataStream >> st;
    switch (st) {
    case 0: state = TriState_Yes; break;
    case 1: state = TriState_No; break;
    default: state = TriState_Unknown;
    }
    return dataStream;
}

QDataStream & operator<<(QDataStream &dataStream, const TriState &state) {
    switch (state) {
    case TriState_Yes: return dataStream << 0;
    case TriState_No: return dataStream << 1;
    default: return dataStream << 2;
    }
}

QDataStream & operator>>(QDataStream &dataStream, U2Qualifier &q) {
    return dataStream >> q.name >> q.value;
}

QDataStream & operator<<(QDataStream &dataStream, const U2Qualifier &q) {
    return dataStream << q.name << q.value;
}

QDataStream & operator>>(QDataStream &dataStream, AnnotationData &) {
    assert(0);
    return dataStream;
    //    return dataStream >> data.name >> data.location >> data.qualifiers;
}

QDataStream & operator<<(QDataStream &dataStream, const AnnotationData &) {
    assert(0);
    return dataStream;
    //    return dataStream << data.name << data.location << data.qualifiers;
}

} //namespace U2
