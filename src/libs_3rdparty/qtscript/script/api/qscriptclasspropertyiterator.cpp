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

#include "qscriptclasspropertyiterator.h"

#include "qscriptstring.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptClassPropertyIterator
  \inmodule QtScript
  \brief The QScriptClassPropertyIterator class provides an iterator interface for custom Qt Script objects.

  \ingroup script

  This class is only relevant if you have subclassed QScriptClass and
  want to provide enumeration of your custom properties (e.g. when
  objects of your class are used with QScriptValueIterator, or with
  the for-in statement in scripts).

  The object() function returns the Qt Script object the iterator is
  traversing.

  toFront(), hasNext() and next() provide forward iteration.

  toBack(), hasPrevious() and previous() provide backward iteration.

  name(), id() and flags() return information about the last property
  that was jumped over using next() or previous().

  \sa QScriptClass::newIterator(), QScriptValueIterator
*/

class QScriptClassPropertyIteratorPrivate
{
    Q_DECLARE_PUBLIC(QScriptClassPropertyIterator)
public:
    QScriptClassPropertyIteratorPrivate() {}
    virtual ~QScriptClassPropertyIteratorPrivate() {}

    QScriptValue object;

    QScriptClassPropertyIterator *q_ptr;
};

/*!
  Constructs an iterator for traversing \a object.

  Subclasses should ensure that the iterator is set to the front of the
  sequence of properties (before the first property).
*/
QScriptClassPropertyIterator::QScriptClassPropertyIterator(const QScriptValue &object)
    : d_ptr(new QScriptClassPropertyIteratorPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->object = object;
}

/*!
  \internal
*/
QScriptClassPropertyIterator::QScriptClassPropertyIterator(const QScriptValue &object,
                                           QScriptClassPropertyIteratorPrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    d_ptr->object = object;
}

/*!
  Destroys the iterator.
*/
QScriptClassPropertyIterator::~QScriptClassPropertyIterator()
{
}

/*!
  Returns the Qt Script object this iterator is traversing.
*/
QScriptValue QScriptClassPropertyIterator::object() const
{
    Q_D(const QScriptClassPropertyIterator);
    return d->object;
}

/*!
  \fn bool QScriptClassPropertyIterator::hasNext() const

  Returns true if there is at least one item ahead of the iterator
  (i.e. the iterator is \e not at the back of the property sequence);
  otherwise returns false.

  \sa next(), hasPrevious()
*/

/*!
  \fn void QScriptClassPropertyIterator::next()

  Advances the iterator by one position.

  Calling this function on an iterator located at the back of the
  container leads to undefined results.

  \sa hasNext(), previous(), name()
*/

/*!
  \fn bool QScriptClassPropertyIterator::hasPrevious() const

  Returns true if there is at least one item behind the iterator
  (i.e. the iterator is \e not at the front of the property sequence);
  otherwise returns false.

  \sa previous(), hasNext()
*/

/*!
  \fn void QScriptClassPropertyIterator::previous()

  Moves the iterator back by one position.

  Calling this function on an iterator located at the front of the
  container leads to undefined results.

  \sa hasPrevious(), next(), name()
*/

/*!
  \fn void QScriptClassPropertyIterator::toFront()

  Moves the iterator to the front of the QScriptValue (before the
  first property).

  \sa toBack(), next()
*/

/*!
  \fn void QScriptClassPropertyIterator::toBack()

  Moves the iterator to the back of the QScriptValue (after the
  last property).

  \sa toFront(), previous()
*/

/*!
  \fn QScriptString QScriptClassPropertyIterator::name() const

  Returns the name of the last property that was jumped over using
  next() or previous().

  \sa id()
*/

/*!
  \fn uint QScriptClassPropertyIterator::id() const

  Returns the id of the last property that was jumped over using
  next() or previous().

  The default implementation returns 0.

  \sa name()
*/
uint QScriptClassPropertyIterator::id() const
{
    return 0;
}

/*!
  Returns the flags of the last property that was jumped over using
  next() or previous().

  The default implementation calls the propertyFlags() function of
  object() with argument name().
*/
QScriptValue::PropertyFlags QScriptClassPropertyIterator::flags() const
{
    return object().propertyFlags(name());
}

QT_END_NAMESPACE
