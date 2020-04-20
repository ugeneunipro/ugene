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

#ifndef _GB2_QD_TESTS_H_
#define _GB2_QD_TESTS_H_


#include <U2Test/XMLTestUtils.h>

#include <QDomElement>

namespace U2 {

class AnnotationGroup;
class AnnotationTableObject;
class U2SequenceObject;
class QDScheduler;
class QDScheme;

//cppcheck-suppress noConstructor
class GTest_QDSchedulerTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_QDSchedulerTest, "qd_search", TaskFlags_NR_FOSCOE);
    ~GTest_QDSchedulerTest();

    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
private:
    QString seqName;
    QString schemaUri;
    QString expectedResName;
    U2SequenceObject* seqObj;
    AnnotationTableObject* expectedResult;
    AnnotationTableObject* result;
    QDScheme* schema;
    QDScheduler* sched;
};

class QDTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

class CompareAnnotationGroupsTask : public Task {
    Q_OBJECT
public:
    CompareAnnotationGroupsTask(const QList<AnnotationGroup *> &_grp1, const QList<AnnotationGroup *> &_grp2)
        : Task(tr("Compare annotation tables task"), TaskFlag_None), grps1(_grp1), grps2(_grp2), equal(false)
    {

    }

    virtual void run();
    bool areEqual() const { assert(isFinished()); return equal; }

private:
    QList<AnnotationGroup *> grps1, grps2;
    bool equal;
};

}//namespace

#endif
