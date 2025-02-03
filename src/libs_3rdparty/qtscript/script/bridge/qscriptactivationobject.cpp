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
#include "qscriptactivationobject_p.h"

#include "JSVariableObject.h"

namespace JSC
{
    ASSERT_CLASS_FITS_IN_CELL(QT_PREPEND_NAMESPACE(QScript::QScriptActivationObject));
}

QT_BEGIN_NAMESPACE

/*!
  \class QScript::QScriptActivationObject
  \internal

  Represent a scope for native function call.
*/

namespace QScript
{

const JSC::ClassInfo QScriptActivationObject::info = { "QScriptActivationObject", 0, 0, 0 };

QScriptActivationObject::QScriptActivationObject(JSC::ExecState *callFrame, JSC::JSObject *delegate)
    : JSC::JSVariableObject(callFrame->globalData().activationStructure,
                            new QScriptActivationObjectData(callFrame->registers(), delegate))
{
}

QScriptActivationObject::~QScriptActivationObject()
{
    delete d_ptr();
}

bool QScriptActivationObject::getOwnPropertySlot(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::PropertySlot& slot)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->getOwnPropertySlot(exec, propertyName, slot);
    return JSC::JSVariableObject::getOwnPropertySlot(exec, propertyName, slot);
}

bool QScriptActivationObject::getOwnPropertyDescriptor(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::PropertyDescriptor& descriptor)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->getOwnPropertyDescriptor(exec, propertyName, descriptor);
    return JSC::JSVariableObject::getOwnPropertyDescriptor(exec, propertyName, descriptor);
}

void QScriptActivationObject::getOwnPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames, JSC::EnumerationMode mode)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->getOwnPropertyNames(exec, propertyNames, mode);
        return;
    }
    return JSC::JSVariableObject::getOwnPropertyNames(exec, propertyNames, mode);
}

void QScriptActivationObject::putWithAttributes(JSC::ExecState *exec, const JSC::Identifier &propertyName, JSC::JSValue value, unsigned attributes)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->putWithAttributes(exec, propertyName, value, attributes);
        return;
    }

    if (symbolTablePutWithAttributes(propertyName, value, attributes))
        return;
    
    JSC::PutPropertySlot slot;
    JSObject::putWithAttributes(exec, propertyName, value, attributes, true, slot);
}

void QScriptActivationObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->put(exec, propertyName, value, slot);
        return;
    }
    JSC::JSVariableObject::put(exec, propertyName, value, slot);
}

void QScriptActivationObject::put(JSC::ExecState* exec, unsigned propertyName, JSC::JSValue value)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->put(exec, propertyName, value);
        return;
    }
    JSC::JSVariableObject::put(exec, propertyName, value);
}

bool QScriptActivationObject::deleteProperty(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->deleteProperty(exec, propertyName);
    return JSC::JSVariableObject::deleteProperty(exec, propertyName);
}

void QScriptActivationObject::defineGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction)
{
    if (d_ptr()->delegate != 0)
        d_ptr()->delegate->defineGetter(exec, propertyName, getterFunction);
    else
        JSC::JSVariableObject::defineGetter(exec, propertyName, getterFunction);
}

void QScriptActivationObject::defineSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction)
{
    if (d_ptr()->delegate != 0)
        d_ptr()->delegate->defineSetter(exec, propertyName, setterFunction);
    else
        JSC::JSVariableObject::defineSetter(exec, propertyName, setterFunction);
}

JSC::JSValue QScriptActivationObject::lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->lookupGetter(exec, propertyName);
    return JSC::JSVariableObject::lookupGetter(exec, propertyName);
}

JSC::JSValue QScriptActivationObject::lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->lookupSetter(exec, propertyName);
    return JSC::JSVariableObject::lookupSetter(exec, propertyName);
}

} // namespace QScript

QT_END_NAMESPACE

