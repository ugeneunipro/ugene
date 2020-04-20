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

#ifndef _MSA_2_SEQUENCE_WORKER_H_
#define _MSA_2_SEQUENCE_WORKER_H_

#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class Alignment2SequencePrompter : public PrompterBase<Alignment2SequencePrompter> {
    Q_OBJECT
public:
    Alignment2SequencePrompter(Actor * p = NULL) : PrompterBase<Alignment2SequencePrompter>(p) {}

protected:
    QString composeRichDoc();

}; // Text2SequencePrompter

class Alignment2SequenceWorker : public BaseWorker {
    Q_OBJECT
public:
    static QMap<QString, QString> cuteAlIdNames;

public:
    Alignment2SequenceWorker(Actor * p) : BaseWorker(p), input(NULL), output(NULL) {}

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

private:
    IntegralBus * input;
    IntegralBus * output;

}; // Text2SequenceWorker

class Alignment2SequenceWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    Alignment2SequenceWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker * createWorker(Actor* a);

}; // Text2SequenceWorkerFactory

} // LocalWorkflow
} // U2

#endif
