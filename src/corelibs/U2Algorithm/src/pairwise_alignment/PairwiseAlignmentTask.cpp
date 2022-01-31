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

#include "PairwiseAlignmentTask.h"

namespace U2 {

const QString PairwiseAlignmentTaskSettings::DEFAULT_NAME("PairwiseAlignmentResult.aln");

PairwiseAlignmentTaskSettings::PairwiseAlignmentTaskSettings(const QVariantMap &someSettings)
    : AbstractAlignmentTaskSettings(someSettings) {
}

PairwiseAlignmentTaskSettings::PairwiseAlignmentTaskSettings(const PairwiseAlignmentTaskSettings &s)
    : AbstractAlignmentTaskSettings(s),
      firstSequenceRef(s.firstSequenceRef),
      secondSequenceRef(s.secondSequenceRef) {
}

bool PairwiseAlignmentTaskSettings::isValid() const {
    return firstSequenceRef.isValid() && secondSequenceRef.isValid() && AbstractAlignmentTaskSettings::isValid();
}

PairwiseAlignmentTask::PairwiseAlignmentTask(const PairwiseAlignmentTaskSettings *settings, const TaskFlags &flags)
    : AbstractAlignmentTask(tr("Pairwise alignment task"), flags, settings) {
}

}  // namespace U2
