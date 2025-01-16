/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "RemoteBLASTPrimerPairsToAnnotationsTask.h"
#include "RemoteBLASTPrimerPairToAnnotationsTask.h"

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include <QPointer>

namespace U2 {


RemoteBLASTPrimerPairsToAnnotationsTask::RemoteBLASTPrimerPairsToAnnotationsTask(const QPointer<U2SequenceObject>& _seqObj,
                                                                                 const QList<QPair<Annotation*, Annotation*>>& _primerAnnotationPairs,
                                                                                 const RemoteBLASTTaskSettings& _cfg)
    : Task(tr("BLAST primer pairs"), TaskFlags_NR_FOSE_COSC),
    seqObj(_seqObj),
    primerAnnotationPairs(_primerAnnotationPairs),
    cfg(_cfg) {
    GCOUNTER(cvar, "BLAST_primer_multiple_pairs");
    // We should not run more than one BLAST request per time,
    // because multiple requests looks very suspicious and BLAST could block them all
    setMaxParallelSubtasks(1);
}

void RemoteBLASTPrimerPairsToAnnotationsTask::prepare() {
    for (const auto& primerPair : qAsConst(primerAnnotationPairs)) {
        auto group = primerPair.first->getGroup();
        auto annotationTableObject = primerPair.first->getGObject();
        SAFE_POINT(annotationTableObject == primerPair.second->getGObject(), "Primers should have similar Annotation Table Objects", );

        auto t = new RemoteBLASTPrimerPairToAnnotationsTask(group->getName(), seqObj, annotationTableObject, primerPair.first->getData(), primerPair.second->getData(), cfg, group->getGroupPath());
        addSubTask(t);
    }
}


}  // namespace U2
