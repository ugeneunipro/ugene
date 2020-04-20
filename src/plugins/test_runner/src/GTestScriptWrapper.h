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

#ifndef _U2_TEST_SCRIPT_WRAPPER_H_
#define _U2_TEST_SCRIPT_WRAPPER_H_

#include <QObject>
#include <QScriptValue>
#include <QTest>

class QScriptContext;

namespace U2 {

class GTestScriptWrapper {
public:
    GTestScriptWrapper();

    static QScriptValue qtestMouseClickE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestMouseDClickE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestMouseMoveE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestMousePressE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestMouseReleaseE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestKeyClickE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestKeyClicksE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestKeyEventE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestKeyPressE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestKeyReleaseE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestSleepE(QScriptContext *ctx, QScriptEngine *eng);
    static QScriptValue qtestWaitE(QScriptContext *ctx, QScriptEngine *eng);

    static void setQTest(QScriptEngine* curEng);
};
}//namespace

#endif
