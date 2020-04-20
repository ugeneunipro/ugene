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

#ifndef _U2_WORKFLOW_BUS_MODEL_H_
#define _U2_WORKFLOW_BUS_MODEL_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/Datatype.h>

// destSlotId <-> sourceActorId:sourceSlotId
typedef QPair<QString, QString> SlotPair;

// connected slots <-> connection path (actorId1, actorId2, ...)
typedef QMap<SlotPair, QStringList> SlotPathMap;

namespace U2 {
namespace Workflow {

/**
 * To support passing data that is not produced by linked actor-neighbour
 * but produced by actor connected in transitive closure of schema graph
 * information of such data is saved in busmap - StrStr map
 * saved as attribute with BUS_MAP_ATTR_ID id
 *
 * in such cases port need to know who will produce needed data
 * thats why we need new Port
 *
 */
class U2LANG_EXPORT IntegralBusPort : public Port {
    Q_OBJECT
public:
    // attribute for busmap
    static const QString BUS_MAP_ATTR_ID;
    static const QString PATHS_ATTR_ID;
    static const uint BLIND_INPUT = 1 << 16;

public:
    IntegralBusPort(const PortDescriptor& d, Actor* p);

    virtual DataTypePtr getType() const;
    DataTypePtr getOwnType() const {return type;}

    // slot is list pairs (actorId:attrId);(actorId:attrId);(actorId:attrId)...
    Actor* getProducer(const QString& slot);
    QList<Actor*> getProducers(const QString& slot);
    Actor* getLinkedActorById(ActorId id) const;
    SlotPathMap getPaths() const;
    QList<QStringList> getPathsBySlotsPair(const QString& dest, const QString& src) const;
    void setPathsBySlotsPair(const QString& dest, const QString& src, const QList<QStringList> &paths);
    void addPathBySlotsPair(const QString& dest, const QString& src, const QStringList &path);
    void clearPaths();

    // find matching data and assign it
    void setupBusMap();

    virtual bool validate(NotificationsList& notificationList) const;
    // reimplemented from Configuration
    virtual void remap(const QMap<ActorId, ActorId>&);
    virtual void updateBindings(const QMap<ActorId, ActorId> &actorsMapping);
    virtual void replaceActor(Actor *oldActor, Actor *newActor, const QList<PortMapping> &mappings);
    virtual void setVisibleSlot(const QString& slotId, const bool isVisible);

    // used when loading schema
    void setBusMapValue(const QString & slotId, const QString & value);

    /** input ports only */
    void copyInput(IntegralBusPort *port, const PortMapping &mapping);

protected:
    virtual DataTypePtr getBusType() const;
    //bool getNearestData(const Descriptor & key, QStringList candidates);
    //
    mutable bool recursing;

private:
    void removeBusMapKey(const QString& slotId);
    void restoreBusMapKey(const QString& slotId);
    StrStrMap getBusMap() const;
    SlotPathMap getPathsMap() const;

    // links with invisible slots
    StrStrMap removedBusMap;
}; // IntegralBusPort

/**
 * as if ActorPrototype creates ports for actor
 * we need special actor prototype for IntegralBusPort
 */
class U2LANG_EXPORT IntegralBusActorPrototype : public ActorPrototype {
public:
    IntegralBusActorPrototype(const Descriptor& desc,
        const QList<PortDescriptor*>& ports = QList<PortDescriptor*>(),
        const QList<Attribute*>& attrs = QList<Attribute*>()) : ActorPrototype(desc, ports, attrs) {}

    virtual Port* createPort(const PortDescriptor& d, Actor* p) {return new IntegralBusPort(d, p);}

}; // IntegralBusActorPrototype

/**
 * validates if binding in busmap to some actor's data is correct
 */
class U2LANG_EXPORT ScreenedSlotValidator : public ConfigurationValidator {
public:
    ScreenedSlotValidator(const QString& slot): screenedSlots(slot) {}
    ScreenedSlotValidator(const QStringList& slotList): screenedSlots(slotList) {}
    static bool validate(const QStringList& screenedSlots, const IntegralBusPort*, NotificationsList &notificationList);
    virtual bool validate(const Configuration*, NotificationsList& notificationList) const;

protected:
    QStringList screenedSlots;

}; // ScreenedSlotValidator

/**
 * When attribute can be set by user or can be chosen from integral bus
 * this validator checks that only one must be chosen
 */
class U2LANG_EXPORT ScreenedParamValidator : public ConfigurationValidator {
public:
    ScreenedParamValidator(const QString& id, const QString& port, const QString& slot);
    virtual ~ScreenedParamValidator() {}

    virtual bool validate(const Configuration*, NotificationsList& notificationList) const;
    QString validate(const Configuration * cfg) const;

    QString getId() const {return id;}
    QString getPort() const {return port;}
    QString getSlot() const {return slot;}

protected:
    QString id;
    QString port;
    QString slot;

}; // ScreenedParamValidator

class U2LANG_EXPORT IntegralBusSlot {
public:
    IntegralBusSlot();
    IntegralBusSlot(const QString &slotId, const QString &portId, const ActorId &actorId);

    QString getId() const;
    QString portId() const;
    ActorId actorId() const;

    void replaceActorId(const ActorId &oldId, const ActorId &newId);

    QString toString() const;
    static QString listToString(const QList<IntegralBusSlot> &slotList);

    static IntegralBusSlot fromString(const QString &slotString, U2OpStatus &os);
    static QList<IntegralBusSlot> listFromString(const QString &slotsString, U2OpStatus &os);

    bool operator==(const IntegralBusSlot&) const;

private:
    QString id;
    QString port;
    ActorId actor;

    static const QString SLOTS_SEP;
    static const QString INNER_SEP;
};

class U2LANG_EXPORT PortValidator : public ConfigurationValidator {
public:
    virtual bool validate(const Configuration *cfg, NotificationsList &notificationList) const;
    virtual bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const = 0;

public:
    static StrStrMap getBusMap(const IntegralBusPort *port);
    static QString slotName(const IntegralBusPort *port, const QString &slotId);
    static bool isBinded(const IntegralBusPort *port, const QString &slotId);
    static bool isBinded(const StrStrMap &busMap, const QString &slotId);
};

}//namespace Workflow
}//namespace U2

typedef QMap<QString, QString> StrStrMap;
Q_DECLARE_METATYPE(StrStrMap)
Q_DECLARE_METATYPE(SlotPathMap)

#endif
