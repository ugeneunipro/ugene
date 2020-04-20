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

#ifndef _U2_GUI_TEST_BASE_H_
#define _U2_GUI_TEST_BASE_H_

#include <U2Core/global.h>
#include <U2Core/U2IdTypes.h>
#include <U2Core/Task.h>
#include <U2Core/MultiTask.h>
#include <U2Core/GObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Gui/MainWindow.h>
#include <U2View/ADVSingleSequenceWidget.h>

#include <U2Test/UGUITest.h>

namespace U2 {

typedef QMap<QString, HI::GUITest*> GUITestMap;
typedef QList<HI::GUITest*> GUITests;

class U2TEST_EXPORT UGUITestBase {
public:
    enum TestType {Normal, PreAdditional, PostAdditionalChecks, PostAdditionalActions} type;

    virtual ~UGUITestBase();

    bool registerTest(HI::GUITest *test, TestType testType = Normal);
    HI::GUITest *getTest(const QString &suite, const QString &name, TestType testType = Normal);
    HI::GUITest *takeTest(const QString &suite, const QString &name, TestType testType = Normal); // removes item from UGUITestBase

    GUITests getTests(TestType testType = Normal, QString label = "");
    GUITests takeTests(TestType testType = Normal); // removes items from UGUITestBase

    GUITests getTestsWithoutRemoving(TestType testType = Normal);

    HI::GUITest *findTest(const QString &name, TestType testType = Normal);

    static const QString unnamedTestsPrefix;

private:
    GUITestMap tests;
    GUITestMap preAdditional;
    GUITestMap postAdditionalChecks;
    GUITestMap postAdditionalActions;
     // GUI checks additional to the launched checks

    GUITestMap &getMap(TestType testType);

    QString getNextTestName(TestType testType);

    bool isNewTest(HI::GUITest *test, TestType testType);
    void addTest(HI::GUITest *test, TestType testType);

    QString nameUnnamedTest(HI::GUITest* test, TestType testType);
};

}

#endif
