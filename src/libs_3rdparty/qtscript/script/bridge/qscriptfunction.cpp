/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "config.h"
#include "qscriptfunction_p.h"

#include "qscriptengine_p.h"
#include "qscriptcontext.h"
#include "qscriptcontext_p.h"
#include "qscriptvalue_p.h"
#include "qscriptactivationobject_p.h"
#include "qscriptobject_p.h"

#include "JSGlobalObject.h"
#include "DebuggerCallFrame.h"
#include "Debugger.h"

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScript::FunctionWrapper));
ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScript::FunctionWithArgWrapper));
}

QT_BEGIN_NAMESPACE

namespace QScript
{

const JSC::ClassInfo FunctionWrapper::info = { "QtNativeFunctionWrapper", &PrototypeFunction::info, 0, 0 };
const JSC::ClassInfo FunctionWithArgWrapper::info = { "QtNativeFunctionWithArgWrapper", &PrototypeFunction::info, 0, 0 };

FunctionWrapper::FunctionWrapper(JSC::ExecState *exec, int length, const JSC::Identifier &name,
                                 QScriptEngine::FunctionSignature function)
    : JSC::PrototypeFunction(exec, length, name, proxyCall),
      data(new Data())
{
    data->function = function;
}

FunctionWrapper::~FunctionWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    consData.native.function.doNotCallDebuggerFunctionExit();
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWrapper::proxyCall(JSC::ExecState *exec, JSC::JSObject *callee,
                                        JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, thisObject, args, callee);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p));
    if (!result.isValid())
        result = QScriptValue(QScriptValue::UndefinedValue);

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWrapper::proxyConstruct(JSC::ExecState *exec, JSC::JSObject *callee,
                                               const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, JSC::JSValue(), args, callee, true);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p));

    if (JSC::Debugger* debugger = eng_p->originalGlobalObject()->debugger())
        debugger->functionExit(QScriptValuePrivate::get(result)->jscValue, -1);

    if (!result.isObject())
        result = ctx->thisObject();

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return JSC::asObject(eng_p->scriptValueToJSCValue(result));
}

FunctionWithArgWrapper::FunctionWithArgWrapper(JSC::ExecState *exec, int length, const JSC::Identifier &name,
                                               QScriptEngine::FunctionWithArgSignature function, void *arg)
    : JSC::PrototypeFunction(exec, length, name, proxyCall),
      data(new Data())
{
    data->function = function;
    data->arg = arg;
}

FunctionWithArgWrapper::~FunctionWithArgWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWithArgWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWithArgWrapper::proxyCall(JSC::ExecState *exec, JSC::JSObject *callee,
                                               JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, thisObject, args, callee);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p), self->data->arg);

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWithArgWrapper::proxyConstruct(JSC::ExecState *exec, JSC::JSObject *callee,
                                                      const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScript::scriptEngineFromExec(exec);

    JSC::ExecState *oldFrame = eng_p->currentFrame;
    eng_p->pushContext(exec, JSC::JSValue(), args, callee, true);
    QScriptContext *ctx = eng_p->contextForFrame(eng_p->currentFrame);

    QScriptValue result = self->data->function(ctx, QScriptEnginePrivate::get(eng_p) , self->data->arg);
    if (!result.isObject())
        result = ctx->thisObject();

    eng_p->popContext();
    eng_p->currentFrame = oldFrame;

    return JSC::asObject(eng_p->scriptValueToJSCValue(result));
}

} // namespace QScript

QT_END_NAMESPACE
