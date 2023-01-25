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

#ifndef _U2_GUI_TEST_BASE_H_
#define _U2_GUI_TEST_BASE_H_

#include <harness/UGUITest.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>
#include <U2Core/U2IdTypes.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

#include <U2View/ADVSingleSequenceWidget.h>

namespace U2 {

class UGUITestBase {
public:
    /** Type of the test: some test types has special handling in runtime. */
    enum TestType {
        /** A standard user test. */
        Normal,

        /** A pre-check. Runs before the Normal test. */
        PreAdditional,

        /** A post-check. Runs after the Normal test. Validates UGENE state after the Normal test is finished. */
        PostAdditionalChecks,

        /** A post-action. Runs after the Normal & PostCheck tests. Used to perform additional cleanup with no checks. */
        PostAdditionalActions
    } type;

    /**
     * Registers test in the test base. Returns true if the test was successfully registered
     * or false if another test is already registered with the given name.
     * */
    bool registerTest(GUITest* test, TestType testType = Normal);

    /** Finds a registered test by the full test name and type. Returns nullptr if no registered test was found. */
    GUITest* getTest(const QString& name, TestType testType = Normal) const;

    /** Finds a registered test by the full test name and type. Returns nullptr if no registered test was found. */
    GUITest* getTest(const QString& suite, const QString& name, TestType testType = Normal) const;

    /**
     * Returns list of registered tests of the given type that have all labels from the list.
     * If label list empty, returns all tests of the given type.
     * If any label in the list starts with '-' (minus) sign the method will performs exclusive
     * filtering: tests with such labels will be excluded from the result.
     * Example: "Nightly,-Ignored" will return all tests that have "Nightly" label but have no "Ignored" label.
     */
    QList<GUITest*> getTests(TestType testType = Normal, const QStringList& labelList = QStringList()) const;

    /** Returns UGUITestBase instance. There is only 1 instance of the database per application. */
    static UGUITestBase* getInstance();

private:
    /** Normal tests. */
    QMap<QString, GUITest*> tests;

    /** PreAdditional tests. */
    QMap<QString, GUITest*> preAdditional;

    /** PostAdditionalChecks tests. */
    QMap<QString, GUITest*> postAdditionalChecks;

    /** PostAdditionalActions tests. */
    QMap<QString, GUITest*> postAdditionalActions;

    /** Returns tests map of the given type. */
    QMap<QString, GUITest*>& getMap(TestType testType);

    const QMap<QString, GUITest*>& getConstMap(TestType testType) const;
};

}  // namespace U2

#endif
