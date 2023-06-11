/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_KALIGN_WORKER_H_
#define _U2_KALIGN_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "KalignTask.h"

namespace U2 {

namespace LocalWorkflow {

class Kalign2Prompter : public PrompterBase<Kalign2Prompter> {
    Q_OBJECT
public:
    Kalign2Prompter(Actor* p = 0)
        : PrompterBase<Kalign2Prompter>(p) {
    }

protected:
    QString composeRichDoc();
};

class Kalign2Worker : public BaseWorker {
    Q_OBJECT
public:
    Kalign2Worker(Actor* a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

private slots:
    void sl_taskFinished();

private:
    IntegralBus *input, *output;
    QString resultName, transId;
    Kalign2TaskSettings cfg;

private:
    void send(const MultipleSequenceAlignment& msa);
};

class Kalign2WorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    Kalign2WorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    virtual Worker* createWorker(Actor* a) {
        return new Kalign2Worker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif
