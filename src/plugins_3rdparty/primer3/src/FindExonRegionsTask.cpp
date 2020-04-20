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

#include <U2Algorithm/SplicedAlignmentTask.h>
#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/AnnotationTableObject.h>

#include "FindExonRegionsTask.h"

namespace U2 {

FindExonRegionsTask::FindExonRegionsTask(U2SequenceObject *dObj, const QString &annName)
    : Task("FindExonRegionsTask", TaskFlags_NR_FOSCOE), dnaObj(dObj), exonAnnName(annName)
{


}

void FindExonRegionsTask::prepare() {

}

QList<Task *> FindExonRegionsTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    if (!subTask->isFinished()) {
        return res;
    }

    // K.O.(14.05.2014):
    // This code is not required anymore, but it's a nice example on how to call SplicedAlignment

    /*if (subTask == loadDocumentTask) {
        Document *doc = loadDocumentTask->getDocument();
        QList<GObject *> objects = doc->findGObjectByType(GObjectTypes::SEQUENCE);
        if (objects.isEmpty()) {
            setError(tr("Failed to load RNA sequence from %2").arg(doc->getName()));
            return res;
        }

        U2SequenceObject *rnaSeqObj = qobject_cast<U2SequenceObject *>(objects.first());
        SAFE_POINT(NULL != rnaSeqObj, tr("Failed to load RNA sequence"), res);

        SplicedAlignmentTaskConfig cfg(rnaSeqObj, dnaObj);

        SplicedAlignmentTaskRegistry *sr = AppContext::getSplicedAlignmentTaskRegistry();
        QStringList algList = sr->getAlgNameList();

        if (algList.isEmpty()) {
            log.trace(tr("No algorithm found to align cDNA sequence"));
            return res;
        }

        alignmentTask = sr->getAlgorithm(algList.first())->createTaskInstance(cfg) ;
        res.append(alignmentTask);
        loadDocumentTask = NULL;
    } else if (subTask == alignmentTask) {
        QList<AnnotationData> results = alignmentTask->getAlignmentResult();
        foreach (const AnnotationData &ann, results) {
            exonRegions.append(ann.location->regions.toList());
        }
    }*/

    return res;
}

Task::ReportResult FindExonRegionsTask::report() {
    QList<GObject*> allAnnotationObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE);
    QList<GObject*> relAnns = GObjectUtils::findObjectsRelatedToObjectByRole(dnaObj, GObjectTypes::ANNOTATION_TABLE,
        ObjectRole_Sequence, allAnnotationObjects, UOF_LoadedOnly);

    if (relAnns.isEmpty()) {
        setError(tr("Failed to search for exon annotations. The sequence %1 doesn't have any related annotations.").arg(dnaObj->getSequenceName()));
        return ReportResult_Finished;
    }
    
    foreach (GObject* a, relAnns) {
        AnnotationTableObject *att = qobject_cast<AnnotationTableObject *>(a);
        const QList<Annotation *> anns = att->getAnnotations();
        foreach (Annotation *ann, anns) {
            if (ann->getName() == exonAnnName) {
                foreach (const U2Region &r, ann->getRegions()) {
                    exonRegions.append(r);
                }
            }
        }
    }
    qSort(exonRegions);
    return ReportResult_Finished;
}

} // namespace U2
