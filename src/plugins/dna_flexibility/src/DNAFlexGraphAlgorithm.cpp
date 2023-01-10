/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "DNAFlexGraphAlgorithm.h"

#include "DNAFlexPlugin.h"
#include "FindHighFlexRegionsAlgorithm.h"

namespace U2 {

/**
 * Calculates data for a DNA Flexibility graph
 *
 * @param result Points of the graph
 * @param sequenceObject The sequence used to draw the graph
 * @param region The region of the sequence to use
 * @param windowData Current parameters of the graph (window, step, etc.)
 */
void DNAFlexGraphAlgorithm::calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) {
    QByteArray sequence = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    CHECK(window > 1, );

    // Reserve result space.
    U2Region region(0, sequenceObject->getSequenceLength());
    int stepCount = GSequenceGraphUtils::getNumSteps(region, window, step);
    result.reserve(stepCount);

    // Calculate the result.
    for (int windowStartPos = region.startPos; windowStartPos <= region.endPos() - window; windowStartPos += step) {
        // Calculating the threshold in the current window.
        float windowThreshold = 0;
        qint64 nPairs = window - 1;
        for (int pos = windowStartPos; pos < windowStartPos + nPairs; pos++) {
            windowThreshold += FindHighFlexRegionsAlgorithm::flexibilityAngle(sequence[pos], sequence[pos + 1]);
        }
        CHECK_OP(os, );
        windowThreshold /= nPairs;

        // Returning the point on the graph
        result.append(windowThreshold);
    }
}

}  // namespace U2
