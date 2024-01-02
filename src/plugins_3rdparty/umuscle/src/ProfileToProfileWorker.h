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
#include <U2Core/MultipleAlignmentObject.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

class MultipleAlignmentObject;

namespace LocalWorkflow {

class ProfileToProfileWorker : public BaseWorker {
    Q_OBJECT
public:
    ProfileToProfileWorker(Actor* a);

    void init() override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_taskFinished();

private:
    IntegralBus* inPort;
    IntegralBus* outPort;

    QList<MultipleAlignmentObject*> objects;
};

class ProfileToProfileWorkerFactory : public DomainFactory {
public:
    ProfileToProfileWorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }

    static void init();
    Worker* createWorker(Actor* a) override;

private:
    static const QString ACTOR_ID;
};

class ProfileToProfilePrompter : public PrompterBase<ProfileToProfilePrompter> {
    Q_OBJECT
public:
    ProfileToProfilePrompter(Actor* p = nullptr)
        : PrompterBase<ProfileToProfilePrompter>(p) {
    }

protected:
    QString composeRichDoc() override;
};

class ProfileToProfileTask : public Task {
    Q_OBJECT
public:
    ProfileToProfileTask(const MultipleAlignment& masterMsa, const MultipleAlignment& secondMsa);
    ~ProfileToProfileTask() override;

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    const MultipleAlignment& getResult();

private:
    MultipleAlignment masterMsa;
    MultipleAlignment secondMsa;
    MultipleAlignment result;
    int seqIdx;
    int subtaskCount;

private:
    void appendResult(Task* task);
    QList<Task*> createAlignTasks();
    bool canCreateTask() const;
};

}  // namespace LocalWorkflow
}  // namespace U2
