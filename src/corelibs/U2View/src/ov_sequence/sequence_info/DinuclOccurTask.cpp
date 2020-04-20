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

#include "DinuclOccurTask.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>


namespace U2 {

DinuclOccurTask::DinuclOccurTask(const DNAAlphabet* _alphabet,
                                 const U2EntityRef _seqRef,
                                 const QVector<U2Region>& regions)
    : BackgroundTask< QMap<QByteArray, qint64> >(
    "Calculating dinucleotides occurrence",
    TaskFlag_None),
      alphabet(_alphabet),
      seqRef(_seqRef),
      regions(regions)
{
    tpm = Progress_Manual;
    stateInfo.setProgress(0);
}


#define DI_NUCL_CODE(n1, n2) ((quint16((quint32)n1)<<8) + n2)
#define GAP = '-';

void DinuclOccurTask::run()
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

    qint64 seqLength = sequenceDbi->getSequenceObject(seqRef.entityId, os).length;
    CHECK_OP(os, );

    if (seqLength < 2) {
        return;
    }

    QVector<quint64> dinuclOccurrence(256 * 256, 0);
    qint64 totalLength = U2Region::sumLength(regions);
    qint64 processedLength = 0;
    foreach (const U2Region& region, regions) {
        QList<U2Region> blocks = U2Region::split(region, REGION_TO_ANALAYZE);
        foreach(const U2Region& block, blocks) {
            // Get the selected region and verify that the data has been correctly read
            QByteArray sequence = sequenceDbi->getSequenceData(seqRef.entityId, block, os);
            if (os.hasError() || sequence.isEmpty()) {
                taskLog.details("Skipping calculation of the dinucleotides occurrence.");
                break;
            }

            // Calculating the values
            for (int i = 0, n = sequence.size(); i < n - 1; ++i) {
                char firstChar = sequence[i];
                char secondChar = sequence[i + 1];
                SAFE_POINT(alphabetChars.contains(secondChar),
                           QString("Unexpected characters has been detected in the sequence: {%1}").arg(secondChar),);

                dinuclOccurrence[DI_NUCL_CODE(firstChar, secondChar)]++;
            }

            // Update the task progress
            processedLength+=block.length;
            stateInfo.setProgress(processedLength* 100 / totalLength);
            CHECK_OP(stateInfo,);
        }
    }

    // Convert to the result
    foreach (char firstChar, alphabetChars) {
        foreach (char secondChar, alphabetChars) {
            qint64 count = (qint64) dinuclOccurrence[DI_NUCL_CODE(firstChar, secondChar)];
            if (count == 0) {
                continue;
            }
            QByteArray dinucl;
            dinucl.append(firstChar);
            dinucl.append(secondChar);
            result[dinucl] = count;
        }
    }
}

} // namespace
