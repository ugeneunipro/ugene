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


#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Log.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ShiftSequenceStartTask.h"

namespace U2 {

ShiftSequenceStartTask::ShiftSequenceStartTask(U2SequenceObject *_seqObj, int _shiftSize)

:Task(tr("ShiftSequenceStartTask"), TaskFlag_NoRun), seqObj(_seqObj),seqStart(_shiftSize)
{
    GCOUNTER(cvar, tvar, "ShiftSequenceStartTask");
}

Task::ReportResult ShiftSequenceStartTask::report(){

    if (seqStart == 0) {
        setError("New sequence origin is the same as the old one");
        return ReportResult_Finished;
    }

    CHECK(abs(seqStart) < seqObj->getSequenceLength(), ReportResult_Finished);

    Document* curDoc = seqObj->getDocument();
    CHECK_EXT(!curDoc->isStateLocked(), setError(tr("Document is locked")), ReportResult_Finished);

    DNASequence dna = seqObj->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);

    dna.seq = dna.seq.mid(seqStart) + dna.seq.mid(0, seqStart);
    seqObj->setWholeSequence(dna);


    Project *p = AppContext::getProject();
    if (p != NULL){
        if (p->isStateLocked()){
            return ReportResult_CallMeAgain;
        }
        docs = p->getDocuments();
    }


    if (!docs.contains(curDoc)){
        docs.append(curDoc);
    }

    fixAnnotations(seqStart);

    return ReportResult_Finished;
}

void ShiftSequenceStartTask::fixAnnotations(int shiftSize) {
    foreach (Document *d, docs) {
        QList<GObject *> annotationTablesList = d->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
        foreach (GObject *table, annotationTablesList) {
            AnnotationTableObject *ato = qobject_cast<AnnotationTableObject *>(table);
            if (ato->hasObjectRelation(seqObj, ObjectRole_Sequence)){
                foreach (Annotation *an, ato->getAnnotations()) {
                    const U2Location& location = an->getLocation();
                    U2Location newLocation = shiftLocation(location, shiftSize, seqObj->getSequenceLength());
                    an->setLocation(newLocation);
                }
            }
        }
    }
}

U2Location ShiftSequenceStartTask::shiftLocation(const U2Location& location, int shiftSize, int seqLength) {

    U2Location newLocation(location);
    newLocation->regions.clear();

    int joinIdx = -1;

    int numRegions = location->regions.size();
    for(int i = 0; i < numRegions; ++i) {
        const U2Region& r = location->regions[i];
        if (r.endPos() == seqLength && (i + 1 < numRegions)) {
            const U2Region& r2 = location->regions[i+1];
            if (r2.startPos == 0) {
                joinIdx = i;
            }
        }

        U2Region newRegion(r.startPos - shiftSize, r.length);
        if (newRegion.endPos() <= 0) {
            newRegion.startPos += seqLength;
        } else if (newRegion.startPos < 0) {
            int additionStartPos = newRegion.startPos + seqLength;
            int additionLength = seqLength - additionStartPos;
            U2Region newRegionAddition(additionStartPos, additionLength);
            newLocation->regions.append(newRegionAddition);
            newRegion.startPos = 0;
            newRegion.length = r.length - additionLength;
            newLocation->op = U2LocationOperator_Join;
        }
        newLocation->regions.append(newRegion);
    }


    if (joinIdx != -1) {
        if (newLocation->regions.size() > joinIdx + 1) {
            const U2Region& r1 = newLocation->regions[joinIdx];
            const U2Region& r2 = newLocation->regions[joinIdx + 1];
            assert((r1.endPos() == r2.startPos) 
                || (r1.endPos() == seqLength && r2.startPos == 0));
            U2Region joined(r1.startPos, r1.length + r2.length);
            newLocation->regions.replace(joinIdx, joined);
            newLocation->regions.remove(joinIdx + 1);

        }
    }



    return newLocation;
}




}//ns
