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

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>

#include "CuffmergeSupportTask.h"

namespace U2 {
namespace LocalWorkflow {

class CuffmergePrompter : public PrompterBase<CuffmergePrompter> {
    Q_OBJECT

public:
    CuffmergePrompter(Actor* parent = 0);

protected:
    QString composeRichDoc() override;
};

class CuffmergeWorker : public BaseWorker {
    Q_OBJECT

public:
    CuffmergeWorker(Actor* actor);

    void init() override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_taskFinished();

protected:
    IntegralBus* input;
    IntegralBus* output;

    QList<SharedDbiDataHandler> annTableHandlers;

private:
    Task* createCuffmergeTask();
    void takeAnnotations();
};

class CuffmergeWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    CuffmergeWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* actor) override {
        return new CuffmergeWorker(actor);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2
