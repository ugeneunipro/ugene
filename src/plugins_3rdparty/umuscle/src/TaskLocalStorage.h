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

#ifndef _UMUSCLE_TASK_LOCAL_STORAGE_
#define _UMUSCLE_TASK_LOCAL_STORAGE_

#include <QThreadStorage>

class MuscleContext;

namespace U2 {

class MuscleContextTLSRef {
public:
    MuscleContextTLSRef(MuscleContext* _ctx, int _workerID) : ctx(_ctx), workerID(_workerID){}
    MuscleContext* ctx;
    int workerID;
};

class TaskLocalData {
public:
    static MuscleContext* current();

    static unsigned currentWorkerID();


    static void bindToMuscleTLSContext(MuscleContext *ctx, int workerID = 0);

    static void detachMuscleTLSContext();


private:
    static QThreadStorage<MuscleContextTLSRef*> tls;
};
} //namespace

#endif
