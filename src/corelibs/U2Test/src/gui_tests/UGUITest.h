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
class U2TEST_EXPORT UGUITestLabels {
public:
    /**
     * A label to mark test as included into the default nightly GUI test suite.
     * Only tests with this labels are executed as a part of nightly GUI testing.
     */
    static const QString Nightly;

    /** A label to mark test as included into the 'Metagenomics' teamcity build suite. */
    static const QString Metagenomics;
};

/** GUI test with quick access to UGENE specific runtime variables: testDir, dataDir ... */
class U2TEST_EXPORT UGUITest : public GUITest {
    Q_OBJECT
public:
    UGUITest(const QString &name, const QString &suite, int timeout, const QStringList &labels)
        : GUITest(name, suite, timeout, labels) {
    }

    /** Returns full test name as known by Teamcity. */
    static QString getTeamcityTestName(const QString &suite, const QString &name) {
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
        className(int timeout = DEFAULT_GUI_TEST_TIMEOUT, const QStringList &labels = QStringList()) \
            : UGUITest(TESTNAME(className), SUITENAME(className), timeout, labels) { \
        } \
\
    protected: \
        virtual void run(HI::GUITestOpStatus &os); \
    };

#define GUI_TEST_CLASS_DEFINITION(className) \
    void className::run(HI::GUITestOpStatus &os)

}    // namespace U2

#endif
