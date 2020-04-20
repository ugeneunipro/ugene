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

#ifndef _FILTER_SEQUENCE_WORKER_H_
#define _FILTER_SEQUENCE_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class PassFilterPrompter : public PrompterBase<PassFilterPrompter> {
    Q_OBJECT
public:
    PassFilterPrompter(Actor *p = NULL) : PrompterBase<PassFilterPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // PassFilterPrompter

class PassFilterWorker : public BaseWorker {
    Q_OBJECT
public:
    PassFilterWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup();

private:
    CommunicationChannel *inChannel;
    CommunicationChannel *outChannel;
    DataTypePtr mtype;
    QStringList passedValues;

}; // PassFilterWorker

class PassFilterWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    PassFilterWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // PassFilterWorkerFactory

} // LocalWorkflow
} // U2

#endif // _FILTER_SEQUENCE_WORKER_H_
