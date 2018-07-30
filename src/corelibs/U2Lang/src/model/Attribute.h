/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_WORKFLOW_ATTR_H_
#define _U2_WORKFLOW_ATTR_H_

#include <cassert>

#include <QVariant>
#include <QScriptValue>
#include <QScriptEngine>

#include <U2Core/Task.h>
#include <U2Core/ScriptTask.h>

#include <U2Lang/AttributeRelation.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/Descriptor.h>
#include <U2Lang/ScriptLibrary.h>
#include <U2Lang/SupportClass.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowScriptEngine.h>

#include "PortRelation.h"
#include "SlotRelation.h"

namespace U2 {

typedef QString ActorId;
inline ActorId str2aid(const QString& s) {return s;}
inline QString aid2str(const ActorId& s) {return s;}

/**
 * attribute value can be obtained from script
 */
class U2LANG_EXPORT AttributeScript {
public:
    AttributeScript(const QString & text);
    AttributeScript() {}

    bool isEmpty() const;

    void setScriptText(const QString & t);
    const QString & getScriptText() const;

    void setScriptVar(const Descriptor & desc, const QVariant & val);
    const QMap<Descriptor, QVariant> & getScriptVars() const;
    void clearScriptVars();

    bool hasVarWithId(const QString & varName)const;
    bool hasVarWithDesc(const QString & varName)const;
    void setVarValueWithId(const QString & varName, const QVariant & value);

private:
    QString                         text;
    QMap<Descriptor, QVariant>      vars; // (desc, val)

}; // AttributeScript

/**
 * Existing types of attributes
 */
enum AttributeGroup {
    COMMON_GROUP,
    MARKER_GROUP,
    GROUPER_SLOT_GROUP
};

/**
 * Value of certain type that can be identified by (descriptors) id
 */
class U2LANG_EXPORT Attribute : public Descriptor {
public:
    enum Flag {
        None = 0,
        CanBeEmpty = 1,             // it has meaning only for required attributes, allows the required attribute to be empty
        NeedValidateEncoding = 2,   // it has meaning that the attribute is some string which must be validated against Unicode -> Byte convertion
                                    // for example many plugins/external tools accept filenames as char*, but QString -> char* convertion may
                                    // produce invalid values if QString's encoding is not the same as system Locale Code Page
        Required = 4,               // values of required attributes cannot be empty, if the appropriate flag is not set
        Hidden = 8                  // Hidden attribute
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    Attribute(const Descriptor& descriptor, const DataTypePtr type, const Flags flags = None, const QVariant & defaultValue = QVariant());
    Attribute(const Descriptor& d, const DataTypePtr type, bool required, const QVariant & defaultValue = QVariant());
    ~Attribute();

    // getters/setters
    const DataTypePtr getAttributeType()const;
    bool isRequiredAttribute() const;
    bool canBeEmpty() const;
    bool needValidateEncoding() const;
    Flags getFlags() const;

    virtual void setAttributeValue(const QVariant & newVal);
    // attribute value is kept in qvariant
    // but it can be transformed to value of specific type using scripting or not (see getAttributeValue)
    virtual const QVariant &getAttributePureValue() const;
    virtual const QVariant &getDefaultPureValue() const;
    virtual bool isDefaultValue() const;


    // base realization without scripting. to support scripting for other types: see template realizations
    template<typename T> T getAttributeValue(Workflow::WorkflowContext *) const {
        return getAttributeValueWithoutScript<T>();
    }

    template<typename T> T getAttributeValueWithoutScript() const {
        return value.value<T>();
    }

    const AttributeScript & getAttributeScript() const;
    // used to change script data
    AttributeScript & getAttributeScript();

    // stores value and script data in variant
    // used in saving schema to xml
    QVariant toVariant() const;
    // reads value and script from variant
    // used in reading schema from xml
    bool fromVariant(const QVariant& variant);
    bool isEmptyString() const;
    void addRelation(const AttributeRelation *relation);
    QVector<const AttributeRelation*> &getRelations();

    void addPortRelation(PortRelationDescriptor* relationDesc);
    const QList<PortRelationDescriptor*>& getPortRelations() const;

    void addSlotRelation(const SlotRelationDescriptor& relationDesc);
    const QList<SlotRelationDescriptor>& getSlotRelations() const;

    virtual bool isEmpty() const;
    virtual Attribute *clone();
    virtual AttributeGroup getGroup();

    /**
     * Ids of actors in the scheme can be changed (after copy-paste, for example).
     * It is needed to update all of ids which are used by this attribute
     */
    virtual void updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap);

    virtual bool validate(ProblemList &problemList);

private:
    void debugCheckAttributeId() const;
    void copy(const Attribute &other);

protected:
    Attribute(const Attribute &other);
    Attribute &operator =(const Attribute &other);

    // type of value
    DataTypePtr   type;
    // Different additional options
    Flags         flags;
    // pure value and default pure value. if script exists, value should be processed throw it
    QVariant            value;
    QVariant            defaultValue;
    // script text and variable values for script evaluating
    // script variables get values only in runtime
    AttributeScript     scriptData;

    QVector<const AttributeRelation*> relations;
    QList<PortRelationDescriptor*>    portRelations;
    QList<SlotRelationDescriptor>     slotRelations;

}; // Attribute

// getAttributeValue function realizations with scripting support
template<>
inline QString Attribute::getAttributeValue(Workflow::WorkflowContext *ctx) const {
    if( scriptData.isEmpty() ) {
        return getAttributeValueWithoutScript<QString>();
    }
    // run script
    WorkflowScriptEngine engine(ctx);
    QMap<QString, QScriptValue> scriptVars;
    foreach( const Descriptor & key, scriptData.getScriptVars().uniqueKeys() ) {
        assert(!key.getId().isEmpty());
        scriptVars[key.getId()] = engine.newVariant(scriptData.getScriptVars().value(key));
    }

    TaskStateInfo tsi;
    WorkflowScriptLibrary::initEngine(&engine);
    QScriptValue scriptResult = ScriptTask::runScript(&engine, scriptVars, scriptData.getScriptText(), tsi);

    // FIXME: report errors!
    // FIXME: write to log
    if( tsi.cancelFlag ) {
        if( !tsi.hasError() ) {
            tsi.setError("Script task canceled");
        }
    }
    if(tsi.hasError()) {
        scriptLog.error(tsi.getError());
        return QString();
    }
    if( scriptResult.isString() ) {
        return scriptResult.toString();
    }

    return QString();
}

template<>
inline int Attribute::getAttributeValue(Workflow::WorkflowContext *ctx) const {
    if( scriptData.isEmpty() ) {
        return getAttributeValueWithoutScript<int>();
    }

    WorkflowScriptEngine engine(ctx);
    QMap<QString, QScriptValue> scriptVars;
    foreach( const Descriptor & key, scriptData.getScriptVars().uniqueKeys() ) {
        assert(!key.getId().isEmpty());
        scriptVars[key.getId()] = engine.newVariant(scriptData.getScriptVars().value(key));
    }

    TaskStateInfo tsi;
    WorkflowScriptLibrary::initEngine(&engine);
    QScriptValue scriptResult = ScriptTask::runScript(&engine, scriptVars, scriptData.getScriptText(), tsi);

    if( tsi.cancelFlag ) {
        if( !tsi.hasError() ) {
            tsi.setError("Script task canceled");
        }
    }
    if(tsi.hasError()) {
        scriptLog.error(tsi.getError());
        return 0;
    }
    if( scriptResult.isNumber() ) {
        return scriptResult.toInt32();
    }

    return 0;
}

} // U2 namespace

Q_DECLARE_METATYPE(U2::AttributeScript)
Q_DECLARE_OPERATORS_FOR_FLAGS(U2::Attribute::Flags)

#endif
