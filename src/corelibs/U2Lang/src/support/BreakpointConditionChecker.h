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

#ifndef _U2_BREAKPOINT_CONDITION_CHECKER_H_
#define _U2_BREAKPOINT_CONDITION_CHECKER_H_

#include <QMutex>

#include <U2Lang/WorkflowContext.h>

namespace U2 {

class AttributeScript;
class WorkflowScriptEngine;
enum BreakpointConditionParameter;

class BreakpointConditionChecker {
    Q_DISABLE_COPY(BreakpointConditionChecker)
public:
    BreakpointConditionChecker(const QString &initConditionText,
        Workflow::WorkflowContext *context = NULL);
    ~BreakpointConditionChecker();

    void setContext(Workflow::WorkflowContext *context);
    bool evaluateCondition(const AttributeScript *conditionContext);

    void setConditionText(const QString &text);
    QString getConditionText() const;
    void setEnabled(bool enable);
    bool isEnabled() const;
    void setConditionParameter(BreakpointConditionParameter newParameter);
    BreakpointConditionParameter getConditionParameter() const;

private:

    QString conditionText;
    WorkflowScriptEngine *engine;
    bool enabled;
    BreakpointConditionParameter parameter;
    int lastConditionEvaluation;
    QMutex engineGuard;
};

} // namespace U2

#endif // _U2_BREAKPOINT_CONDITION_CHECKER_H_
