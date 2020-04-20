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

#ifndef _U2_SEQUENCEPROTOTYPE_H_
#define _U2_SEQUENCEPROTOTYPE_H_

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatus.h>

#include "DbiClassPrototype.h"

#include <QObject>
namespace U2 {

/************************************************************************/
/* SequencePrototype */
/************************************************************************/
class SequencePrototype : public DbiClassPrototype {
    Q_OBJECT
public:
    SequencePrototype(QObject *parent = NULL);

public slots:
    /** No arguments. Returns sequence length */
    qint64 length();

    /** No arguments. Returns sequence name */
    QString name();

    /**
     * Arguments: [startPos [, length]]
     * Throws script exception if the startPos or the length are negative.
     * Returns length symbols of the sequence starting from startPos.
     * If length is not specified than returns the data from startPos
     * up to the end of the sequence. If startPos is not specified then
     * returns the whole sequence data.
     */
    QString string();

    /**
     * Arguments: string [, startPos [, length]]
     * Exceptions and the meaning of startPos and length arguments are
     * the same as in the "string()" method.
     * Removes length symbols of the sequence starting from startPost
     * and inserts string instead of them.
     */
    void splice();

private:
    U2SequenceObject * getSequenceObject() const;
    /** Returns NULL and throws script exception if this object is invalid */
    U2SequenceObject * getValidSequenceObject() const;
    /** startPosArg < lengthPosArg */
    U2Region getRegion(int startPosArg, int lengthPosArg);
};

/************************************************************************/
/* SequenceScriptClass */
/************************************************************************/
class U2LANG_EXPORT SequenceScriptClass : public DbiScriptClass {
    Q_OBJECT
public:
    using DbiScriptClass::newInstance;

    SequenceScriptClass(QScriptEngine *engine);

    QString name() const;
    QScriptValue newInstance(const QString &data, const QString &name);
    QScriptValue newInstance(const ScriptDbiData &id, bool deepCopy);

    static QScriptValue constructor(QScriptContext *ctx, QScriptEngine *engine);
    static const QString CLASS_NAME;

    static Workflow::SharedDbiDataHandler copySequence(const ScriptDbiData &id, QScriptEngine *engine);

private:
    WorkflowScriptEngine * workflowEngine() const;
};

} // U2

Q_DECLARE_METATYPE(U2::SequenceScriptClass*)

#endif // _U2_SEQUENCEPROTOTYPE_H_
