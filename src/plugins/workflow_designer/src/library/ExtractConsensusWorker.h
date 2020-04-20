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

#ifndef _U2_EXTRACTCONSENSUSWORKER_H_
#define _U2_EXTRACTCONSENSUSWORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
    class AssemblyConsensusAlgorithm;
    class AssemblyModel;
    class ExportConsensusTask;
namespace LocalWorkflow {
using namespace Workflow;

class ExtractConsensusWorker : public BaseWorker {
    Q_OBJECT
public:
    ExtractConsensusWorker(Actor *actor);

    void init();
    Task * tick();
    void cleanup();

private slots:
    void sl_taskFinished();

private:
    bool hasAssembly() const;
    U2EntityRef takeAssembly(U2OpStatus &os);
    Task * createTask(const U2EntityRef &assembly);
    void finish();
    void sendResult(const SharedDbiDataHandler &seqId);
};

class ExtractConsensusTaskHelper : public Task {
    Q_OBJECT
public:
    ExtractConsensusTaskHelper(const QString &algoId, bool keepGaps, const U2EntityRef &assembly, const U2DbiRef &targetDbi);

    void prepare();
    U2EntityRef getResult() const;

private:
    const QString algoId;
    const bool keepGaps;
    const U2EntityRef assembly;
    const U2DbiRef targetDbi;
    ExportConsensusTask *exportTask;

private:
    AssemblyConsensusAlgorithm * createAlgorithm();
    AssemblyModel * createModel();
};

class ExtractConsensusWorkerFactory : public DomainFactory {
public:
    ExtractConsensusWorkerFactory();

    Worker * createWorker(Actor *actor);

    static void init();

    static const QString ACTOR_ID;
};

class ExtractConsensusWorkerPrompter : public PrompterBase<ExtractConsensusWorkerPrompter>{
    Q_OBJECT
public:
    ExtractConsensusWorkerPrompter(Actor *actor = NULL);

protected:
    QString composeRichDoc();
};

} // LocalWorkflow
} // U2

#endif // _U2_EXTRACTCONSENSUSWORKER_H_
