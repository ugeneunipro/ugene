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

#include "TLSTask.h"

namespace U2 {

QThreadStorage<TLSContextRef*> TLSUtils::tls;

/************************************************************************/
/* TaskLocalData                                                        */
/************************************************************************/
TLSContext* TLSUtils::current( QString contextId ) {
    Q_UNUSED(contextId)
    TLSContextRef* ref = tls.localData();
    if (ref!=NULL) {
        assert(ref->ctx!=NULL);
        assert(ref->ctx->id == contextId);
        return ref->ctx;
    }
    assert(0);
    return NULL;
}

void TLSUtils::bindToTLSContext(TLSContext *ctx) {
    assert(ctx!=NULL);
    assert(!tls.hasLocalData());
    tls.setLocalData(new TLSContextRef(ctx));
}

void TLSUtils::detachTLSContext() {
    TLSContextRef* ref = tls.localData();
    assert(ref!=NULL && ref->ctx!=NULL);
    ref->ctx = NULL;
    tls.setLocalData(NULL);
}

/************************************************************************/
/* TLSTask                                                              */
/************************************************************************/

TLSTask::TLSTask( const QString& _name, TaskFlags _flags, bool _deleteContext)
:Task(_name, _flags), taskContext(NULL), deleteContext(_deleteContext)
{
}

void TLSTask::prepare()
{
    taskContext = createContextInstance();
}

void TLSTask::run()
{
    TLSUtils::bindToTLSContext(taskContext);
    try {
        _run();
    } catch(...) {
        stateInfo.setError("_run() throws exception");
    }
    TLSUtils::detachTLSContext();
}

TLSTask::~TLSTask()
{
    if(deleteContext)
        delete taskContext;
    taskContext = NULL;
}
} //namespace U2
