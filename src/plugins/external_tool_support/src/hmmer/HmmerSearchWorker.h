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

#include "HmmerSearchSettings.h"

namespace U2 {

namespace LocalWorkflow {

class HmmerSearchPrompter : public PrompterBase<HmmerSearchPrompter> {
    Q_OBJECT
public:
    HmmerSearchPrompter(Actor* p = nullptr);

protected:
    QString composeRichDoc();
};

class HmmerSearchWorker : public BaseWorker {
    Q_OBJECT
public:
    HmmerSearchWorker(Actor* a);

    void init();
    bool isReady() const;
    Task* tick();
    void cleanup();

private slots:
    void sl_taskFinished(Task* task);

protected:
    IntegralBus* hmmPort;
    IntegralBus* seqPort;
    IntegralBus* output;
    QString resultName;
    HmmerSearchSettings cfg;
    QStringList hmms;
};

class HmmerSearchWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR;

    static void init();
    HmmerSearchWorkerFactory();
    Worker* createWorker(Actor* a);
};

}  // namespace LocalWorkflow
}  // namespace U2
