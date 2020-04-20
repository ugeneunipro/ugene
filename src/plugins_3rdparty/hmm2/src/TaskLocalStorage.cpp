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


HMMERTaskLocalData::HMMERTaskLocalData()
{
    sre_randseed = 42;
    rnd = 0;
    rnd1 = 0;
    rnd2 = 0;
}

struct HMMERTaskLocalData *getHMMERTaskLocalData() {
    return U2::TaskLocalData::current();
}


namespace U2 {

QHash<qint64, struct HMMERTaskLocalData*> TaskLocalData::data;
QThreadStorage<ContextIdContainer*> TaskLocalData::tls;
QMutex TaskLocalData::mutex;


HMMERTaskLocalData* TaskLocalData::current(){
    static HMMERTaskLocalData def;
    ContextIdContainer* idc = tls.localData();
    if (idc!=NULL){
        QMutexLocker ml(&mutex);
        HMMERTaskLocalData* res = data.value(idc->contextId);
        assert(res!=NULL);
        return res;
    } else {
        return &def;
    }
}

HMMERTaskLocalData* TaskLocalData::createHMMContext(qint64 contextId, bool bindThreadToContext) {
    QMutexLocker ml(&mutex);
    assert(!data.contains(contextId));
    HMMERTaskLocalData* ctx = new HMMERTaskLocalData();
    data[contextId] = ctx;

    if (bindThreadToContext) {
        bindToHMMContext(contextId);
    }

    return ctx;
}

void TaskLocalData::freeHMMContext(qint64 contextId) {
    QMutexLocker ml(&mutex);
    HMMERTaskLocalData* v = data.value(contextId);
    assert(v!=NULL);
    int n = data.remove(contextId); Q_UNUSED(n);
    assert(n == 1);
    delete v;
}



void TaskLocalData::bindToHMMContext(qint64 contextId){
    assert(!tls.hasLocalData());

    ContextIdContainer* idc = new ContextIdContainer(contextId);
    tls.setLocalData(idc);
}


qint64 TaskLocalData::detachFromHMMContext() {
    ContextIdContainer *idc = tls.localData();
    assert(idc!=NULL);
    qint64 contextId = idc->contextId;
    tls.setLocalData(NULL); //automatically deletes prev data
    return contextId;
}

}//namespace

