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

#include <QMutex>

#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowTransport.h>

namespace U2 {

namespace Workflow {

/**
 * Keeps the type of the bus.
 * Helps to convert messages for actors.
 */
class U2LANG_EXPORT BusMap {
public:
    BusMap(const StrStrMap& busMap, const QMap<QString, QStringList>& listMap, const SlotPathMap& paths);
    BusMap(const StrStrMap& busMap, bool breaksDataflow, const QString& actorId);

    Message takeMessageMap(CommunicationChannel* ch, QVariantMap& context);
    Message lookMessageMap(CommunicationChannel* ch);
    QVariantMap composeMessageMap(const Message& m, const QVariantMap& context);

    static void parseSource(const QString& src, QString& srcId, QStringList& path);
    static QString getNewSourceId(const QString& srcId, const QString& actorId);

private:
    bool input;

    StrStrMap busMap;
    QMap<QString, QStringList> listMap;
    SlotPathMap paths;

    bool breaksDataflow;
    QString actorId;

private:
    QVariantMap getMessageData(const Message& m) const;
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
    bool addCommunication(const QString& id, CommunicationChannel* ch) override;
    CommunicationChannel* getCommunication(const QString& id) override;

    void putWithoutContext(const Message& m);

    // reimplemented from CommunicationChannel
    Message get() override;
    Message look() const override;
    void put(const Message& m, bool isMessageRestored = false) override;
    // put incoming context to the output channels
    virtual void transit();
    int hasMessage() const override;
    int takenMessages() const override;
    int hasRoom(const DataType* t = nullptr) const override;
    bool isEnded() const override;
    void setEnded() override;
    int capacity() const override {
        return 1;
    }
    void setCapacity(int) override {
    }
    virtual Message lookMessage() const;
    QQueue<Message> getMessages(int startIndex, int endIndex) const override;

    QVariantMap getContext() const {
        return context;
    }
    QVariantMap getLastMessageContext() const {
        return lastMessageContext;
    }
    void setContext(const QVariantMap& m, int metadataId);
    int getContextMetadataId() const;

    virtual void addComplement(IntegralBus* b) {
        assert(!complement);
        complement = b;
    }

    QString getPortId() const {
        return portId;
    }
    DataTypePtr getBusType() const {
        return busType;
    }

    void setPrintSlots(bool in, const QList<QString>& printSlots);

    void setWorkflowContext(WorkflowContext* context);

protected:
    virtual Message composeMessage(const Message&);

protected:
    // type of port integral bus is binded to
    DataTypePtr busType;
    // communications with other ports
    QMap<QString, CommunicationChannel*> outerChannels;
    // busmap of port integral bus is binded to
    BusMap* busMap;
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
    WorkflowContext* workflowContext;

    QMutex* contextMutex;

};  // IntegralBus

}  // namespace Workflow

}  // namespace U2
