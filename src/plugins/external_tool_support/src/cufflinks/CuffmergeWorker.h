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

#ifndef _U2_CUFFMERGE_WORKER_H
#define _U2_CUFFMERGE_WORKER_H

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "CuffmergeSupportTask.h"

namespace U2 {
namespace LocalWorkflow {


class CuffmergePrompter : public PrompterBase<CuffmergePrompter>
{
    Q_OBJECT

public:
    CuffmergePrompter(Actor* parent = 0);

protected:
    QString composeRichDoc();
};


class CuffmergeWorker : public BaseWorker
{
    Q_OBJECT

public:
    CuffmergeWorker(Actor* actor);

    void init();
    Task * tick();
    void cleanup();

private slots:
    void sl_taskFinished();

protected:
    IntegralBus *input;
    IntegralBus *output;

    QList<SharedDbiDataHandler> annTableHandlers;

private:
    CuffmergeSettings scanParameters() const;
    Task * createCuffmergeTask();
    void takeAnnotations();
};


class CuffmergeWorkerFactory : public DomainFactory
{
public:
    static const QString ACTOR_ID;
    static void init();
    CuffmergeWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* actor) { return new CuffmergeWorker(actor); }
};

} // namespace LocalWorkflow
} // namespace U2

#endif
