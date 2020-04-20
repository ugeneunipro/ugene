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

#ifndef _MARK_SEQUENCE_WORKER_H_
#define _MARK_SEQUENCE_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class MarkSequencePrompter : public PrompterBase<MarkSequencePrompter> {
    Q_OBJECT
public:
    MarkSequencePrompter(Actor *p = NULL) : PrompterBase<MarkSequencePrompter>(p) {}

protected:
    QString composeRichDoc();

}; // MarkSequencePrompter

class MarkSequenceWorker : public BaseWorker {
    Q_OBJECT
public:
    static QMap<QString, QString> cuteAlIdNames;

public:
    MarkSequenceWorker(Actor *p);

    virtual void init();
    virtual Task *tick();
    virtual void cleanup() {}

private:
    IntegralBus *inChannel;
    IntegralBus *outChannel;
    DataTypePtr mtype;

}; // MarkSequenceWorker

class MarkSequenceWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    MarkSequenceWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // MarkSequenceWorkerFactory

} // LocalWorkflow
} // U2

#endif // _MARK_SEQUENCE_WORKER_H_
