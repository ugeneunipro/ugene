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

#include "CuffdiffSupportTask.h"

namespace U2 {
namespace LocalWorkflow {

class CuffdiffPrompter : public PrompterBase<CuffdiffPrompter> {
    Q_OBJECT

public:
    CuffdiffPrompter(Actor* parent = 0);

protected:
    QString composeRichDoc() override;
};

class CuffdiffWorker : public BaseWorker {
    Q_OBJECT
public:
    CuffdiffWorker(Actor* actor);

    void init() override;
    bool isReady() const override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_onTaskFinished();

private:
    CommunicationChannel* inAssembly;
    CommunicationChannel* inTranscript;

    bool groupBySamples;
    QMap<QString, QStringList> assemblyUrls;

private:
    void initSlotsState();
    CuffdiffSettings scanParameters() const;
    CuffdiffSettings takeSettings();
    void takeAssembly();
};

class CuffdiffWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    CuffdiffWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* actor) override {
        return new CuffdiffWorker(actor);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2
