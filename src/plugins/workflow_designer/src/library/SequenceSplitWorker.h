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

#ifndef __SEQUENCE_SPLIT_WORKER_H__
#define __SEQUENCE_SPLIT_WORKER_H__

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequence.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/ExtractAnnotatedRegionTask.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace LocalWorkflow {

class SequenceSplitPromter : public PrompterBase<SequenceSplitPromter> {
    Q_OBJECT
public:
    SequenceSplitPromter(Actor *p = 0) : PrompterBase<SequenceSplitPromter>(p) {};
protected:
    QString composeRichDoc();
};

class SequenceSplitWorker : public BaseWorker {
    Q_OBJECT
public:
    SequenceSplitWorker(Actor *p)
        : BaseWorker(p), seqPort(NULL), outPort(NULL), useAcceptedOrFiltered(false)
    {

    }

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

protected:
    IntegralBus * seqPort;
    IntegralBus * outPort;

private:
    QList<Task *> ssTasks;
    QList<SharedAnnotationData> inputAnns;

    QStringList acceptedNames;
    QStringList filteredNames;
    bool useAcceptedOrFiltered;
    ExtractAnnotatedRegionTaskSettings cfg;
private slots:
    void sl_onTaskFinished(Task *t);
};

class SequenceSplitWorkerFactory : public DomainFactory {
public:
    const static QString ACTOR;
    SequenceSplitWorkerFactory() : DomainFactory(ACTOR) {};
    static void init();
    virtual Worker * createWorker(Actor *a);
};

} //ns LocalWorkflow
} //ns U2

#endif
