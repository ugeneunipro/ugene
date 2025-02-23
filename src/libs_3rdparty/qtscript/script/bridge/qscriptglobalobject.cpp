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
#include "qscriptglobalobject_p.h"

#include "../api/qscriptengine.h"
#include "../api/qscriptengine_p.h"

namespace JSC
{
QT_USE_NAMESPACE

ASSERT_CLASS_FITS_IN_CELL(QScript::GlobalObject);
ASSERT_CLASS_FITS_IN_CELL(QScript::OriginalGlobalObjectProxy);

} // namespace JSC

QT_BEGIN_NAMESPACE

namespace QScript
{

GlobalObject::GlobalObject()
    : JSC::JSGlobalObject(), customGlobalObject(0)
{
}

GlobalObject::~GlobalObject()
{
}

void GlobalObject::markChildren(JSC::MarkStack& markStack)
{
    JSC::JSGlobalObject::markChildren(markStack);
    if (customGlobalObject)
        markStack.append(customGlobalObject);
}

bool GlobalObject::getOwnPropertySlot(JSC::ExecState* exec,
                                      const JSC::Identifier& propertyName,
                                      JSC::PropertySlot& slot)
{
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    if (propertyName == exec->propertyNames().arguments && engine->currentFrame->argumentCount() > 0) {
        JSC::JSValue args = engine->scriptValueToJSCValue(engine->contextForFrame(engine->currentFrame)->argumentsObject());
        slot.setValue(args);
        return true;
    }
    if (customGlobalObject)
        return customGlobalObject->getOwnPropertySlot(exec, propertyName, slot);
    return JSC::JSGlobalObject::getOwnPropertySlot(exec, propertyName, slot);
}

bool GlobalObject::getOwnPropertyDescriptor(JSC::ExecState* exec,
                                            const JSC::Identifier& propertyName,
                                            JSC::PropertyDescriptor& descriptor)
{
    // Must match the logic of getOwnPropertySlot().
    QScriptEnginePrivate *engine = scriptEngineFromExec(exec);
    if (propertyName == exec->propertyNames().arguments && engine->currentFrame->argumentCount() > 0) {
        // ### Can we get rid of this special handling of the arguments property?
        JSC::JSValue args = engine->scriptValueToJSCValue(engine->contextForFrame(engine->currentFrame)->argumentsObject());
        descriptor.setValue(args);
        return true;
    }
    if (customGlobalObject)
        return customGlobalObject->getOwnPropertyDescriptor(exec, propertyName, descriptor);
    return JSC::JSGlobalObject::getOwnPropertyDescriptor(exec, propertyName, descriptor);
}

void GlobalObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                       JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (customGlobalObject)
        customGlobalObject->put(exec, propertyName, value, slot);
    else
        JSC::JSGlobalObject::put(exec, propertyName, value, slot);
}

void GlobalObject::putWithAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                     JSC::JSValue value, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->putWithAttributes(exec, propertyName, value, attributes);
    else
        JSC::JSGlobalObject::putWithAttributes(exec, propertyName, value, attributes);
}

bool GlobalObject::deleteProperty(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (customGlobalObject)
        return customGlobalObject->deleteProperty(exec, propertyName);
    return JSC::JSGlobalObject::deleteProperty(exec, propertyName);
}

void GlobalObject::getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames,
                                       JSC::EnumerationMode mode)
{
    if (customGlobalObject)
        customGlobalObject->getOwnPropertyNames(exec, propertyNames, mode);
    else
        JSC::JSGlobalObject::getOwnPropertyNames(exec, propertyNames, mode);
}

void GlobalObject::defineGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->defineGetter(exec, propertyName, getterFunction, attributes);
    else
        JSC::JSGlobalObject::defineGetter(exec, propertyName, getterFunction, attributes);
}

void GlobalObject::defineSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction, unsigned attributes)
{
    if (customGlobalObject)
        customGlobalObject->defineSetter(exec, propertyName, setterFunction, attributes);
    else
        JSC::JSGlobalObject::defineSetter(exec, propertyName, setterFunction, attributes);
}

JSC::JSValue GlobalObject::lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (customGlobalObject)
        return customGlobalObject->lookupGetter(exec, propertyName);
    return JSC::JSGlobalObject::lookupGetter(exec, propertyName);
}

JSC::JSValue GlobalObject::lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (customGlobalObject)
        return customGlobalObject->lookupSetter(exec, propertyName);
    return JSC::JSGlobalObject::lookupSetter(exec, propertyName);
}

} // namespace QScript

QT_END_NAMESPACE
