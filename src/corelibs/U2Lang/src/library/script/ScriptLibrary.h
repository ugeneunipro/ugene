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

#ifndef _WORKFLOW_SCRIPT_FUNC_H_
#define _WORKFLOW_SCRIPT_FUNC_H_

#include <QScriptValue>

#include <U2Core/global.h>

#include <U2Lang/DbiDataHandler.h>

class QScriptContext;

namespace U2 {

class WorkflowScriptRegistry;
class WorkflowScriptFactory;
class WorkflowScriptEngine;

class U2LANG_EXPORT WorkflowScriptFactory {
public:
    virtual ~WorkflowScriptFactory();

    virtual void createScript(WorkflowScriptEngine *engine) = 0;
};

class U2LANG_EXPORT WorkflowScriptRegistry {
public:
    ~WorkflowScriptRegistry();
    void registerScriptFactory(WorkflowScriptFactory* f);
    void unregisterScriptFactory(WorkflowScriptFactory* f);
    const QList<WorkflowScriptFactory*> &getFactories() const;
private:
    QList<WorkflowScriptFactory*> list;
};

class U2LANG_EXPORT WorkflowScriptLibrary {
public:

    static WorkflowScriptRegistry* getScriptRegistry();

    static void initEngine(WorkflowScriptEngine *engine);

    // ================== Misc =======================
    static QScriptValue print(QScriptContext *ctx, QScriptEngine *);


    // unrefactored obsolete deprecated functions
    // ================== Sequence =======================
    static QScriptValue getSubsequence(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue concatSequence(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue complement(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue sequenceSize(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue translate(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue charAt(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue sequenceName(QScriptContext *ctx, QScriptEngine *);
    static QScriptValue alphabetType(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue sequenceFromText(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue isAmino(QScriptContext *ctx, QScriptEngine *);
    static QScriptValue getMinimumQuality(QScriptContext *ctx, QScriptEngine *);
    static QScriptValue getTrimmedByQuality(QScriptContext *ctx, QScriptEngine *);
    static QScriptValue hasQuality(QScriptContext *ctx, QScriptEngine *);

    // ================== Alignment =======================
    static QScriptValue getSequenceFromAlignment(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue findInAlignment(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue createAlignment(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue addToAlignment(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue removeFromAlignment(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue rowNum(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue columnNum(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue alignmentAlphabetType(QScriptContext *ctx, QScriptEngine *engine);

    // ================== Annotations =======================
    static QScriptValue getAnnotationRegion(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue filterByQualifier(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue addQualifier(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue getLocation(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue hasAnnotationName(QScriptContext *ctx, QScriptEngine *engine);

    // ================== Files =======================
    static QScriptValue writeFile(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue appendFile(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue readFile(QScriptContext *ctx, QScriptEngine *engine);
    static QScriptValue readSequences(QScriptContext *ctx, QScriptEngine *engine);


    // ================== Misc =======================
    static QScriptValue debugOut(QScriptContext *ctx, QScriptEngine *);

}; // WorkflowScriptLibrary

} // U2

#endif
