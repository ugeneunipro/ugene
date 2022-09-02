/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_COMPOSE_RESULT_SUBTASK_H_
#define _U2_COMPOSE_RESULT_SUBTASK_H_

#include <U2Core/DNAChromatogram.h>
#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>
#include <U2Lang/DbiDataStorage.h>

#include "BlastAlignToReferenceTask.h"

namespace U2 {

class MultipleChromatogramAlignment;
class MultipleChromatogramAlignmentObject;
class MultipleChromatogramAlignmentRow;

namespace Workflow {

class BlastAlignToReferenceTask;

class ComposeResultSubtask : public Task {
    Q_OBJECT
public:
    ComposeResultSubtask(const SharedDbiDataHandler& reference,
                         const QList<AlignToReferenceResult>& pairwiseAlignments,
                         DbiDataStorage* storage);

    void run() override;

    const SharedDbiDataHandler& getAnnotations() const;

    U2SequenceObject* takeReferenceSequenceObject();
    MultipleChromatogramAlignmentObject* takeMcaObject();

private:
    /** Merges reference gaps from all pairwise alignments. */
    QVector<U2MsaGap> getReferenceGaps() const;

    static QVector<U2MsaGap> getShiftedGaps(const QVector<U2MsaGap>& gaps);
    void insertShiftedGapsIntoReference();
    void insertShiftedGapsIntoRead(MultipleChromatogramAlignment& alignment,
                                   int mcaRowIndex,
                                   const AlignToReferenceResult& alignResult,
                                   const QVector<U2MsaGap>& mergedReferenceGaps);
    void createAlignmentAndAnnotations();
    void enlargeReferenceByGaps();
    U2Region getReadRegion(const MultipleChromatogramAlignmentRow& readRow, const QVector<U2MsaGap>& referenceGapModel) const;
    U2Location getLocation(const U2Region& region, bool isComplement);

private:
    const SharedDbiDataHandler reference;
    const QList<AlignToReferenceResult> pairwiseAlignments;
    DbiDataStorage* storage = nullptr;
    MultipleChromatogramAlignmentObject* mcaObject = nullptr;
    U2SequenceObject* referenceSequenceObject = nullptr;
    SharedDbiDataHandler annotationsDataHandler;
};

}  // namespace Workflow
}  // namespace U2

#endif  // _U2_COMPOSE_RESULT_SUBTASK_H_
