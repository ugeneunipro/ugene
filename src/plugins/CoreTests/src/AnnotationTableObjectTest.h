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

#ifndef _U2_ANNOTATION_TABLE_OBJECT_TESTS_H_
#define _U2_ANNOTATION_TABLE_OBJECT_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <QDomElement>
#include <U2Core/U2Region.h>
#include <U2Core/AnnotationTableObject.h>

namespace U2 {

class Document;

class GTestAnnotationDataItem: public QObject {
    Q_OBJECT
public:
    GTestAnnotationDataItem(const SharedAnnotationData& ad, QObject* p) : QObject(p), aData(ad){}
    const SharedAnnotationData& getAnnotation() const {return aData;}
private:
    SharedAnnotationData aData;
};

class GTest_CheckNumAnnotations : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckNumAnnotations, "check-num-annotations")

    ReportResult report();

private:
    QString        objContextName;
    int            num;
};

class GTest_FindAnnotationByNum : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindAnnotationByNum, "find-annotation-by-number")

    ReportResult report();

    Annotation * getAnnotation() const {return result;}

    void cleanup();

private:
    QString     objContextName;
    QString     annotationContextName;
    int         number;
    Annotation *result;
};

class GTest_FindAnnotationByName : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindAnnotationByName, "find-annotation-by-name")

    ReportResult report();

    Annotation * getAnnotation() const {return result;}

    void cleanup();

private:
    QString     objContextName;
    QString     annotationContextName;
    QString     aName;
    int         number;
    Annotation *result;
};

class GTest_FindAnnotationByLocation : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindAnnotationByLocation, "find-annotation-by-location")

    ReportResult report();

    Annotation * getAnnotation() const {return result;}

    void cleanup();

private:
    QString     objContextName;
    QString     annotationContextName;
    QString     annotationName;
    U2Region    location;
    U2Strand    strand;
    Annotation *result;
};

class GTest_CheckAnnotationLocation : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationLocation, "check-annotation-location")

    ReportResult report();

private:
    QString             annCtxName;
    QVector<U2Region>   location;
    U2Strand            strand;
};

class GTest_CheckAnnotationName : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationName, "check-annotation-name")

    ReportResult report();

private:
    QString         annCtxName;
    QString         aName;
};

class GTest_CheckAnnotationSequence : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationSequence, "check-annotation-sequence")

    ReportResult report();

private:
    QString         seqPart;
    QString         seqCtxName;
    QString         aCtxName;
};


class GTest_CheckAnnotationQualifier : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationQualifier, "check-annotation-qualifier")

    ReportResult report();

private:
    QString        annCtxName;
    QString        qName;
    QString        qValue;
};


class GTest_CheckAnnotationQualifierIsAbsent : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationQualifierIsAbsent, "check-no-annotation-qualifier")

    ReportResult report();

private:
    QString        annCtxName;
    QString        qName;
};


class GTest_CheckAnnotationsNumInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationsNumInTwoObjects, "compare-annotations-num-in-two-objects")

    ReportResult report();
    bool compareNumObjects;
    QString docContextName;
    QString secondDocContextName;
};

class GTest_CheckAnnotationsLocationsInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationsLocationsInTwoObjects, "compare-annotations-locations-in-two-objects")

    ReportResult report();
    bool compareNumObjects;
    QString docContextName;
    QString secondDocContextName;
};

// compare annotations that may be in different order in two docs
class GTest_CheckAnnotationsLocationsAndNumReorderdered : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationsLocationsAndNumReorderdered, "compare-annotations-locations-and-num-reordered")

    ReportResult report();
    QString doc1CtxName;
    QString doc2CtxName;
};

class GTest_CheckAnnotationsQualifiersInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationsQualifiersInTwoObjects, "compare-annotations-qualifiers-in-two-objects")

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_CheckAnnotationsNamesInTwoObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckAnnotationsNamesInTwoObjects, "compare-annotations-names-in-two-objects")

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class GTest_CreateTmpAnnotationObject : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CreateTmpAnnotationObject, "create-tmp-annotation-object")

    ReportResult report();
    void cleanup();
    QString objContextName;
    AnnotationTableObject* aobj;
};

class AnnotationTableObjectTest {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//namespace
#endif
