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

#include "CharOccurTask.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>


namespace U2 {


CharOccurResult::CharOccurResult(char _charInSequence, qint64 _numberOfOccurrence, double _percentageOfOccur)
    : charInSequence(_charInSequence),
      numberOfOccurr(_numberOfOccurrence),
      percentageOfOccur(_percentageOfOccur)
{
}


CharOccurTask::CharOccurTask(const DNAAlphabet* _alphabet,
                             U2EntityRef _seqRef,
                             const QVector<U2Region>& regions)
    : BackgroundTask< QList<CharOccurResult> >(
    "Calculating characters occurrence",
    TaskFlag_None),
      alphabet(_alphabet),
      seqRef(_seqRef),
      regions(regions)
{
    tpm = Progress_Manual;
    stateInfo.setProgress(0);
}


void CharOccurTask::run()
{
    // Create the connection
    U2OpStatus2Log os;
    DbiConnection dbiConnection(seqRef.dbiRef, os);
    CHECK_OP(os, );

    U2SequenceDbi* sequenceDbi = dbiConnection.dbi->getSequenceDbi();

    // Verify the alphabet
    SAFE_POINT(0 != alphabet, "The alphabet is NULL!",)

    QByteArray alphabetChars = alphabet->getAlphabetChars();
    SAFE_POINT(!alphabetChars.isEmpty(), "There are no characters in the alphabet!",);

    QVector<quint64> charactersOccurrence(256, 0);
    qint64 totalLength = U2Region::sumLength(regions);
    qint64 processedLength = 0;
    foreach(const U2Region& region, regions) {
        QList<U2Region> blocks = U2Region::split(region, REGION_TO_ANALAYZE);
        foreach(const U2Region& block, blocks) {
            // Get the selected region and verify that the data has been correctly read
            QByteArray sequence = sequenceDbi->getSequenceData(seqRef.entityId, block, os);
            if (os.hasError() || sequence.isEmpty()) {
                taskLog.details("Skipping calculation of the characters occurrence.");
                break;
            }

            // Calculating the values
            const char* sequenceData = sequence.constData();
            for (int i = 0, n = sequence.size(); i < n; i++) {
                char c = sequenceData[i];
                charactersOccurrence[c]++;
            }

            // Update the task progress
            processedLength += block.length;
            stateInfo.setProgress(processedLength * 100 / totalLength);
            CHECK_OP(stateInfo,);
        }
    }

    // Calculate the percentage and format the result
    QList<CharOccurResult> calculatedResults;
    for (int i = 0; i < charactersOccurrence.length(); i++) {
        char c = (char) i;
        qint64 numberOfOccur = charactersOccurrence[i];
        if (numberOfOccur == 0) {
            continue;
        }
        SAFE_POINT(alphabetChars.contains(c),
                   QString("Unexpected characters has been detected in the sequence: {%1}").arg(c),);
        double percentageOfOccur = numberOfOccur * 100.0 / totalLength;
        CharOccurResult calcResult(c, numberOfOccur, percentageOfOccur);
        calculatedResults.append(calcResult);
    }

    result = calculatedResults;
}

} // namespace
