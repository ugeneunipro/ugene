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

#ifndef _U2_WORKFLOW_TRANSPORT_H_
#define _U2_WORKFLOW_TRANSPORT_H_

#include <QVariant>
#include <QQueue>

#include <U2Lang/Datatype.h>

namespace U2 {

namespace Workflow {

/**
 * Message is entity for data that is passed between actors in runtime
 */
class U2LANG_EXPORT Message {
public:
    Message(DataTypePtr t, const QVariant &d, int metadataId = -1);

    DataTypePtr getType() const;
    QVariant getData() const;
    bool isEmpty() const;
    int getMetadataId() const;

    static Message getEmptyMapMessage();

private:
    // type of data
    DataTypePtr t;
    // data itself
    QVariant data;
    // message metadata identifier
    int metadataId;
}; // Message

/**
 * pure virtual controller of passing/receiving messages
 */
class U2LANG_EXPORT CommunicationChannel {
public:
    virtual ~CommunicationChannel() {}

    // take message from channel
    virtual Message get() = 0;
    // look at message without getting it out of channel
    // Message is united of data that was putted to outer channels
    // used in scripting
    virtual Message look() const = 0;
    // after calling message is in channel until get() invocation.
    // isMessageRestored should be true for messages which are queued in channel repeatedly
    // since they weren't processed
    virtual void put(const Message& m, bool isMessageRestored = false) = 0;
    // how many messages in channel
    virtual int hasMessage() const = 0;
    // how many messages taken from channel
    virtual int takenMessages() const = 0;
    //
    virtual int hasRoom(const DataType* t = NULL) const = 0;
    // user can set 'ended' flag to channel
    // it means that no other data will be supplied to it
    virtual bool isEnded() const = 0;
    virtual void setEnded() = 0;

    // how many messages can be put to channel
    virtual int capacity() const = 0;
    virtual void setCapacity(int) = 0;
    // get messages dump (this is intended to be used in WD debug purposes);
    // messages shouldn't be removed from channel;
    // indices should be treated inclusively;
    // default indices values should be used for obtaining all the messages in channel
    virtual QQueue<Message> getMessages(int startIndex = 0, int endIndex = -1) const = 0;

}; // CommunicationChannel

/**
 * represents entity that communicate with others by CommunicationChannel
 * moreover, it binds channels to port identifiers
 */
class U2LANG_EXPORT CommunicationSubject {
public:
    virtual ~CommunicationSubject() {}

    virtual bool addCommunication(const QString& portId, CommunicationChannel* ch) = 0;
    virtual CommunicationChannel* getCommunication(const QString& portId) = 0;

}; // CommunicationSubject

}//Workflow namespace

}//GB2 namespace

#endif // _U2_WORKFLOW_TRANSPORT_H_
