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

#ifndef _U2_SEQUENCES_TO_MSA_WORKER_H_
#define _U2_SEQUENCES_TO_MSA_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/DNASequence.h>

namespace U2 {
namespace LocalWorkflow {

class SequencesToMSAPromter : public PrompterBase<SequencesToMSAPromter> {
    Q_OBJECT
public:
    SequencesToMSAPromter( Actor * p = 0 ) : PrompterBase<SequencesToMSAPromter>(p) {};
protected:
    QString composeRichDoc();
};

class SequencesToMSAWorker : public BaseWorker {
    Q_OBJECT
public:
    SequencesToMSAWorker( Actor * p ) : BaseWorker(p), inPort(NULL), outPort(NULL) {};

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();
private:
    IntegralBus* inPort;
    IntegralBus* outPort;
private slots:
    void sl_onTaskFinished(Task* t);
private:
    QList<DNASequence> data;
};

class SequencesToMSAWorkerFactory : public DomainFactory {
public:
    const static QString ACTOR_ID;
    SequencesToMSAWorkerFactory() : DomainFactory(ACTOR_ID) {};
    static void init();
    virtual Worker* createWorker( Actor * a ) { return new SequencesToMSAWorker(a); }
};

class MSAFromSequencesTask : public Task {
    Q_OBJECT
public:
    MSAFromSequencesTask(const QList<DNASequence>& sequences)
        : Task(tr("MSAFromSequencesTask"), TaskFlag_None), sequences_(sequences) {}
    void run();
    MultipleSequenceAlignment getResult() const { return ma; }
private:
    QList<DNASequence> sequences_;
    MultipleSequenceAlignment ma;
};

} //LocalWorkflow namespace
} //U2 namespace

#endif
