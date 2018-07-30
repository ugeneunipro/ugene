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

#ifndef _U2_ACTOR_H_
#define _U2_ACTOR_H_

#include <U2Lang/Configuration.h>
#include <U2Lang/Peer.h>
#include <U2Lang/PortMapping.h>

namespace U2 {
namespace Workflow {

class ActorDocument;
class ActorPrototype;
class Port;

class U2LANG_EXPORT ValidatorDesc {
public:
    QString type;
    QMap<QString, QString> options;
};

/**
 * It is a configuration - it has set of named attributes
 * with set of Ports and links between them
 *
 * represents one of ActorPrototypes
 * Peer is needed for saving Worker that is associated with current actor (see LastReadyScheduler::tick())
 */
class U2LANG_EXPORT Actor: public QObject, public Configuration, public Peer {
    Q_OBJECT
public:
    virtual ~Actor();

    ActorPrototype* getProto() const;

    // this id is an actor block name at the *.uwl file
    ActorId getId() const;
    void setId(const ActorId &id);

    QString getLabel() const;
    void setLabel(const QString& l);

    Port* getPort(const QString& id) const;
    QList<Port*> getPorts() const;
    QList<Port*> getInputPorts() const;
    QList<Port*> getOutputPorts() const;

    QList<Port*> getEnabledPorts() const;
    QList<Port*> getEnabledInputPorts() const;
    QList<Port*> getEnabledOutputPorts() const;

    // reimplemented: Configuration::setParameter
    virtual void setParameter(const QString& name, const QVariant& val);

    // NULL if description not set by user
    ActorDocument* getDescription() const;
    void setDescription(ActorDocument* d);

    const QMap<QString, QString> & getParamAliases() const;
    QMap<QString, QString> & getParamAliases();
    bool hasParamAliases() const;

    const QMap<QString, QString> & getAliasHelp() const;
    QMap<QString, QString> & getAliasHelp();
    bool hasAliasHelp() const;

    // reimplemented: Configuration::remap
    virtual void remap(const QMap<ActorId, ActorId>&);

    AttributeScript *getScript() const;
    void setScript(AttributeScript* _script);

    AttributeScript *getCondition() const;

    ActorId getOwner() const;
    void setOwner(const ActorId &owner);

    /**
     * Ids of other actors in the scheme can be changed (after copy-paste, for example).
     * It is needed to update all of ids which are used by this actor
     */
    virtual void updateActorIds(const QMap<ActorId, ActorId> &actorIdsMap);

    void update(const QMap<ActorId, ActorId> &actorsMapping);
    void replaceActor(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings);

    void updateDelegateTags();

    void updateItemsAvailability();
    void updateItemsAvailability(const Attribute* influencingAttribute);

    void addCustomValidator(const ValidatorDesc &desc);
    const QList<ValidatorDesc> & getCustomValidators() const;
    virtual bool validate(ProblemList &problemList) const;

signals:
    void si_labelChanged();
    void si_modified();

protected:
    friend class ActorPrototype;
    Actor(const ActorId &actorId, ActorPrototype* proto, AttributeScript *script);
    Actor(const Actor&);

protected:
    ActorId id;
    // name of actor
    // can be set by user
    // default is: 'proto->getDisplayName() actorId'
    QString label;
    // semantic template of this actor
    ActorPrototype* proto;
    // ports, binded to their names
    QMap<QString,Port*> ports;
    // description of actor
    // has setter and getter
    ActorDocument* doc;
    // user can set aliases for schema parameters and use them in cmdline or in other schemas through includes
    // ( paramName, alias ) pairs
    QMap<QString, QString> paramAliases;
    // (alias, alias help message) pairs
    QMap<QString, QString> aliasHelpDescs;
    // actor can be written on script
    // this object identifies it's text and variables
    AttributeScript * script;
    // condition that is to be evaluated
    // before the actor's execution
    AttributeScript * condition;
    // an actor could be a subactor of some another
    ActorId owner;
    QList<ValidatorDesc> customValidators;

private:
    // setups variables for script
    void setupVariablesForPort(AttributeScript *_script, bool inputOnly = false);
    void setupVariablesForAttribute(AttributeScript *_script);

    void updateGrouperSlots(const QMap<ActorId, ActorId> &actorsMapping);
}; // Actor

} // Workflow
} // U2

#endif // _U2_ACTOR_H_
