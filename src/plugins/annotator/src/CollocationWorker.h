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

#ifndef _U2_ANNOTATOR_WORKER_H_
#define _U2_ANNOTATOR_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "CollocationsSearchAlgorithm.h"

namespace U2 {

namespace LocalWorkflow {

class CollocationPrompter;
typedef PrompterBase<CollocationPrompter> CollocationPrompterBase;

class CollocationPrompter : public CollocationPrompterBase {
    Q_OBJECT
public:
    CollocationPrompter(Actor* p = 0) : CollocationPrompterBase(p) {}
    virtual ~CollocationPrompter() {}
protected:
    QString composeRichDoc();
};

class CollocationWorker : public BaseWorker {
    Q_OBJECT
public:
    CollocationWorker(Actor* a) : BaseWorker(a), input(NULL), output(NULL) {}
    virtual ~CollocationWorker() {}
    virtual void init();
    virtual Task* tick();
    virtual void cleanup() {}
private slots:
    void sl_taskFinished();

protected:
    IntegralBus *input, *output;
    CollocationsAlgorithmSettings cfg;
};

class CollocationWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    CollocationWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual ~CollocationWorkerFactory() {}
    virtual Worker* createWorker(Actor* a) {return new CollocationWorker(a);}
};

}//Workflow namespace
}//U2 namespace

#endif
