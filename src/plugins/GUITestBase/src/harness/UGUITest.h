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

#ifndef _U2_UGUI_TEST_H_
#define _U2_UGUI_TEST_H_

#include <GTGlobals.h>
#include <core/GUITest.h>
#include <core/GUITestOpStatus.h>

#include <QTimer>

#include <U2Core/global.h>

namespace U2 {
using namespace HI;

/** Default timeout for all GUI tests. */
#define DEFAULT_GUI_TEST_TIMEOUT 240000

/**
 * A namespace for known GUI test labels and utility methods.
 */
class UGUITestLabels {
public:
    /**
     * A label to mark test as included into the default nightly GUI test suite.
     * Only tests with this labels are executed as a part of nightly GUI testing.
     */
    static const QString Nightly;

    /** A test with this label is safe to run on Linux. */
    static const QString Linux;

    /** A test with this label is safe to run on MacOS. */
    static const QString MacOS;

    /** A test with this label is safe to run in Windows. */
    static const QString Windows;

    /**
     * A test with this label is included into the list of tests, but is not run and reported as ignored.
     * Any Ignored or IgnoredOn<Platform> label is considered as a TODO and must have a bug number in the test description.
     */
    static const QString Ignored;

    /**
     * A test with this label is included into the list of tests, but is not run on Linux and reported as ignored.
     * Any IgnoredOnLinux label is considered as a TODO and must have a bug number in the test description.
     */
    static const QString IgnoredOnLinux;

    /**
     * A test with this label is included into the list of tests, but is not run on MacOS and reported as ignored.
     * Any IgnoredOnMacOS label is considered as a TODO and must have a bug number in the test description.
     */
    static const QString IgnoredOnMacOS;

    /**
     * A test with this label is included into the list of tests, but is not run on Windows and reported as ignored.
     * Any IgnoredOnWindows label is considered as a TODO and must have a bug number in the test description.
     */
    static const QString IgnoredOnWindows;

    /** Returns true if the test has Ignored or IgnoredOn<CurrentOS>. */
    static bool hasIgnoredLabel(const GUITest* test);

    /** Returns true if the test has current platform label: Linux, MacOS or Windows. */
    static bool hasPlatformLabel(const GUITest* test);
};

/** GUI test with quick access to UGENE specific runtime variables: testDir, dataDir ... */
class UGUITest : public GUITest {
    Q_OBJECT
public:
    UGUITest(const QString& name, const QString& suite, int timeout, const QSet<QString>& labelSet)
        : GUITest(name, suite, timeout, labelSet) {
    }

    /** Returns full test name as known by Teamcity. */
    static QString getTeamcityTestName(const QString& suite, const QString& name) {
        return suite + "_" + name;
    }

    static const QString testDir;
    static const QString dataDir;
    static const QString screenshotDir;
    static const QString sandBoxDir;
};

#define TESTNAME(className) #className
#define SUITENAME(className) QString(GUI_TEST_SUITE)

#define GUI_TEST_CLASS_DECLARATION(className) \
    class className : public UGUITest { \
    public: \
        className(int timeout = DEFAULT_GUI_TEST_TIMEOUT, const QStringList &labelList = QStringList()) \
            : UGUITest(TESTNAME(className), SUITENAME(className), timeout, labelList.toSet()) { \
        } \
\
    protected: \
        void run(HI::GUITestOpStatus& os) override; \
    };

#define GUI_TEST_CLASS_DEFINITION(className) \
    void className::run(HI::GUITestOpStatus& os)

}  // namespace U2

#endif
