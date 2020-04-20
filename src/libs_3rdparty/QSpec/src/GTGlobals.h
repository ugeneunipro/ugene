/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include <core/global.h>
#include <core/GUITestOpStatus.h>
#include <QMessageLogger>

#include <QAction>

namespace HI {
/*!
 * \brief The class contains the most commonly used methods
 */
class HI_EXPORT GTGlobals {
public:
    enum UseMethod {UseMouse, UseKey, UseKeyBoard};
    enum WindowAction {Minimize, Maximize, Close, WindowActionCount};

    // if failIfNull is set to true, fails if object wasn't found
	class HI_EXPORT FindOptions {
    public:
        FindOptions(bool failIfNotFound = true, Qt::MatchFlags matchPolicy = Qt::MatchExactly, int depth = INFINITE_DEPTH, bool searchInHidden = false);

        bool failIfNotFound;
        Qt::MatchFlags matchPolicy;
        int depth;
        bool searchInHidden;

        static const int INFINITE_DEPTH = 0;
        static const bool FAIL_IF_NOT_FOUND = true;
    };

    static void sleep(int msec = 2000);
    static void systemSleep(int sec = 2);
    static void sendEvent(QObject *obj, QEvent *e);
    static void takeScreenShot(QString path);
    static void GUITestFail();
};

#define GT_DEBUG_MESSAGE(condition, errorMessage, result) \
{ \
    QString cond = #condition;\
    if (condition) { \
        qDebug("GT_DEBUG_MESSAGE Checking condition (%s). Result: OK", cond.toLocal8Bit().constData()); \
    } \
    else { \
        qWarning("\n------------"); \
        qWarning("GT_DEBUG_MESSAGE Checking condition (%s). Result: FAILED", cond.toLocal8Bit().constData()); \
        qWarning("GT_DEBUG_MESSAGE errorMessage '%s'", QString(errorMessage).toLocal8Bit().constData()); \
        qWarning("------------\n"); \
    } \
    if (os.hasError()) { \
        qCritical("GT_DEBUG_MESSAGE OpStatus already has error"); \
        qCritical("GT_DEBUG_MESSAGE OpStatus error '%s'",os.getError().toLocal8Bit().constData()); \
    } \
}

/**
    Checks condition is false and returns the result if it is.
    Before the result is returned the 'extraOp' operation is performed (for example logging)

    Code style hint: use CHECK macro only to make error processing more compact but not all if {return;} patterns !
*/
#define CHECK_EXT(condition, extraOp, result) \
    if (!(condition)) { \
        extraOp; \
        return result; \
    }

/** Used in tests */
#define CHECK_SET_ERR(condition, errorMessage) \
    CHECK_SET_ERR_RESULT(condition, errorMessage, )

#define CHECK_SET_ERR_NO_MESSAGE(condition, errorMessage) \
    if(!condition){ \
    CHECK_SET_ERR(condition, errorMessage) \
}

#define CHECK_OP_SET_ERR(os, errorMessage) \
    CHECK_SET_ERR(!os.isCoR(), errorMessage)

#define CHECK_SET_ERR_RESULT(condition, errorMessage, result) \
{ \
    GT_DEBUG_MESSAGE(condition, errorMessage, result); \
    if (os.hasError()) { HI::GTGlobals::GUITestFail(); os.setError(os.getError()); return result; } \
    CHECK_EXT(condition, if (!os.hasError()) { HI::GTGlobals::GUITestFail(); os.setError(errorMessage);}, result) \
}

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

#define GT_CHECK_OP(os, errorMessage) \
    GT_CHECK(!os.isCoR(), errorMessage)

#define GT_CHECK_OP_RESULT(os, errorMessage, result) \
    GT_CHECK_RESULT(!os.isCoR(), errorMessage, result)

#define DRIVER_CHECK(condition, errorMessage) \
    if(!condition){ \
        qCritical("Driver error: '%s'",QString(errorMessage).toLocal8Bit().constData()); \
        return false; \
    } \

} //namespace

#endif
