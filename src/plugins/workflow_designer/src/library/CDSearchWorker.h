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

#pragma once

#include <U2Algorithm/CDSearchTaskFactory.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class CDSearchPrompter;
typedef PrompterBase<CDSearchPrompter> CDSearchPrompterBase;

class CDSearchPrompter : public CDSearchPrompterBase {
    Q_OBJECT
public:
    CDSearchPrompter(Actor* p = 0)
        : CDSearchPrompterBase(p) {
    }

protected:
    QString composeRichDoc() override;
};

class CDSearchWorker : public BaseWorker {
    Q_OBJECT
public:
    CDSearchWorker(Actor* a)
        : BaseWorker(a), input(nullptr), output(nullptr), cds(nullptr) {
    }
    void init() override;
    Task* tick() override;
    void cleanup() override {};
private slots:
    void sl_taskFinished(Task*);

protected:
    IntegralBus *input, *output;
    CDSearchSettings settings;
    CDSearchResultListener* cds;
};

class CDSearchWorkerFactory : public DomainFactory {
    static const QString ACTOR_ID;

public:
    static void init();
    CDSearchWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) override {
        return new CDSearchWorker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2
