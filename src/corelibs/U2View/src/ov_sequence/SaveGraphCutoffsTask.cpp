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

#include "SaveGraphCutoffsTask.h"

namespace U2 {

SaveCutoffsTask::SaveCutoffsTask(SaveCutoffsTaskSettings _s):
Task("Run saving graph cutoffs as annotations task", TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported), settings(_s){

}

void SaveCutoffsTask::prepare(){
    /*
    int startPos = settings.d->cachedFrom, len = settings.d->cachedLen, step = settings.d->cachedS;
    PairVector& points = settings.d->cachedData;
    for (int i = 0, n = nPoints; i < n; i++) {
        float fy1 = points.firstPoints[i];
        if (fy1 == UNKNOWN_VAL) {

        }
    }
    */
}

void SaveCutoffsTask::run(){

}

Task::ReportResult SaveCutoffsTask::report(){
    return ReportResult_Finished;
}

}
