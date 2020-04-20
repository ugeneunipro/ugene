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
 *  You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "TaskLocalStorage.h"
#include "muscle/muscle.h"
#include "muscle/objscore.h"
#include "muscle/profile.h"
#include "muscle/enumopts.h"
#include "muscle/params.h"
#include "muscle/muscle_context.h"

class MuscleContext *getMuscleContext() {
    return U2::TaskLocalData::current();
}
int getMuscleWorkerID() {
    return U2::TaskLocalData::currentWorkerID();
}

namespace U2 {

QThreadStorage<MuscleContextTLSRef*> TaskLocalData::tls;

class MuscleContext* TaskLocalData::current(){
    MuscleContextTLSRef* ref = tls.localData();
    if (ref!=NULL) {
        assert(ref->ctx!=NULL);
        return ref->ctx;
    }
    assert(0);
    return NULL;
}

unsigned TaskLocalData::currentWorkerID() {
    MuscleContextTLSRef* ref = tls.localData();
    if (ref!=NULL) {
        return ref->workerID;
    }
    assert(0);
    return -1;

}

void TaskLocalData::bindToMuscleTLSContext(MuscleContext *ctx, int workerID) {
    assert(ctx!=NULL);
    assert(!tls.hasLocalData());
    tls.setLocalData(new MuscleContextTLSRef(ctx, workerID));
}

void TaskLocalData::detachMuscleTLSContext() {
    MuscleContextTLSRef* ref = tls.localData();
    assert(ref!=NULL && ref->ctx!=NULL);
    ref->ctx = NULL;
    tls.setLocalData(NULL);
}


}//namespace

