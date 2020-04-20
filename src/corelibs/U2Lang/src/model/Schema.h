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

#ifndef _U2_WORKFLOW_ITERATION_H_
#define _U2_WORKFLOW_ITERATION_H_

#include <QFont>
#include <QPair>

#include <U2Lang/Aliasing.h>
#include <U2Lang/Attribute.h>
#include <U2Lang/PortMapping.h>

typedef QPair<U2::ActorId,QString> IterationCfgKey;
typedef QMap<IterationCfgKey, QVariant> IterationCfg;
typedef QMap<U2::ActorId, QVariantMap> CfgMap;

Q_DECLARE_METATYPE(IterationCfg)
Q_DECLARE_METATYPE(CfgMap)

namespace U2 {

class Wizard;

namespace Workflow {

class Actor;
class Link;
class Port;

/**
 * Schema's actors' graph
 * saves with schema to file and loads with it
 */
class U2LANG_EXPORT ActorBindingsGraph {
public:
    ActorBindingsGraph() {}
    virtual ~ActorBindingsGraph() {}

    void clear();
    bool isEmpty() const;

    bool validateGraph(QString &message) const;
    bool addBinding(Port *source, Port *dest);
    bool contains(Port *source, Port *dest) const;
    void removeBinding(Port *source, Port *dest);
    const QMap<Port*, QList<Port*> > & getBindings() const;
    QMap<Port*, QList<Port*> > & getBindings();
    QMap<int, QList<Actor*> > getTopologicalSortedGraph(QList<Actor*> actors) const;
    QList<Link*> getFlows() const;

private:
    QMap<Port*, QList<Port*> > bindings;
}; // ActorBindingsGraph

/**
 * Schema is oriented graph of actors
 * graph is oriented because Link has orientation
 */
class U2LANG_EXPORT Schema {
public:
    Schema();
    virtual ~Schema();
    Schema( const Schema & other );
    Schema & operator=( const Schema & other );

    void reset();
    void update();
    /** The actors' aliases of @other can be removed in case of duplication */
    void merge(Schema &other);

    Actor* actorById(ActorId) const;
    QList<Actor*> actorsByOwnerId(ActorId) const;

    QString getDomain() const;
    void setDomain(const QString & d);

    const ActorBindingsGraph & getActorBindingsGraph() const;

    const QList<Actor*> & getProcesses() const;
    void addProcess(Actor *actor);
    void removeProcess(Actor *actor);
    void renameProcess(const ActorId &oldId, const ActorId &newId);
    void replaceProcess(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings);

    void applyConfiguration(const QMap<ActorId, QVariantMap> &cfg);

    QList<Link*> getFlows() const;
    void addFlow(Link* l);
    void removeFlow(Link* l);

    void setDeepCopyFlag(bool flag);

    bool hasParamAliases() const;
    bool hasAliasHelp() const;
    bool hasPortAliases() const;
    const QList<PortAlias> &getPortAliases() const;
    bool addPortAlias(const PortAlias &alias);
    void setPortAliases(const QList<PortAlias> &aliases);

    // replaces dummy processes (schema-processes) by his schemas and set up correct links
    bool expand();

    QString getTypeName() const;
    void setTypeName(const QString &typeName);

    const QList<Wizard*> & getWizards() const;
    void setWizards(const QList<Wizard*> &value);
    QList<Wizard*> takeWizards();

    static ActorId uniqueActorId(const QString &id, const QList<Actor*> &procs);

private:
    // set of actors
    QList<Actor*> procs;
    // name of domain in which we work now
    // default is LocalDomainFactory::ID
    QString domain;
    // true if schema was deeply copied
    // this means that every actor, link and iteration was copied
    // if true -> need to delete all corresponding data
    bool deepCopy;
    // keeps how ports are visually connected (it often repeats flows)
    ActorBindingsGraph graph;
    // keeps new names of ports (and inner slots) for includes
    QList<PortAlias> portAliases;
    // if you include this schema to another schema then here is new type name
    QString includedTypeName;
    QList<Wizard*> wizards;

private:
    void setAliasedAttributes(Actor *proc, Actor *subProc);
    void replaceInLinksAndSlots(Actor *proc, const PortAlias &portAlias);
    void replaceOutLinks(Actor *proc, const PortAlias &portAlias);
    void replaceOutSlots(Actor *proc, const PortAlias &portAlias);
    void replacePortAliases(const PortAlias &subPortAlias);

    bool recursiveExpand(QList<QString> &schemaIds);
    void update(const QMap<ActorId, ActorId> &actorsMapping);

}; // Schema

class U2LANG_EXPORT ActorVisualData {
public:
    ActorVisualData();
    ActorVisualData(const ActorId &actorId);
    ActorId getActorId() const;
    void setActorId(const ActorId &value);

    QPointF getPos(bool &contains) const;
    QString getStyle(bool &contains) const;
    QColor getColor(bool &contains) const;
    QFont getFont(bool &contains) const;
    QRectF getRect(bool &contains) const;
    qreal getPortAngle(const QString &portId, bool &contains) const;

    void setPos(const QPointF &value);
    void setStyle(const QString &value);
    void setColor(const QColor &value);
    void setFont(const QFont &value);
    void setRect(const QRectF &value);
    void setPortAngle(const QString &portId, qreal value);

    QMap<QString, qreal> getAngleMap() const;

private:
    ActorId actorId;

    QPointF pos; bool posInited;
    QString styleId; bool styleInited;
    QColor color; bool colorInited;
    QFont font; bool fontInited;
    QRectF rect; bool rectInited;
    QMap<QString, qreal> angleMap;

private:
    void initialize();
};

/**
 * Schema's metadata
 * saves with schema to file and loads with it
 *
 * Schema don't aggregate metadata (see WorkflowViewController for usage)
 */
class U2LANG_EXPORT Metadata {
public:
    Metadata();

    void reset();
    void resetVisual();
    void mergeVisual(const Metadata &other);

    ActorVisualData getActorVisualData(const ActorId &actorId, bool &contains) const;
    void setActorVisualData(const ActorVisualData &data);

    QPointF getTextPos(const ActorId &srcActorId, const QString &srcPortId,
        const ActorId &dstActorId, const QString &dstPortId, bool &contains) const;

    void setTextPos(const ActorId &srcActorId, const QString &srcPortId,
        const ActorId &dstActorId, const QString &dstPortId,
        const QPointF &value);

    void removeActorMeta(const ActorId &actorId);
    void renameActors(const QMap<ActorId, ActorId> &actorsMapping);
    void replaceProcess(const ActorId &oldId, const ActorId &newId, const QList<PortMapping> &mappings);

    QList<ActorVisualData> getActorsVisual() const;
    QMap<QString, QPointF> getTextPosMap() const;

    void setSampleMark(bool isSample);
    bool isSample() const;

public:
    QString name;
    QString url;
    QString comment;
    int scalePercent;
    bool isSampleWorkflow;
    // ECMAScript code of schema run estimations
    QString estimationsCode;

private:
    QMap<ActorId, ActorVisualData> actorVisual;
    QMap<QString, QPointF> textPosMap; // actorId.portId->actorId.portId <-> textPos

private:
    QString getPortString(const ActorId &actorId, const QString &portId) const;
    ActorId getActorId(const QString &portStr) const;
    QString getLinkString(const ActorId &srcActorId, const QString &srcPortId,
        const ActorId &dstActorId, const QString &dstPortId) const;
    bool isActorLinked(const ActorId &actorId, const QString &linkStr) const;
    QString renameLink(const QString &linkStr, const QMap<ActorId, ActorId> &actorsMapping) const;
    QString renameLink(const QString &linkStr, const ActorId &oldId, const ActorId &newId, const QList<PortMapping> &mappings) const;
}; // Metadata

}//Workflow namespace

}//GB2 namespace

#endif // _U2_WORKFLOW_ITERATION_H_
