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

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "KalignSupportTask.h"

namespace U2 {

namespace LocalWorkflow {

class KalignPrompter : public PrompterBase<KalignPrompter> {
    Q_OBJECT
public:
    KalignPrompter(Actor* p = 0)
        : PrompterBase<KalignPrompter>(p) {
    }

protected:
    QString composeRichDoc() override;
};

class KalignWorker : public BaseWorker {
    Q_OBJECT
public:
    KalignWorker(Actor* a);

    void init() override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_taskFinished();

private:
    IntegralBus *input, *output;
    QString resultName, transId;
    KalignSupportTaskSettings cfg;

private:
    void send(const MultipleSequenceAlignment& msa);
};

class KalignWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    KalignWorkerFactory();
    Worker* createWorker(Actor* a) override;
};

}  // namespace LocalWorkflow
}  // namespace U2
