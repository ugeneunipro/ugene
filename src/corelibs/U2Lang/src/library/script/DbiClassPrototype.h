/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QObject>
#include <QScriptClass>
#include <QScriptable>

#include <U2Core/ScriptEngine.h>

#include <U2Lang/DbiDataHandler.h>

namespace U2 {

class WorkflowScriptEngine;

/**
 * Keeps shared dbi data handler
 */
class ScriptDbiData {
public:
    ScriptDbiData();
    virtual ~ScriptDbiData();
    ScriptDbiData(const Workflow::SharedDbiDataHandler& seqId);

    const Workflow::SharedDbiDataHandler& getId() const;

    /** Removes own dbi data handler */
    void release();

private:
    Workflow::SharedDbiDataHandler id;
};

/**
 * Base class for dbi class prototypes
 */
class DbiClassPrototype : public QObject, public QScriptable {
    Q_OBJECT
public:
    DbiClassPrototype(QObject* parent = nullptr);
    virtual ~DbiClassPrototype();

public:
    template<class T>
    static void registerScriptClass(QScriptEngine* engine);

public slots:
    /** Returns NullValue if the id was released */
    QScriptValue getId();

    /** Removes dbi id from own script dbi data */
    void release();

protected:
    ScriptDbiData* thisData() const;
    WorkflowScriptEngine* workflowEngine() const;
    Workflow::DbiDataStorage* dataStorage() const;
};

/**
 * Base class for dbi classes.
 * Its inheritors must have this public method for constructing new instances:
 * static QScriptValue constructor(QScriptContext *ctx, QScriptEngine *engine);
 *
 * and this public field for keeping script class name:
 * static const QString CLASS_NAME;
 */
class DbiScriptClass : public QObject, public QScriptClass {
public:
    DbiScriptClass(QScriptEngine* engine);

    QScriptValue prototype() const override;
    QScriptValue newInstance(const Workflow::SharedDbiDataHandler& id);

    /** Virtual copy constructor for dbi types */
    virtual QScriptValue newInstance(const ScriptDbiData& id, bool deepCopy) = 0;

protected:
    QScriptValue proto;

protected:
    template<class T>
    static QScriptValue toScriptValue(QScriptEngine* engine, const ScriptDbiData& id);
    static void fromScriptValue(const QScriptValue& obj, ScriptDbiData& id);
};

}  // namespace U2

/** Template implementations */
namespace U2 {

template<class T>
void DbiClassPrototype::registerScriptClass(QScriptEngine* engine) {
    auto sClass = new T(engine);
    QScriptValue constructorFun = engine->newFunction(T::constructor, sClass->prototype());
    constructorFun.setData(qScriptValueFromValue(engine, sClass));
    engine->globalObject().setProperty(sClass->name(), constructorFun);
}

template<class T>
QScriptValue DbiScriptClass::toScriptValue(QScriptEngine* engine, const ScriptDbiData& id) {
    QScriptValue factory = engine->globalObject().property(T::CLASS_NAME);
    auto sClass = qscriptvalue_cast<T*>(factory.data());
    if (!sClass) {
        return engine->newVariant(QVariant::fromValue(id));
    }
    return sClass->newInstance(id, false /* deepCopy */);
}

}  // namespace U2

Q_DECLARE_METATYPE(U2::ScriptDbiData)
Q_DECLARE_METATYPE(U2::ScriptDbiData*)
