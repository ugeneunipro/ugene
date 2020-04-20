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

#include <QMutexLocker>

#include <U2Lang/Attribute.h>
#include <U2Lang/WorkflowBreakpointSharedInfo.h>
#include <U2Lang/WorkflowScriptEngine.h>

#include "BreakpointConditionChecker.h"

const int DEFAULT_CONDITION_EVAL_RESULT = -1;

namespace U2 {

BreakpointConditionChecker::BreakpointConditionChecker(const QString &initConditionText,
    Workflow::WorkflowContext *context) :
    conditionText(initConditionText),
    engine((NULL == context) ? NULL : new WorkflowScriptEngine(context)),
    enabled(false),
    parameter(IS_TRUE),
    lastConditionEvaluation(DEFAULT_CONDITION_EVAL_RESULT),
    engineGuard()
{
    if (NULL != engine) {
        WorkflowScriptLibrary::initEngine(engine);
    }
}

BreakpointConditionChecker::~BreakpointConditionChecker() {
    delete engine;
}

void BreakpointConditionChecker::setContext(Workflow::WorkflowContext *context) {
    QMutexLocker lock(&engineGuard);
    if (NULL == context) {
        delete engine;
        engine = NULL;
    }
    else if (NULL == engine) {
        engine = new WorkflowScriptEngine(context);
        WorkflowScriptLibrary::initEngine(engine);
    }
}

bool BreakpointConditionChecker::evaluateCondition(const AttributeScript *conditionContext) {
    Q_ASSERT(NULL != conditionContext);

    QMutexLocker lock(&engineGuard);

    if (NULL == engine || NULL == engine->getWorkflowContext()) {
        return false;
    }
    if (conditionText.isEmpty() || !enabled) {
        return true;
    }

    QMap<QString, QScriptValue> scriptVars;
    foreach (const Descriptor & key, conditionContext->getScriptVars().uniqueKeys()) {
        assert(!key.getId().isEmpty());
        scriptVars[key.getId()] = engine->newVariant(conditionContext->getScriptVars().value(key));
    }
    TaskStateInfo stateInfo;
    QScriptValue evaluationResult = ScriptTask::runScript(engine, scriptVars, conditionText,
        stateInfo);
    if (stateInfo.hasError()) {
        coreLog.error("Breakpoint condition evaluation failed. Error:\n" + stateInfo.getError());
        return false;
    } else if (evaluationResult.isBool()) {
        bool evaluatedResult = evaluationResult.toBool();
        if (HAS_CHANGED == parameter) {
            const bool returningValue = (DEFAULT_CONDITION_EVAL_RESULT == lastConditionEvaluation)
                ? false : (static_cast<bool>(lastConditionEvaluation) != evaluatedResult);
            lastConditionEvaluation = static_cast<int>(evaluatedResult);
            evaluatedResult = returningValue;
        }
        coreLog.trace(QString("Condition of breakpoint is %1").arg(evaluatedResult
            ? "true" : "false"));
        return evaluatedResult;
    } else {
        coreLog.error("Breakpoint condition's evaluation has provided no boolean value");
        return false;
    }
}

void BreakpointConditionChecker::setEnabled(bool enable) {
    enabled = enable;
}

bool BreakpointConditionChecker::isEnabled() const {
    return enabled;
}

QString BreakpointConditionChecker::getConditionText() const {
    return conditionText;
}

void BreakpointConditionChecker::setConditionParameter(
    BreakpointConditionParameter newParameter)
{
    parameter = newParameter;
}

BreakpointConditionParameter BreakpointConditionChecker::getConditionParameter() const {
    return parameter;
}

void BreakpointConditionChecker::setConditionText(const QString &text) {
    conditionText = text;
}

} // namespace U2
