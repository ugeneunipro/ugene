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

/**
    A framework to check, trace and recover from error situation in release mode.
    The logic is similar to usual assertions in debug mode but in
    release mode the check is performed and if not passed - the error message
    is dumped to log. This log can be analyzed and sent as a part of crash handler report

    NOTE: all checks below work in release mode. Be careful not to make code slow!
*/
#ifndef _U2_SAFE_POINTS_
#define _U2_SAFE_POINTS_

#include <assert.h>

#include <U2Core/global.h>

namespace U2 {
/** Safe points related utility methods. */
class U2CORE_EXPORT U2SafePoints {
public:
    /**
     * Prints error message into the core-log & calls 'assert(false)'.
     * If UGENE runs in release mode with asserts disabled this function will not stop the control flow,
     * but if GUI or XML tests mode is enabled the function will make UGENE to stop (call std::abort)
     * after printing the message.
     */
    static void fail(const QString& message);
};
}  // namespace U2

/**
    Recover utility. Must be used when code tries to recover from invalid internal state
    by returning from the method some default value.
    Traces the message to log. Asserts in debug mode.

    Warning: never use this function as a simple check since it stops application execution in debug mode
            use CHECK_OP instead
*/
#define SAFE_POINT(condition, message, result) \
    if (Q_UNLIKELY(!(condition))) { \
        U2::U2SafePoints::fail(QString("Trying to recover from error: %1 at %2:%3").arg(message).arg(__FILE__).arg(__LINE__)); \
        return result; \
    }

/**
    Recover utility. Must be used when code tries to recover from invalid internal state
    by returning from the method some default value.
    Dumps the message to the error-level log. Asserts in debug mode.

    Warning: never use this function as a simple check since it stops application execution in debug mode
    use CHECK_OP instead

*/
#define SAFE_POINT_OP(os, result) \
    if (Q_UNLIKELY(os.hasError())) { \
        U2::U2SafePoints::fail(QString("Trying to recover from error: %1 at %2:%3").arg(os.getError()).arg(__FILE__).arg(__LINE__)); \
        return result; \
    }

/**
    Recover utility. Must be used when code tries to recover from invalid internal state
    by returning from the method some default value.
    Asserts in debug mode. Performs extra recovery op in release

    Warning: never use this function as a simple check since it stops application execution in debug mode
            use CHECK_OP instead
*/
#define SAFE_POINT_EXT(condition, extraOp, result) \
    if (Q_UNLIKELY(!(condition))) { \
        QString message = U2_TOSTRING(condition); \
        U2::U2SafePoints::fail(QString("Trying to recover from error: %1 at %2:%3").arg(message).arg(__FILE__).arg(__LINE__)); \
        extraOp; \
        return result; \
    }

/**
    FAIL utility. Same as SAFE_POINT but uses unconditional fail.
    Can be used in code that must be unreachable
*/
#define FAIL(message, result) \
    U2::U2SafePoints::fail(QString("Trying to recover from error: %1 at %2:%3").arg(message).arg(__FILE__).arg(__LINE__)); \
    return result;

/**
 * Checks condition and runs the extra op if the condition is falsy.
 * Do not use directly: designed to be used as a low level implementation for other utilities.
 */
#define RUN_IF_FALTHY_1(condition, extraOp) \
    if (!(condition)) { \
        extraOp; \
    }

/**
 * Checks condition and runs 2 extra ops if the condition is falsy.
 * Do not use directly: designed to be used as a low level implementation for other utilities.
 */
#define RUN_IF_FALTHY_2(condition, extraOp1, extraOp2) \
    if (!(condition)) { \
        extraOp1; \
        extraOp2; \
    }

/** Checks 'condition' and returns if result is 'false'. */
#define CHECK(condition, result) RUN_IF_FALTHY_1(condition, return result)

/** Checks 'condition' and calls 'break' if 'condition' is 'false'. */
#define CHECK_BREAK(condition) RUN_IF_FALTHY_1(condition, break)

/** Checks 'condition' and calls 'continue' if 'condition' is 'false'. */
#define CHECK_CONTINUE(condition) RUN_IF_FALTHY_1(condition, continue)

/** Checks 'condition' and executes 'extraOp' and returns 'result' if 'condition' is 'false'. */
#define CHECK_EXT(condition, extraOp, result) RUN_IF_FALTHY_2(condition, extraOp, return result)

/** Checks 'condition' and executes 'extraOp' and calls 'break' if 'condition' is 'false'. */
#define CHECK_EXT_BREAK(condition, extraOp) RUN_IF_FALTHY_2(condition, extraOp, break)

/** Checks 'condition' and calls 'extraOp' and 'continue' if 'condition' is 'false'. */
#define CHECK_EXT_CONTINUE(condition, extraOp) RUN_IF_FALTHY_2(condition, extraOp, continue)

/** Checks if 'os' has error or is cancelled and returns 'result' if it does. */
#define CHECK_OP(os, result) CHECK(!os.isCoR(), result)

/** Checks if 'os' has error or is cancelled and calls 'break' if it does. */
#define CHECK_OP_BREAK(os) CHECK_BREAK(!os.isCoR())

/** Checks if 'os' has error or is cancelled and executes 'extraOp' and returns 'result' if it does. */
#define CHECK_OP_EXT(os, extraOp, result) CHECK_EXT(!(os.isCoR()), extraOp, result)

#endif
