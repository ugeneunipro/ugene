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

#include "TaskLocalStorage.h"
#include <assert.h>

class ETSContext *getETSContext() {
    return U2::TaskLocalData::current();
}
int getETSWorkerID() {
    return U2::TaskLocalData::currentWorkerID();
}

namespace U2 {

QThreadStorage<ETSContextTLSRef*> TaskLocalData::tls;

class ETSContext* TaskLocalData::current(){
    ETSContextTLSRef* ref = tls.localData();
    if (ref!=NULL) {
        assert(ref->ctx!=NULL);
        return ref->ctx;
    }
    assert(0);
    return NULL;
}

unsigned TaskLocalData::currentWorkerID() {
    ETSContextTLSRef* ref = tls.localData();
    if (ref!=NULL) {
        return ref->workerID;
    }
    assert(0);
    return -1;

}

void TaskLocalData::bindToETSTLSContext(ETSContext *ctx, int workerID) {
    assert(ctx!=NULL);
    assert(!tls.hasLocalData());
    tls.setLocalData(new ETSContextTLSRef(ctx, workerID));
}

void TaskLocalData::detachETSTLSContext() {
    ETSContextTLSRef* ref = tls.localData();
    assert(ref!=NULL && ref->ctx!=NULL);
    ref->ctx = NULL;
    tls.setLocalData(NULL);
}


}//namespace

