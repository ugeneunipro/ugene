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
#undef QT_DISABLE_DEPRECATED_BEFORE
#include "config.h" // compile on Windows
#include "qscriptstring.h"
#include "qscriptstring_p.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptString
  \inmodule QtScript
  \brief The QScriptString class acts as a handle to "interned" strings in a QScriptEngine.

  \ingroup script


  QScriptString can be used to achieve faster (repeated)
  property getting/setting, and comparison of property names, of
  script objects.

  To get a QScriptString representation of a string, pass the string
  to QScriptEngine::toStringHandle(). The typical usage pattern is to
  register one or more pre-defined strings when setting up your script
  environment, then subsequently use the relevant QScriptString as
  argument to e.g. QScriptValue::property().

  Call the toString() function to obtain the string that a
  QScriptString represents.

  Call the toArrayIndex() function to convert a QScriptString to an
  array index. This is useful when using QScriptClass to implement
  array-like objects.
*/

/*!
  Constructs an invalid QScriptString.
*/
QScriptString::QScriptString()
    : d_ptr(0)
{
}

/*!
  Constructs a new QScriptString that is a copy of \a other.
*/
QScriptString::QScriptString(const QScriptString &other)
    : d_ptr(other.d_ptr)
{
    if (d_func() && (d_func()->type == QScriptStringPrivate::StackAllocated)) {
        Q_ASSERT(d_func()->ref.load() != 1);
        d_ptr.detach();
        d_func()->ref.store(1);
        d_func()->type = QScriptStringPrivate::HeapAllocated;
        d_func()->engine->registerScriptString(d_func());
    }
}

/*!
  Destroys this QScriptString.
*/
QScriptString::~QScriptString()
{
    Q_D(QScriptString);
    if (d) {
        switch (d->type) {
        case QScriptStringPrivate::StackAllocated:
            Q_ASSERT(d->ref.load() == 1);
            d->ref.ref(); // avoid deletion
            break;
        case QScriptStringPrivate::HeapAllocated:
            if (d->engine && (d->ref.load() == 1)) {
                // Make sure the identifier is removed from the correct engine.
                QScript::APIShim shim(d->engine);
                d->identifier = JSC::Identifier();
                d->engine->unregisterScriptString(d);
            }
            break;
        }
    }
}

/*!
  Assigns the \a other value to this QScriptString.
*/
QScriptString &QScriptString::operator=(const QScriptString &other)
{
    if (d_func() && d_func()->engine && (d_func()->ref.load() == 1) && (d_func()->type == QScriptStringPrivate::HeapAllocated)) {
        // current d_ptr will be deleted at the assignment below, so unregister it first
        d_func()->engine->unregisterScriptString(d_func());
    }
    d_ptr = other.d_ptr;
    if (d_func() && (d_func()->type == QScriptStringPrivate::StackAllocated)) {
        Q_ASSERT(d_func()->ref.load() != 1);
        d_ptr.detach();
        d_func()->ref.store(1);
        d_func()->type = QScriptStringPrivate::HeapAllocated;
        d_func()->engine->registerScriptString(d_func());
    }
    return *this;
}

/*!
  Returns true if this QScriptString is valid; otherwise
  returns false.
*/
bool QScriptString::isValid() const
{
    return QScriptStringPrivate::isValid(*this);
}

/*!
  Returns true if this QScriptString is equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator==(const QScriptString &other) const
{
    Q_D(const QScriptString);
    if (!d || !other.d_func())
        return d == other.d_func();
    return d->identifier == other.d_func()->identifier;
}

/*!
  Returns true if this QScriptString is not equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator!=(const QScriptString &other) const
{
    return !operator==(other);
}

/*!
  \since 4.6

  Attempts to convert this QScriptString to a Qt Script array index,
  and returns the result.

  If a conversion error occurs, *\a{ok} is set to false; otherwise
  *\a{ok} is set to true.
*/
quint32 QScriptString::toArrayIndex(bool *ok) const
{
    Q_D(const QScriptString);
    if (!d) {
        if (ok)
            *ok = false;
        return -1;
    }
    bool tmp;
    bool *okok = ok ? ok : &tmp;
    quint32 result = d->identifier.toArrayIndex(okok);
    if (!*okok)
        result = -1;
    return result;
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa isValid()
*/
QString QScriptString::toString() const
{
    Q_D(const QScriptString);
    if (!d || !d->engine)
        return QString();
    return d->identifier.ustring();
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa toString()
*/
QScriptString::operator QString() const
{
    return toString();
}

uint qHash(const QScriptString &key)
{
    QScriptStringPrivate *d = QScriptStringPrivate::get(key);
    if (!d)
        return 0;
    return qHash(d->identifier.ustring().rep());
}

QT_END_NAMESPACE
