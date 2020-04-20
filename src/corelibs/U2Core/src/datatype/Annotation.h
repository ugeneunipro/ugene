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

#ifndef _U2_ANNOTATION_H_
#define _U2_ANNOTATION_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2Qualifier.h>
#include <U2Core/U2FeatureType.h>

namespace U2 {

class AnnotationGroup;
class AnnotationModification;
class DNATranslation;
class AnnotationTableObject;
class U2SequenceObject;

class U2CORE_EXPORT Annotation : public U2Entity {
public:
                                    Annotation(const U2DataId &featureId, const SharedAnnotationData &data,
                                        AnnotationGroup *parentGroup, AnnotationTableObject *parentObject);

    AnnotationTableObject *         getGObject() const;

    const SharedAnnotationData &    getData() const;

    QString                         getName() const;

    U2FeatureType                   getType() const;

    void                            setName(const QString &name);

    void                            setType(U2FeatureType type);

    bool                            isOrder() const;

    bool                            isJoin() const;

    bool                            isBond() const;

    U2Strand                        getStrand() const;

    void                            setStrand(const U2Strand &strand);

    U2LocationOperator              getLocationOperator() const;

    void                            setLocationOperator(U2LocationOperator op);

    U2Location                      getLocation() const;

    void                            setLocation(const U2Location &location);

    QVector<U2Region>               getRegions() const;

    qint64                          getRegionsLen() const;

    /**
     * Replaces existing annotation region(s) with supplied @regions
     */
    void                            updateRegions(const QVector<U2Region> &regions);

    void                            addLocationRegion(const U2Region &reg);

    QVector<U2Qualifier>            getQualifiers() const;

    void                            addQualifier(const U2Qualifier &q);

    void                            removeQualifier(const U2Qualifier &q);

    AnnotationGroup *               getGroup() const;

    void                            setGroup(AnnotationGroup *newGroup);
    /**
     * Appends to @res all existing qualifiers with the given @name
     */
    void                            findQualifiers(const QString &name, QList<U2Qualifier> &res) const;
    /**
     * Returns the value of an arbitrary qualifier with the given @name.
     * Null string is returned if no qualifier was found.
     */
    QString                         findFirstQualifierValue(const QString &name) const;

    static QString                  getQualifiersTip(const SharedAnnotationData &data, int maxRows, U2SequenceObject *seqObj = NULL,
                                        DNATranslation *complTT = NULL, DNATranslation *aminoTT = NULL);

    bool                            isCaseAnnotation() const;

    void                            setCaseAnnotation(bool caseAnnotation);

    static bool                     annotationLessThan(Annotation *first, Annotation *second);

    static bool                     annotationLessThanByRegion(Annotation *first, Annotation *second);

    static bool                     isValidAnnotationName(const QString &n);

    static bool                     isValidQualifierName(const QString &name);

    static bool                     isValidQualifierValue(const QString &value);

    static QString                  produceValidAnnotationName(const QString &name);

private:
    AnnotationTableObject *         parentObject;

    SharedAnnotationData            data;
    AnnotationGroup *               group;
};

} // namespace U2

#endif // _U2_ANNOTATION_H_
