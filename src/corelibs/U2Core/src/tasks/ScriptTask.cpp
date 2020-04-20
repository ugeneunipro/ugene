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

#include "ScriptTask.h"

#include <QScriptEngine>

namespace U2 {


ScriptTask::ScriptTask(const QString& taskName, const QString& _scriptText, const ScriptTaskSettings& _settings)
: Task(taskName, _settings.mainThreadScript ? TaskFlag_NoRun : TaskFlag_None), scriptText(_scriptText), conf(_settings)
{
    setVerboseLogMode(true);
}

void ScriptTask::run() {
    assert(!isMainThreadScript());
    QScriptEngine engine;
    result = runScript(&engine, conf.inputParametersMap, scriptText, stateInfo);
}

Task::ReportResult ScriptTask::report() {
    if (isMainThreadScript()) {
        QScriptEngine engine;
        result = runScript(&engine, conf.inputParametersMap, scriptText, stateInfo);
    }
    return ReportResult_Finished;
}


QScriptValue ScriptTask::runScript(QScriptEngine* engine, const QMap<QString, QScriptValue>& inputParametersMap, const QString& scriptText, TaskStateInfo& stateInfo) {
    // create new script engine
    QScriptValue result;

    // setup all input params as global vars
    QScriptValue thiz = engine->globalObject();
    foreach(const QString& varName, inputParametersMap.keys()) {
        QScriptValue varVal = inputParametersMap.value(varName);
        thiz.setProperty(varName, varVal);
    }

    // check script syntax
    QScriptSyntaxCheckResult syntaxResult = engine->checkSyntax(scriptText);
    if (syntaxResult.state() != QScriptSyntaxCheckResult::Valid) {
        stateInfo.setError(tr("Script syntax check failed! Line: %1, error: %2").arg(syntaxResult.errorLineNumber()).arg(syntaxResult.errorMessage()));
        return result;
    }

    // run the script
    result = engine->evaluate(scriptText);

    if (engine->hasUncaughtException()) {
        stateInfo.setError(tr("Exception during script execution! Line: %1, error: %2")
                        .arg(engine->uncaughtExceptionLineNumber()).arg(engine->uncaughtExceptionBacktrace().join("\n")));
    }
    return result;
}


} //namespace

