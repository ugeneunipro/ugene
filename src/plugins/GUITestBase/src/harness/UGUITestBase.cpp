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

#include "UGUITestBase.h"

#include <U2Core/TextUtils.h>

namespace U2 {

UGUITestBase* UGUITestBase::getInstance() {
    static UGUITestBase* instance = new UGUITestBase();
    return instance;
}

bool UGUITestBase::registerTest(GUITest* test, TestType testType) {
    Q_ASSERT(test);
    QString fullTestName = test->getFullName();
    if (getTest(fullTestName, testType) != nullptr) {
        return false;
    }
    getMap(testType).insert(fullTestName, test);
    return true;
}

GUITest* UGUITestBase::getTest(const QString& name, TestType testType) const {
    return getConstMap(testType).value(name);
}

GUITest* UGUITestBase::getTest(const QString& suite, const QString& name, TestType testType) const {
    return getTest(HI::GUITest::getFullTestName(suite, name), testType);
}

const QMap<QString, GUITest*>& UGUITestBase::getConstMap(TestType testType) const {
    switch (testType) {
        case PreAdditional:
            return preAdditional;
        case PostAdditionalChecks:
            return postAdditionalChecks;
        case PostAdditionalActions:
            return postAdditionalActions;
        case Normal:
        default:
            return tests;
    }
}

QMap<QString, GUITest*>& UGUITestBase::getMap(TestType testType) {
    switch (testType) {
        case PreAdditional:
            return preAdditional;
        case PostAdditionalChecks:
            return postAdditionalChecks;
        case PostAdditionalActions:
            return postAdditionalActions;
        case Normal:
        default:
            return tests;
    }
}

/**
 * Returns true if set1 shares common elements with set2.
 * Note: We can't use QSet::intersects today because it is not available in QT5.4 (was added in QT5.6)
 */
static bool intersects(const QSet<QString>& set1, const QSet<QString>& set2) {
    for (const QString& value1 : qAsConst(set1)) {
        if (set2.contains(value1)) {
            return true;
        }
    }
    return false;
}

QList<GUITest*> UGUITestBase::getTests(TestType testType, const QStringList& labelList) const {
    QList<GUITest*> allTestList = getConstMap(testType).values();
    if (labelList.isEmpty()) {
        return allTestList;
    }
    QList<GUITest*> filteredTestList;
    QSet<QString> includeLabelSet;
    QSet<QString> excludeLabelSet;
    for (const QString& label : qAsConst(labelList)) {
        if (label.startsWith("-")) {
            excludeLabelSet << label;
        } else {
            includeLabelSet << label;
        }
    }
    for (GUITest* test : qAsConst(allTestList)) {
        if (test->labelSet.contains(includeLabelSet) && !intersects(test->labelSet, excludeLabelSet)) {
            filteredTestList << test;
        }
    }
    return filteredTestList;
}

}  // namespace U2
