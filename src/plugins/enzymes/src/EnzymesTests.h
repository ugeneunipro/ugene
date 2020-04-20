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

#ifndef _U2_ENZYMES_TESTS_H_
#define _U2_ENZYMES_TESTS_H_

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObject.h>
#include <U2Core/U2Region.h>
#include <U2Test/XMLTestUtils.h>

#include <QMultiMap>
#include <QDomElement>

#include "DNAFragment.h"

namespace U2 {

class LoadEnzymeFileTask;
class U2SequenceObject;

//cppcheck-suppress noConstructor
class GTest_FindEnzymes : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindEnzymes, "find-enzymes");

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    ReportResult report();
    void cleanup();

private:
    int minHits;
    int maxHits;
    QString                 enzymesUrl;
    QString                 seqObjCtx;
    QString                 aObjName;
    U2SequenceObject*       seqObj;
    QVector<U2Region>       excludedRegions;
    QStringList             enzymeNames;
    AnnotationTableObject*    aObj;
    LoadEnzymeFileTask*     loadTask;
    bool                    contextIsAdded;
    QMultiMap<QString, U2Region> resultsPerEnzyme;
};

//cppcheck-suppress noConstructor
class GTest_DigestIntoFragments : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DigestIntoFragments, "digest-into-fragments");

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    QString                 seqObjCtx;
    QString                 aObjCtx;
    QString                 enzymesUrl;
    QStringList             enzymeNames;
    bool                    searchForEnzymes;
    bool                    isCircular;
    AnnotationTableObject*  aObj;
    U2SequenceObject*       seqObj;
    LoadEnzymeFileTask*     loadTask;
};

class LigateFragmentsTask;

//cppcheck-suppress noConstructor
class GTest_LigateFragments : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_LigateFragments, "ligate-fragments");

    void cleanup();
    void prepare();
    ReportResult report();
    void prepareFragmentsList();

private:
    QStringList             seqObjNames;
    QStringList             annObjNames;
    QStringList             fragmentNames;
    QString                 resultDocName;
    QList<GObject*>         sObjs, aObjs;
    QList<DNAFragment>      targetFragments;
    bool                    makeCircular;
    bool                    checkOverhangs;
    bool                    contextAdded;
    LigateFragmentsTask*    ligateTask;
};


class EnzymeTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} //namespace

#endif
