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

#ifndef _U2_ANNOTATION_DATA_H_
#define _U2_ANNOTATION_DATA_H_

#include <U2Core/U2FeatureType.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2Qualifier.h>

#include <QDataStream>
#include <QSharedData>
#include <QVector>
#include <QStringList>

namespace U2 {

class U2CORE_EXPORT AnnotationData : public QSharedData {
public:
    AnnotationData();

    AnnotationData & operator =(const AnnotationData &a);

    bool operator ==(const AnnotationData &other) const;
    bool operator !=(const AnnotationData &other) const;
    bool operator <(const AnnotationData &other) const;

    void findQualifiers(const QString &name, QVector<U2Qualifier> &res) const;
    QString findFirstQualifierValue(const QString &name) const;
    void removeAllQualifiers(const QString &name, QStringList &values);

    bool isJoin() const;
    bool isOrder() const;
    bool isBond() const;

    U2Strand getStrand() const;
    void setStrand(U2Strand s);

    U2LocationOperator getLocationOperator() const;
    void setLocationOperator(U2LocationOperator o);

    const QVector<U2Region> & getRegions() const;

    QString                 name;
    U2Location              location;
    QVector<U2Qualifier>    qualifiers;
    bool                    caseAnnotation;
    U2FeatureType           type;
};

typedef QSharedDataPointer<AnnotationData> SharedAnnotationData;

U2CORE_EXPORT QDataStream & operator>>(QDataStream &dataStream, TriState &state);
U2CORE_EXPORT QDataStream & operator<<(QDataStream &dataStream, const TriState &state);
U2CORE_EXPORT QDataStream & operator>>(QDataStream &dataStream, U2Qualifier &q);
U2CORE_EXPORT QDataStream & operator<<(QDataStream &dataStream, const U2Qualifier &q);
U2CORE_EXPORT QDataStream & operator>>(QDataStream &dataStream, AnnotationData &data);
U2CORE_EXPORT QDataStream & operator<<(QDataStream &dataStream, const AnnotationData &data);

} // namespace U2

Q_DECLARE_METATYPE(U2::AnnotationData)
Q_DECLARE_METATYPE(U2::SharedAnnotationData)
Q_DECLARE_METATYPE(QList<U2::SharedAnnotationData>)

#endif
