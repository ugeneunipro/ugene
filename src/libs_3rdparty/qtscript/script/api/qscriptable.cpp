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

#include "qscriptable.h"
#include "qscriptable_p.h"
#include "qscriptengine.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptable
  \inmodule QtScript

  \brief The QScriptable class provides access to the Qt Script environment from Qt C++ member functions.

  \ingroup script


  With QScriptEngine::newQObject(), you can expose the signals and
  slots and properties of any QObject (or subclass) to script
  code. QScriptable augments this functionality by giving your C++
  members access to the Qt Script environment they are invoked in;
  conceptually, it is similar to QObject::sender().

  By subclassing QScriptable, you get the following functions in your
  class: thisObject(), argumentCount(), argument(), context() and
  engine(). With these functions, you have full access to the Qt
  Script environment from the slots and property access functions of
  your class, when they are invoked from script code.

  For example, you can throw a Qt Script exception from a slot;
  manipulate the `this' object associated with the function call;
  inspect the arguments stored in the QScriptContext to know the
  "real" arguments passed to the function from script code; and call
  script functions from your slot.

  A typical use case of QScriptable is to implement prototype objects
  for custom C++ types. You define the scriptable interface of your
  custom type in a QScriptable subclass using properties and slots;
  then you wrap an instance of your class using
  QScriptEngine::newQObject(), and finally pass the result to
  QScriptEngine::setDefaultPrototype(). See the \l{Default Prototypes Example}
  to see how this can be done.

  The following is what subclassing QScriptable typically looks
  like:

  \snippet code/src_script_qscriptable.cpp 0

  The only difference from regular QObject subclassing is that you
  also inherit from QScriptable.

  In the implementation of your slots, you can then use the functions
  inherited from QScriptable:

  \snippet code/src_script_qscriptable.cpp 1

  \sa {Default Prototypes Example}, QScriptEngine::newFunction()
*/

/*!
  \internal
*/
QScriptable::QScriptable()
    : d_ptr(new QScriptablePrivate())
{
    d_ptr->q_ptr = this;
}

/*!
  \internal
*/
QScriptable::~QScriptable()
{
}

/*!
  Returns a pointer to the QScriptEngine associated with the current
  Qt function call, or 0 if the Qt function was not invoked from
  script code.
*/
QScriptEngine *QScriptable::engine() const
{
    Q_D(const QScriptable);
    return d->engine;
}

/*!
  Returns a pointer to the QScriptContext associated with the current
  Qt function call, or 0 if the Qt function was not invoked from
  script code.
*/
QScriptContext *QScriptable::context() const
{
    if (QScriptEngine *e = engine())
        return e->currentContext();

    return 0;
}

/*!
  Returns the `this' object associated with the current Qt function
  call, or an invalid QScriptValue if the Qt function was not invoked
  from script code.
*/

QScriptValue QScriptable::thisObject() const
{
    if (QScriptContext *c = context())
        return c->thisObject();

    return QScriptValue();
}

/*!
  Returns the number of arguments passed to the function in this
  invocation, or -1 if the Qt function was not invoked from script
  code.

  \sa argument()
*/
int QScriptable::argumentCount() const
{
    if (QScriptContext *c = context())
        return c->argumentCount();

    return -1;
}

/*!
  Returns the function argument at the given \a index, or an invalid
  QScriptValue if the Qt function was not invoked from script code.

  \sa argumentCount()
*/
QScriptValue QScriptable::argument(int index) const
{
    if (QScriptContext *c = context())
        return c->argument(index);

    return QScriptValue();
}

QT_END_NAMESPACE
