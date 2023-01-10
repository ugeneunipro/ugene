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

#ifndef _U2_BLAST_WORKER_H_
#define _U2_BLAST_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include "BlastCommonTask.h"

namespace U2 {

class ExternalTool;

namespace LocalWorkflow {

class BlastPrompter : public PrompterBase<BlastPrompter> {
    Q_OBJECT
public:
    BlastPrompter(Actor* p = 0);

protected:
    QString composeRichDoc();
};

class BlastWorker : public BaseWorker {
    Q_OBJECT
public:
    BlastWorker(Actor* a);

    void init() override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_taskFinished();

protected:
    IntegralBus *input, *output;
    QString resultName, transId;
    BlastTaskSettings cfg;
};

class BlastWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    static QString getHitsName();
    static QString getHitsDescription();

    static void init();
    BlastWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) override {
        return new BlastWorker(a);
    }
};

class ToolsValidator : public ActorValidator {
public:
    bool validate(const Actor* actor, NotificationsList& notificationList, const QMap<QString, QString>& options) const override;

private:
    ExternalTool* getTool(const QString& program) const;
};

}  // namespace LocalWorkflow
}  // namespace U2

#endif
