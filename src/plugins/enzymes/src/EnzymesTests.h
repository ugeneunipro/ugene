/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QDomElement>
#include <QMultiMap>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObject.h>
#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include "DNAFragment.h"

namespace U2 {

class LoadEnzymeFileTask;
class U2SequenceObject;

// cppcheck-suppress noConstructor
class GTest_FindEnzymes : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindEnzymes, "find-enzymes");

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;
    void cleanup() override;

private:
    bool hasUnexpectedError() const;

    int minHits;
    int maxHits;
    QString enzymesUrl;
    QString seqObjCtx;
    QString aObjName;
    U2SequenceObject* seqObj;
    QVector<U2Region> excludedRegions;
    QStringList enzymeNames;
    // Text the task should fail with
    QString expectedError;
    AnnotationTableObject* aObj;
    LoadEnzymeFileTask* loadTask;
    bool contextIsAdded;
    // Annotation data representation
    struct AnnData {
        // Annotation region
        U2Region reg;
        // Annotation qualifier's name
        QString qualName;
        // The corresponding annotation's value
        QString qualValue;
    };
    QMap<QString, QList<AnnData>> resultsPerEnzyme;
    // if this parameter is true, the test will pass
    // only if the annotations number from the XML file
    // will be the same that number of real found annotations
    bool exactAnnotationsNumber = false;
};

// cppcheck-suppress noConstructor
class GTest_DigestIntoFragments : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DigestIntoFragments, "digest-into-fragments");

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    QString seqObjCtx;
    QString aObjCtx;
    QString enzymesUrl;
    QStringList enzymeNames;
    bool searchForEnzymes;
    bool isCircular;
    AnnotationTableObject* aObj;
    U2SequenceObject* seqObj;
    LoadEnzymeFileTask* loadTask;
};

class LigateFragmentsTask;

// cppcheck-suppress noConstructor
class GTest_LigateFragments : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_LigateFragments, "ligate-fragments");

    void cleanup() override;
    void prepare() override;
    ReportResult report() override;
    void prepareFragmentsList();

private:
    QStringList seqObjNames;
    QStringList annObjNames;
    QStringList fragmentNames;
    QString resultDocName;
    QList<GObject*> sObjs, aObjs;
    QList<DNAFragment> targetFragments;
    bool makeCircular;
    bool checkOverhangs;
    bool contextAdded;
    LigateFragmentsTask* ligateTask;
};

class EnzymeTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
