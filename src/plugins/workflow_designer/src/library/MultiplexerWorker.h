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

#ifndef _MULTIPLEXER_WORKER_H_
#define _MULTIPLEXER_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

enum MultiplexingRules {
    ONE_TO_MANY,
    ONE_TO_ONE,
};

class MultiplexerPrompter : public PrompterBase<MultiplexerPrompter> {
    Q_OBJECT
public:
    MultiplexerPrompter(Actor *p = NULL) : PrompterBase<MultiplexerPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // MultiplexerPrompter

class MultiplexerWorker : public BaseWorker {
    Q_OBJECT
public:
    MultiplexerWorker(Actor *p);

    virtual void init();
    virtual bool isReady() const;
    virtual Task *tick();
    virtual void cleanup();

private:
    IntegralBus *inChannel1;
    IntegralBus *inChannel2;
    IntegralBus *outChannel;

    uint rule;
    bool hasMultiData;
    QVariantMap multiData;
    int multiMetadataId;

    bool messagesInited;
    QList<QVariantMap> messages;

    void shutDown();
    bool checkIfEnded();
    bool hasDataFotMultiplexing() const;
    void sendUnitedMessage(const QVariantMap &m1, QVariantMap &m2, int metadataId);
    void multiplexManyMode();
    QString getInputActorName(IntegralBus *bus) const;
    QString getMessagesMismatchError() const;

}; // MultiplexerWorker

class MultiplexerWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    MultiplexerWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // MultiplexerWorkerFactory

} // LocalWorkflow
} // U2

#endif // _MULTIPLEXER_WORKER_H_
