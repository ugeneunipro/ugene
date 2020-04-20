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

#include "UGUITestBase.h"

namespace U2 {

const QString UGUITestBase::unnamedTestsPrefix = "test";

UGUITestBase::~UGUITestBase() {

    qDeleteAll(tests);
    qDeleteAll(preAdditional);
    qDeleteAll(postAdditionalActions);
    qDeleteAll(postAdditionalChecks);
}

bool UGUITestBase::registerTest(HI::GUITest *test, TestType testType) {

    Q_ASSERT(test);

    test->setName(nameUnnamedTest(test, testType));

    if (isNewTest(test, testType)) {
        addTest(test, testType);
        return true;
    }

    return false;
}

QString UGUITestBase::nameUnnamedTest(HI::GUITest* test, TestType testType) {

    Q_ASSERT(test);
    if (!test) {
        return "";
    }

    QString testName = test->getName();
    if (testName.isEmpty()) {
        testName = getNextTestName(testType);
    }

    return testName;
}

bool UGUITestBase::isNewTest(HI::GUITest *test, TestType testType) {

    return test && !findTest(test->getFullName(), testType);
}

void UGUITestBase::addTest(HI::GUITest *test, TestType testType) {

    if (test) {
        getMap(testType).insert(test->getFullName(), test);
    }
}

QString UGUITestBase::getNextTestName(TestType testType) {

    int testsCount = getMap(testType).size();
    return unnamedTestsPrefix + QString::number(testsCount);
}

HI::GUITest *UGUITestBase::findTest(const QString &name, TestType testType) {
    GUITestMap map = getMap(testType);
    return map.value(name);
}

HI::GUITest *UGUITestBase::getTest(const QString &suite, const QString &name, TestType testType) {

    return getMap(testType).value(suite + ":" + name);
}

HI::GUITest *UGUITestBase::takeTest(const QString &suite, const QString &name, TestType testType) {

    return getMap(testType).take(suite + ":" + name);
}

GUITestMap& UGUITestBase::getMap(TestType testType) {

    switch(testType) {
    case PreAdditional:
        return preAdditional;
    case PostAdditionalChecks:
        return postAdditionalChecks;
    case PostAdditionalActions:
        return postAdditionalActions;

    default:
    case Normal: return tests;
    }
}

GUITests UGUITestBase::getTests(TestType testType, QString label) {

    GUITests testList = getMap(testType).values();
    foreach (GUITest* t, testList) {
        if(t->getLabel() != label){
            testList.takeAt(testList.indexOf(t));
        }
    }
    return testList;
}

GUITests UGUITestBase::takeTests(TestType testType) {

    GUITests testList = getMap(testType).values();
    getMap(testType).clear();

    return testList;
}


} // namespace
