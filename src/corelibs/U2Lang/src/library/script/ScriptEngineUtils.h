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

#ifndef _U2_SCRIPTENGINEUTILS_H_
#define _U2_SCRIPTENGINEUTILS_H_

#include <U2Lang/WorkflowScriptEngine.h>

#define SCRIPT_CHECK(condition, ctx, error, result) \
    if (!(condition)) { \
    ctx->throwError(error); \
    return result; \
    }

namespace U2 {

class SequenceScriptClass;
using namespace Workflow;

class U2LANG_EXPORT ScriptEngineUtils {
public:
    static WorkflowScriptEngine * workflowEngine(QScriptEngine *engine);
    static DbiDataStorage * dataStorage(QScriptEngine *engine);
    static SequenceScriptClass * getSequenceClass(QScriptEngine *engine);
    static SharedDbiDataHandler getDbiId(QScriptEngine *engine, const QScriptValue &value, const QString &className);
    static SharedDbiDataHandler getDbiId(QScriptEngine *engine, const QScriptValue &value);
    static QScriptValue toScriptValue(QScriptEngine *engine, const QVariant &value, DataTypePtr type);
    static QVariant fromScriptValue(QScriptEngine *engine, const QScriptValue &value, DataTypePtr type);
};

} // U2

#endif // _U2_SCRIPTENGINEUTILS_H_
