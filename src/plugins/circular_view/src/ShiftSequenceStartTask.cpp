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
#include <U2Core/IOAdapter.h>
#include <U2Core/GObject.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/MultiTask.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ShiftSequenceStartTask.h"

namespace U2 {

ShiftSequenceStartTask::ShiftSequenceStartTask(U2SequenceObject* sequenceObject, qint64 newSequenceStartPosition)
        : Task(tr("ShiftSequenceStartTask"), TaskFlag_NoRun), sequenceObject(sequenceObject), newSequenceStartPosition(newSequenceStartPosition) {
    GCOUNTER(cvar, tvar, "ShiftSequenceStartTask");
}

Task::ReportResult ShiftSequenceStartTask::report(){

    if (newSequenceStartPosition == 0) {
        setError(tr("New sequence origin is the same as the old one"));
        return ReportResult_Finished;
    }

    qint64 sequenceLength = sequenceObject->getSequenceLength();
    if (newSequenceStartPosition < 0 || newSequenceStartPosition >= sequenceLength) {
        setError(tr("Sequence start position is out of range"));
        return ReportResult_Finished;
    }

    Document* documentWithSequence = sequenceObject->getDocument();
    CHECK_EXT(!documentWithSequence->isStateLocked(), setError(tr("Document is locked")), ReportResult_Finished);

    DNASequence dnaSequence = sequenceObject->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    dnaSequence.seq = dnaSequence.seq.mid(newSequenceStartPosition) + dnaSequence.seq.mid(0, newSequenceStartPosition);
    sequenceObject->setWholeSequence(dnaSequence);

    QList<Document*> documentsToUpdate;
    Project *p = AppContext::getProject();
    if (p != NULL){
        if (p->isStateLocked()){
            return ReportResult_CallMeAgain;
        }
        documentsToUpdate = p->getDocuments();
    }

    if (!documentsToUpdate.contains(documentWithSequence)){
        documentsToUpdate.append(documentWithSequence);
    }

    foreach (Document* document, documentsToUpdate) {
        QList<GObject*> annotationTablesList = document->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
        foreach (GObject* object, annotationTablesList) {
            AnnotationTableObject* annotationTableObject = qobject_cast<AnnotationTableObject*>(object);
            if (annotationTableObject->hasObjectRelation(sequenceObject, ObjectRole_Sequence)){
                foreach (Annotation* annotation, annotationTableObject->getAnnotations()) {
                    const U2Location& location = annotation->getLocation();
                    U2Location newLocation = U1AnnotationUtils::shiftLocation(location, -newSequenceStartPosition, sequenceLength);
                    annotation->setLocation(newLocation);
                }
            }
        }
    }

    return ReportResult_Finished;
}

} //ns
