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

#ifndef _U2_WORKFLOW_INTEGRAL_BUS_H_
#define _U2_WORKFLOW_INTEGRAL_BUS_H_

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowTransport.h>
#include <QMutex>

namespace U2 {

namespace Workflow {

/**
 * Keeps the type of the bus.
 * Helps to convert messages for actors.
 */
class U2LANG_EXPORT BusMap {
public:
    BusMap(const StrStrMap &busMap, const QMap<QString, QStringList> &listMap, const SlotPathMap &paths);
    BusMap(const StrStrMap &busMap, bool breaksDataflow, const QString &actorId);

    Message takeMessageMap(CommunicationChannel *ch, QVariantMap &context);
    Message lookMessageMap(CommunicationChannel *ch);
    QVariantMap composeMessageMap(const Message &m, const QVariantMap &context);

    static void parseSource(const QString &src, QString &srcId, QStringList &path);
    static QString getNewSourceId(const QString &srcId, const QString &actorId);

private:
    bool input;

    StrStrMap busMap;
    QMap<QString, QStringList> listMap;
    SlotPathMap paths;

    bool breaksDataflow;
    QString actorId;

private:
    QVariantMap getMessageData(const Message &m) const;
};

/**
 * represents communication channel for support passing data between actors
 * connected in transitive closure of schema graph
 *
 * is a container of communications with other actors
 */
class U2LANG_EXPORT IntegralBus : public QObject, public CommunicationSubject, public CommunicationChannel {
    Q_OBJECT
public:
    IntegralBus(Port* peer);
    ~IntegralBus();

    // reimplemented from CommunicationSubject
    virtual bool addCommunication(const QString& id, CommunicationChannel* ch);
    virtual CommunicationChannel* getCommunication(const QString& id);

    void putWithoutContext(const Message& m);

    // reimplemented from CommunicationChannel
    virtual Message get();
    virtual Message look() const;
    virtual void put(const Message& m, bool isMessageRestored = false);
    // put incoming context to the output channels
    virtual void transit();
    virtual int hasMessage() const;
    virtual int takenMessages() const;
    virtual int hasRoom(const DataType* t = NULL) const;
    virtual bool isEnded() const;
    virtual void setEnded();
    virtual int capacity() const {return 1;}
    virtual void setCapacity(int) {}
    virtual Message lookMessage() const;
    virtual QQueue<Message> getMessages(int startIndex, int endIndex) const;

    QVariantMap getContext() const {return context;}
    QVariantMap getLastMessageContext() const {return lastMessageContext;}
    void setContext(const QVariantMap& m, int metadataId);
    int getContextMetadataId() const;

    virtual void addComplement(IntegralBus* b) {assert(!complement);complement = b;}

    QString getPortId() const {return portId;}
    DataTypePtr getBusType() const {return busType;}

    void setPrintSlots(bool in, const QList<QString> &printSlots);

    void setWorkflowContext(WorkflowContext *context);

protected:
    virtual Message composeMessage(const Message&);

protected:
    // type of port integral bus is binded to
    DataTypePtr busType;
    // communications with other ports
    QMap<QString, CommunicationChannel*> outerChannels;
    // busmap of port integral bus is binded to
    BusMap *busMap;
    // context of an output message. See put() for details
    QVariantMap context;
    QVariantMap lastMessageContext;
    int contextMetadataId;
    //
    IntegralBus* complement;
    // integral bus is binded to port with this id
    QString portId;
    //
    int takenMsgs;

    // a content of these slots is printed to the standart output
    QList<QString> printSlots;
    ActorId actorId;
    WorkflowContext *workflowContext;

    QMutex *contextMutex;

}; // IntegralBus

}//Workflow namespace

}//GB2 namespace

#endif // _U2_WORKFLOW_INTEGRAL_BUS_H_
