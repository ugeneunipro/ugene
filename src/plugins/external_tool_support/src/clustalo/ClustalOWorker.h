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

#include "ClustalOSupportTask.h"

namespace U2 {

namespace LocalWorkflow {

class ClustalOPrompter : public PrompterBase<ClustalOPrompter> {
    Q_OBJECT
public:
    ClustalOPrompter(Actor* actor = nullptr);

protected:
    QString composeRichDoc() override;
};

class ClustalOWorker : public BaseWorker {
    Q_OBJECT
public:
    ClustalOWorker(Actor* a);

    void init() override;

    Task* tick() override;

    void cleanup() override;

private slots:
    void sl_taskFinished();

protected:
    IntegralBus *input, *output;
    QString resultName, transId;
    ClustalOSupportTaskSettings cfg;
};

class ClustalOWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    static void init();

    ClustalOWorkerFactory();

    Worker* createWorker(Actor* actor) override;
};

}  // namespace LocalWorkflow
}  // namespace U2
