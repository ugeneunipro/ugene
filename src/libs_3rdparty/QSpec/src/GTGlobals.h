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

#ifndef _HI_GT_GLOBALS_H_
#define _HI_GT_GLOBALS_H_

#include <core/GUITestOpStatus.h>
#include <core/global.h>

#include <QAction>
#include <QMessageLogger>
#include <QTime>

/**
 * Default wait time for any UI operation to complete.
 * Example of UI operation to wait for:
 *  - wait for a popup to show
 *  - wait for an element to be present.
 */
#define GT_OP_WAIT_MILLIS 30000

/**
 * Default check time to check that WAIT op is completed.
 * The completion of UI op is checked in the loop every GT_OP_CHECK_MILLIS.
 */
#define GT_OP_CHECK_MILLIS 100

namespace HI {

/** Common utils and data types for GT tests. */
class HI_EXPORT GTGlobals {
public:
    enum UseMethod {
        /** Perform action with a mouse. */
        UseMouse = 1,
        /** Use a special key. For example Up/Down for SpinBox to increase or decrease a value. */
        UseKey = 2,
        /** Input a value using keyboard. */
        UseKeyBoard = 3,
    };
    enum WindowAction { Minimize,
                        Maximize,
                        Close,
                        WindowActionCount };

    // if failIfNull is set to true, fails if object wasn't found
    class HI_EXPORT FindOptions {
    public:
        FindOptions(bool failIfNotFound = true, Qt::MatchFlags matchPolicy = Qt::MatchExactly, int depth = INFINITE_DEPTH);

        bool failIfNotFound;
        Qt::MatchFlags matchPolicy;
        int depth;

        static const int INFINITE_DEPTH = 0;
    };

    static void sleep(int msec = 2000);
    static void sendEvent(QObject* obj, QEvent* e);

    /** Takes a screenshot and saves to file. */
    static void takeScreenShot(HI::GUITestOpStatus& os, const QString& path);

    /** Takes a screenshot and returns an image. */
    static QImage takeScreenShot(HI::GUITestOpStatus& os);

    static void GUITestFail();
};

#define GT_DEBUG_MESSAGE(condition, errorMessage, result) \
    { \
        QByteArray _cond = QString(#condition).toLocal8Bit(); \
        QByteArray _time = QTime::currentTime().toString().toLocal8Bit(); \
        QByteArray _error = QString(errorMessage).toLocal8Bit(); \
        if (condition) { \
            qDebug("[%s] GT_OK: (%s) for '%s'", _time.constData(), _cond.constData(), _error.constData()); \
        } else { \
            qWarning("[%s] GT_FAIL: (%s) for '%s'", _time.constData(), _cond.constData(), _error.constData()); \
        } \
    }

/** Used in tests */
#define CHECK_SET_ERR(condition, errorMessage) \
    CHECK_SET_ERR_RESULT(condition, errorMessage, )

#define CHECK_OP_SET_ERR(os, errorMessage) \
    CHECK_SET_ERR(!os.isCoR(), errorMessage)

#define CHECK_SET_ERR_RESULT(condition, errorMessage, result) \
    { \
        GT_DEBUG_MESSAGE(condition, errorMessage, result); \
        if (os.hasError()) { \
            HI::GTGlobals::GUITestFail(); \
            return result; \
        } \
        if (!(condition)) { \
            os.setError(errorMessage); \
            HI::GTGlobals::GUITestFail(); \
            return result; \
        } \
    }

/** Unconditionally marks active test as failed. Prints 'errorMessage' into the log. */
#define GT_FAIL(errorMessage, result) \
    GT_DEBUG_MESSAGE(false, errorMessage, result); \
    HI::GTGlobals::GUITestFail(); \
    os.setError(errorMessage); \
    return result;

#define CHECK_OP_SET_ERR_RESULT(os, errorMessage, result) \
    CHECK_SET_ERR_RESULT(!os.isCoR(), errorMessage, result)

/** Used in util methods */
#define GT_CHECK(condition, errorMessage) \
    GT_CHECK_RESULT(condition, errorMessage, )

#define GT_CHECK_NO_MESSAGE(condition, errorMessage) \
    if (!(condition)) { \
        GT_CHECK(condition, errorMessage) \
    }

#define GT_CHECK_RESULT(condition, errorMessage, result) \
    CHECK_SET_ERR_RESULT(condition, GT_CLASS_NAME " __ " GT_METHOD_NAME " _  " + QString(errorMessage), result)

#define GT_CHECK_OP_RESULT(os, errorMessage, result) \
    GT_CHECK_RESULT(!(os).isCoR(), errorMessage, result)

#define DRIVER_CHECK(condition, errorMessage) \
    if (!(condition)) { \
        qCritical("Driver error: '%s'", QString(errorMessage).toLocal8Bit().constData()); \
        return false; \
    }

}  // namespace HI

#endif
