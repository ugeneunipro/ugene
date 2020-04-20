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

#ifndef _GET_FILE_LIST_WORKER_
#define _GET_FILE_LIST_WORKER_

#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

class DatasetFilesIterator;

namespace LocalWorkflow {

class GetFileListPrompter : public PrompterBase<GetFileListPrompter> {
    Q_OBJECT
public:
    GetFileListPrompter(Actor *p = NULL) : PrompterBase<GetFileListPrompter>(p) {}

protected:
    QString composeRichDoc();

}; // GetFileListPrompter

class GetFileListWorker : public BaseWorker {
    Q_OBJECT
public:
    GetFileListWorker(Actor *p);

    virtual void init();
    virtual Task * tick();
    virtual void cleanup();

private:
    IntegralBus *outChannel;
    DatasetFilesIterator *files;

private:
    QString getNextUrl();
}; // GetFileListWorker

class GetFileListWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    GetFileListWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // GetFileListWorkerFactory

} // LocalWorkflow
} // U2

#endif // _GET_FILE_LIST_WORKER_
