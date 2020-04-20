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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ReverseSequenceTask.h"

namespace U2 {

const int CHUNK_SIZE = 1024*256;

ReverseComplementSequenceTask::ReverseComplementSequenceTask(U2SequenceObject *dObj, const QList<AnnotationTableObject *> &annotations,
    DNASequenceSelection *s, DNATranslation *transl)
    : Task(tr("Reverse Complement Sequence Task"), TaskFlags_NR_FOSE_COSC),
      seqObj(dObj),
      aObjs(annotations),
      selection(s),
      complTT(transl)
{
    SAFE_POINT_EXT(seqObj != NULL, setError(L10N::nullPointerError("sequence object")),);
    addSubTask(new ReverseSequenceTask(seqObj, aObjs, selection));
    addSubTask(new ComplementSequenceTask(seqObj, aObjs, selection, complTT));
}

ReverseSequenceTask::ReverseSequenceTask(U2SequenceObject *seqObj, const QList<AnnotationTableObject *> &annotations, DNASequenceSelection *selection)
    : Task(tr("Reverse Sequence Task"), TaskFlags_NR_FOSE_COSC),
      seqObj(seqObj),
      aObjs(annotations),
      selection(selection)
{
    SAFE_POINT_EXT(seqObj != NULL, setError(L10N::nullPointerError("sequence object")),);
}

Task::ReportResult ReverseSequenceTask::report() {
    if (seqObj->getSequenceLength() == 0) {
        return ReportResult_Finished;
    }

    QVector<U2Region> regionsForward = SequenceWalkerTask::splitRange(U2Region(0, seqObj->getSequenceLength()),
                                                               CHUNK_SIZE, 0, 0, false);
    QVector<U2Region> regionsReverse = SequenceWalkerTask::splitRange(U2Region(0, seqObj->getSequenceLength()),
                                                               CHUNK_SIZE, 0, 0, true);
    SAFE_POINT_EXT(regionsForward.size() == regionsReverse.size(),
                   setError("Splitting sequence range worked wrong"), ReportResult_Finished);

    int size = regionsForward.size();
    U2Region middleRegion = regionsForward.first();
    for (int i = 0 ; i < (size - 1)/2; i++) {
        QByteArray buffer = seqObj->getSequenceData(regionsForward[i]);

        QByteArray revSeq = seqObj->getSequenceData(regionsReverse[size - 1 - i]);
        TextUtils::reverse(revSeq.data(), revSeq.size());
        seqObj->replaceRegion(regionsForward[i], DNASequence(revSeq), stateInfo);

        TextUtils::reverse(buffer.data(), buffer.size());
        seqObj->replaceRegion(regionsReverse[size - 1 - i], DNASequence(buffer), stateInfo);

        middleRegion.startPos = regionsForward[i].endPos();
        middleRegion.length = regionsReverse[size - 1 - i].startPos - middleRegion.startPos;

        if (hasError()) {
            return ReportResult_Finished;
        }
    }
    if(middleRegion.length > 1) {
        if(middleRegion.length <= CHUNK_SIZE) {
            QByteArray revSeq = seqObj->getSequenceData(middleRegion);
            TextUtils::reverse(revSeq.data(), revSeq.size());
            seqObj->replaceRegion(middleRegion, DNASequence(revSeq), stateInfo);
        } else {
            SAFE_POINT(middleRegion.length <= CHUNK_SIZE * 2, "Incorrect middle region", ReportResult_Finished);
            U2Region secondRegion(middleRegion.startPos + CHUNK_SIZE, middleRegion.length - CHUNK_SIZE);
            middleRegion.length = CHUNK_SIZE;

            QByteArray revSeq = seqObj->getSequenceData(middleRegion);
            TextUtils::reverse(revSeq.data(), revSeq.size());

            QByteArray secondRevSeq = seqObj->getSequenceData(secondRegion);
            TextUtils::reverse(secondRevSeq.data(), secondRevSeq.size());

            middleRegion.length = secondRegion.length;
            secondRegion.startPos = middleRegion.endPos();
            secondRegion.length = CHUNK_SIZE;

            seqObj->replaceRegion(middleRegion, DNASequence(secondRevSeq), stateInfo);
            seqObj->replaceRegion(secondRegion, DNASequence(revSeq), stateInfo);
        }
    }

    const int len = seqObj->getSequenceLength();
    // mirror selection
    if (NULL != selection) {
        QVector<U2Region> regions = selection->getSelectedRegions();
        U2Region::mirror(len, regions);
        U2Region::reverse(regions);
        selection->setSelectedRegions(regions);
    }

    // fix annotation locations
    foreach (AnnotationTableObject *aObj, aObjs) {
        foreach (Annotation *a, aObj->getAnnotations()) {
            U2Location location = a->getLocation();
            U2Region::mirror(len, location->regions);
            U2Region::reverse(location->regions);
            a->setLocation(location);
        }
    }

    return ReportResult_Finished;
}

ComplementSequenceTask::ComplementSequenceTask(U2SequenceObject *seqObj,
                                               const QList<AnnotationTableObject *> &annotations,
                                               DNASequenceSelection *selection,
                                               DNATranslation *complTT)
    : Task(tr("Complement Sequence Task"), TaskFlags_NR_FOSE_COSC),
      seqObj(seqObj),
      aObjs(annotations),
      selection(selection),
      complTT(complTT)
{
    SAFE_POINT_EXT(seqObj != NULL, setError(L10N::nullPointerError("sequence object")),);
    SAFE_POINT_EXT(complTT != NULL,
                   setError(L10N::nullPointerError("DNA translation table")),);
}

Task::ReportResult ComplementSequenceTask::report() {
    if (seqObj->getSequenceLength() == 0) {
        return ReportResult_Finished;
    }

    QVector<U2Region> regions = SequenceWalkerTask::splitRange(U2Region(0, seqObj->getSequenceLength()), CHUNK_SIZE, 0, 0, false);
    U2OpStatusImpl os;
    foreach(const U2Region& r, regions) {
        QByteArray chunk = seqObj->getSequenceData(r);
        complTT->translate(chunk.data(), chunk.size());
        seqObj->replaceRegion(r, DNASequence(chunk), os);
        if (os.hasError()) {
            setError(os.getError());
            return ReportResult_Finished;
        }
    }

    // fix annotation locations
    foreach (AnnotationTableObject *aObj, aObjs) {
        foreach (Annotation *a, aObj->getAnnotations()) {
            U2Strand strand = a->getStrand();
            a->setStrand(strand == U2Strand::Direct ? U2Strand::Complementary : U2Strand::Direct);
        }
    }

    return ReportResult_Finished;
}


}//namespace
