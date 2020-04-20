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

#ifndef _U2_GENOME_ALIGNER_INDEX_WORKER_H_
#define _U2_GENOME_ALIGNER_INDEX_WORKER_H_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "GenomeAlignerTask.h"
#include "GenomeAlignerIO.h"

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* Genome aligner index build                                           */
/************************************************************************/
class GenomeAlignerBuildPrompter : public PrompterBase<GenomeAlignerBuildPrompter> {
    Q_OBJECT
public:
    GenomeAlignerBuildPrompter(Actor* p = 0) : PrompterBase<GenomeAlignerBuildPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class GenomeAlignerBuildWorker : public BaseWorker {
    Q_OBJECT
public:
    GenomeAlignerBuildWorker(Actor* a) : BaseWorker(a), output(NULL), done(false) {}
    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual bool isDone() const;
    virtual void cleanup();
private slots:
    void sl_taskFinished();

protected:
    CommunicationChannel *output;
    QString resultName, transId;
    GUrl refSeqUrl;
    GUrl indexUrl;
    bool done;
    DnaAssemblyToRefTaskSettings settings;
};

class GenomeAlignerBuildWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    GenomeAlignerBuildWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new GenomeAlignerBuildWorker(a);}
};

/************************************************************************/
/* Genome aligner index read                                            */
/************************************************************************/
class GenomeAlignerIndexReaderPrompter : public PrompterBase<GenomeAlignerIndexReaderPrompter> {
    Q_OBJECT
public:
    GenomeAlignerIndexReaderPrompter(Actor* p = 0) : PrompterBase<GenomeAlignerIndexReaderPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class GenomeAlignerIndexReaderWorker : public BaseWorker {
    Q_OBJECT
public:
    GenomeAlignerIndexReaderWorker(Actor* a) : BaseWorker(a), output(NULL), done(false) {}
    virtual void init();
    virtual bool isReady() const;
    virtual Task* tick();
    virtual bool isDone() const;
    virtual void cleanup();
private slots:
    void sl_taskFinished();

protected:
    CommunicationChannel *output;
    QString resultName,transId;
    GUrl indexUrl;
    bool done;
};

class GenomeAlignerIndexReaderWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    GenomeAlignerIndexReaderWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new GenomeAlignerIndexReaderWorker(a);}
};

} // Workflow namespace
} // U2 namespace

#endif // GENOME_ALIGNER_INDEX_WORKER
