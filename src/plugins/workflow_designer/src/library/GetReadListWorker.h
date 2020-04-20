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

#ifndef _U2_GET_READ_LIST_WORKER_H_
#define _U2_GET_READ_LIST_WORKER_H_

#include <U2Lang/IntegralBusUtils.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

class DatasetFilesIterator;

namespace LocalWorkflow {

class GetReadsListPrompter : public PrompterBase<GetReadsListPrompter> {
    Q_OBJECT
public:
    GetReadsListPrompter(Actor *p = NULL) : PrompterBase<GetReadsListPrompter>(p) {}

protected:
    QString composeRichDoc();

};

class GetReadsListWorker : public BaseWorker {
    Q_OBJECT
public:
    GetReadsListWorker(Actor *p);

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

private:
    IntegralBus *outChannel;
    DatasetFilesIterator *files;
    DatasetFilesIterator *pairedFiles;

};

class GetReadsListWorkerFactory : public DomainFactory {
public:
    static const QString SE_ACTOR_ID;
    static const QString PE_ACTOR_ID;

    static const QString SE_SLOT_ID;
    static const QString PE_SLOT_ID;

    static const Descriptor SE_SLOT();
    static const Descriptor PE_SLOT();

    GetReadsListWorkerFactory(const QString &id) : DomainFactory(id) {}
    static void init();
    static void cleanup();
    virtual Worker *createWorker(Actor *a);
};

class SeReadsListSplitter : public Workflow::CandidatesSplitter {
public:
    SeReadsListSplitter();

    bool canSplit(const Descriptor &toDesc, DataTypePtr toDatatype);

    static const QString ID;

private:
    bool isMain(const QString &candidateSlotId);
};

class PeReadsListSplitter : public Workflow::CandidatesSplitter {
public:
    PeReadsListSplitter();

    bool canSplit(const Descriptor &toDesc, DataTypePtr toDatatype);

    static const QString ID;

private:
    bool isMain(const QString &candidateSlotId);
};


} // LocalWorkflow
} // U2

#endif
