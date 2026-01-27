/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <limits.h>

#include <QQueue>

#include <U2Core/ExternalToolRunTask.h>

#include <U2Lang/IntegralBus.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowManager.h>
#include <U2Lang/WorkflowTransport.h>

namespace U2 {

namespace LocalWorkflow {

using namespace Workflow;

/**
 * currently, all wd workers inherits this class
 *
 * base class for workers in integral bus model
 */
class U2LANG_EXPORT BaseWorker : public QObject, public Worker, public CommunicationSubject {
    Q_OBJECT
public:
    using Workflow::Worker::tick;

    BaseWorker(Actor* a, bool autoTransitBus = true);
    ~BaseWorker() override;

    virtual ActorId getActorId() const;

    virtual void setDone();

    // reimplemented from Worker
    bool isDone() const override;
    bool isReady() const override;

    // reimplemented from CommunicationSubject
    bool addCommunication(const QString& name, CommunicationChannel* _ch) override;
    CommunicationChannel* getCommunication(const QString& name) override;
    virtual QStringList getOutputFiles();

    // if you want your worker support scripts -> you should call this function to get Messages from channels
    // call this when channel has message
    // after calling: set all needed values for running your worker
    // called from 'tick' and then setup worker params
    virtual Message getMessageAndSetupScriptValues(CommunicationChannel* channel);

    QMap<QString, IntegralBus*>& getPorts() {
        return ports;
    }
    Actor* getActor() const {
        return actor;
    }

    void deleteBackupMessagesFromPreviousTick() {
        messagesProcessedOnLastTick.clear();
    }
    // replace current worker's messages in channels with ones from previous tick
    void saveCurrentChannelsStateAndRestorePrevious();
    // fill channels with messages which were existed before the pause
    void restoreActualChannelsState();
    // method should be invoked by scheduler instead of tick().
    // parameter canBeCanceled indicates if the returning task can be canceled
    // in case of the schema's pause
    Task* tick(bool& canResultBeCanceled);

    QList<ExternalToolListener*> createLogListeners(int listenersNumber = 1) const;

private:
    // bind values from input ports to script vars.
    // This function is called before 'get' data from channel -> to set up parameters for scripting
    void bindScriptValues();
    void setScriptVariableFromBus(AttributeScript* script, IntegralBus* bus);
    bool processDone;

    // this field contains all messages that Worker processes during one tick.
    // New messages are added here in getMessageAndSetupScriptValues(CommunicationChannel *) method.
    // Also during the pause state it contains backup messages
    // (which were existed at the moment before the pause) from the same ports
    QMap<CommunicationChannel*, QQueue<Message>> messagesProcessedOnLastTick;

    // puts all messages from messagesProcessedOnLastTick to appropriate channel
    // which is actually key for the queue of messages
    void addMessagesFromBackupToAppropriratePort(CommunicationChannel* channel);

protected:
    Actor* actor;
    // integral buses of actor's ports
    QMap<QString, IntegralBus*> ports;

    // default implementation always return false
    // TODO: check all workers' task and override this method
    // in order to cancel tasks on debug events (e.g. pause, breakpoint, etc)
    virtual bool canTaskBeCanceled(Task* workerTask) const;

    /** Returns the value of a parameter with paramId */
    template<class T>
    T getValue(const QString& paramId) const;

    WorkflowMonitor* monitor() const;
    void reportError(const QString& message);
};  // BaseWorker

/**
 * simple realization of Communnication channel
 */
class U2LANG_EXPORT SimpleQueue : public CommunicationChannel {
public:
    SimpleQueue();

    // reimplemented from CommunicationChannel
    Message get() override;
    Message look() const override;
    void put(const Message& m, bool isMessageRestored = false) override;
    int hasMessage() const override;
    int takenMessages() const override;
    int hasRoom(const DataType*) const override;
    bool isEnded() const override;
    void setEnded() override;
    // capacity is INT_MAX
    int capacity() const override;
    // does nothing
    void setCapacity(int) override;
    QQueue<Message> getMessages(int startIndex = 0, int endIndex = -1) const override;

protected:
    // first in, first out
    QQueue<Message> que;
    // 'end' flag
    bool ended;
    //
    int takenMsgs;

};  // SimpleQueue

/**
 * runtime domain for SimplestSequentialScheduler and SimpleQueue
 *
 * currently, container for all DomainFactories of computational tasks
 */
class U2LANG_EXPORT LocalDomainFactory : public DomainFactory {
public:
    static const QString ID;

public:
    LocalDomainFactory();

    Worker* createWorker(Actor*) override;
    CommunicationChannel* createConnection(Link*) override;
    Scheduler* createScheduler(Schema*) override;
    void destroy(Scheduler*, Schema*) override;

};  // LocalDomainFactory

/************************************************************************/
/* Template definitions */
/************************************************************************/
template<class T>
T BaseWorker::getValue(const QString& paramId) const {
    Attribute* attr = actor->getParameter(paramId);
    if (attr == nullptr) {
        return T();
    }
    return attr->getAttributeValue<T>(context);
}

template<>
inline QString BaseWorker::getValue(const QString& paramId) const {
    Attribute* attr = actor->getParameter(paramId);
    if (attr == nullptr) {
        return "";
    }
    QString value = attr->getAttributeValue<QString>(context);
    bool dir = false;
    if (RFSUtils::isOutUrlAttribute(attr, actor, dir)) {
        return context->absolutePath(value);
    }
    return value;
}

}  // namespace LocalWorkflow

}  // namespace U2
