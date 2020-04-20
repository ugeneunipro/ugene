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

#ifndef _U2_SCRIPT_TASK_H_
#define _U2_SCRIPT_TASK_H_

#include <U2Core/Task.h>

#include <QMap>
#include <QScriptValue>

namespace U2 {

// Class to configure script task
class U2CORE_EXPORT ScriptTaskSettings {
public:
    ScriptTaskSettings() : mainThreadScript(false) {}

    // Tells if to run script in main thread. Otherwise the task is executed during reporting stage
    bool mainThreadScript;

    // Map of input script parameters
    QMap<QString, QScriptValue> inputParametersMap;
};

// Basic implementation of script task
class U2CORE_EXPORT ScriptTask : public Task {
    Q_OBJECT
public:

    ScriptTask(const QString& taskName, const QString& scriptText, const ScriptTaskSettings& settings = ScriptTaskSettings());

    // Returns settings this task was/will run with
    const ScriptTaskSettings& getSettings() const {return conf;}

    // Returns the result of script execution
    const QScriptValue& getResult() const {return result;}

    bool isMainThreadScript() const {return conf.mainThreadScript;}

    // Used to run script if mainThreadScript is FALSE
    void run();

    // Used to run script if mainThreadScript is TRUE
    ReportResult report();

    // Runs the script using given input parameters
    // Returns the result of the script
    // If error occurs -> sets the error message into 'errorMessage' parameter
    static QScriptValue runScript(QScriptEngine* engine, const QMap<QString, QScriptValue>& inputParametersMap,
        const QString& scriptText, TaskStateInfo& stateInfo);

private:
    QString             scriptText;
    ScriptTaskSettings  conf;
    QScriptValue        result;
};

} //namespace

#endif
