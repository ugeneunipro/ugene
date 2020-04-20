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

#ifndef _EXPORT_PHRED_QUALITY_WORKER_H_
#define _EXPORT_PHRED_QUALITY_WORKER_H_

#include <U2Core/DNASequence.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "ExportQualityScoresTask.h"

namespace U2 {

class U2SequenceObject;

namespace LocalWorkflow {

class ExportPhredQualityPrompter : public PrompterBase<ExportPhredQualityPrompter> {
    Q_OBJECT
public:
    ExportPhredQualityPrompter(Actor* p = 0) : PrompterBase<ExportPhredQualityPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class ExportPhredQualityWorker : public BaseWorker {
    Q_OBJECT
public:
    ExportPhredQualityWorker(Actor* a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

protected:
    CommunicationChannel *input;
    QString             fileName;
    QList<U2SequenceObject*> seqObjList;
    Task*               currentTask;

};

class ExportPhredQualityWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    ExportPhredQualityWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a);
};

} // Workflow namespace
} // U2 namespace

#endif // _EXPORT_PHRED_QUALITY_WORKER_H_
