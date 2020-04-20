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

#ifndef _IMPORT_PHRED_QUALITY_WORKER_H_
#define _IMPORT_PHRED_QUALITY_WORKER_H_

#include <U2Core/DNASequence.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include "ImportQualityScoresTask.h"

namespace U2 {

class U2SequenceObject;

namespace LocalWorkflow {

class ImportPhredQualityPrompter : public PrompterBase<ImportPhredQualityPrompter> {
    Q_OBJECT
public:
    ImportPhredQualityPrompter(Actor* p = 0) : PrompterBase<ImportPhredQualityPrompter>(p) {}
protected:
    QString composeRichDoc();
};

class ImportPhredQualityWorker : public BaseWorker {
    Q_OBJECT
public:
    ImportPhredQualityWorker(Actor* a);

    virtual void init();
    virtual Task* tick();
    virtual void cleanup();

protected:
    CommunicationChannel *input, *output;
    ReadQualityScoresTask* readTask;
    QString             fileName;
    DNAQualityType      type;
    DNAQualityFormat    format;
};

class ImportPhredQualityWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    ImportPhredQualityWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* a) {return new ImportPhredQualityWorker(a);}
};

} // Workflow namespace
} // U2 namespace

#endif // _IMPORT_PHRED_QUALITY_WORKER_H_
