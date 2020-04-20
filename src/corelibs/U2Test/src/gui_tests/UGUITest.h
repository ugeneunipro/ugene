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

#include <QTimer>

#include <GTGlobals.h>
#include <core/GUITest.h>
#include <core/GUITestOpStatus.h>

#include <U2Core/global.h>

namespace U2 {
using namespace HI;

class U2TEST_EXPORT UGUITest : public GUITest {
    Q_OBJECT
public:
    UGUITest(const QString &_name = "", const QString &_suite = "", int timeout = 240000) : GUITest(_name, _suite, timeout) {}
    virtual ~UGUITest(){}

    static const QString testDir;
    static const QString dataDir;
    static const QString screenshotDir;
    static const QString sandBoxDir;
};

typedef QList<UGUITest*> UGUITests;

#define TESTNAME(className) #className
#define SUITENAME(className) QString(GUI_TEST_SUITE)

#define GUI_TEST_CLASS_DECLARATION(className) \
    class className : public UGUITest { \
    public: \
        className () : UGUITest(TESTNAME(className), SUITENAME(className)){} \
    protected: \
        virtual void run(HI::GUITestOpStatus &os); \
    };

#define GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(className, timeout) \
    class className : public UGUITest { \
    public: \
        className () : UGUITest(TESTNAME(className), SUITENAME(className), timeout){} \
    protected: \
        virtual void run(HI::GUITestOpStatus &os); \
    };

#define GUI_TEST_CLASS_DEFINITION(className) \
    void className::run(HI::GUITestOpStatus &os)

}

#endif
