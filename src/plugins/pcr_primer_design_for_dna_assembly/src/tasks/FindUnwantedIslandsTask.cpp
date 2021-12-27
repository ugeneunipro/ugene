/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "FindUnwantedIslandsTask.h"

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/PrimerDimersFinder.h>
#include <U2Core/PrimerStatistics.h>
#include <U2Core/U2SafePoints.h>

#include "utils/UnwantedConnectionsUtils.h"

namespace U2 {

FindUnwantedIslandsTask::FindUnwantedIslandsTask(const U2Region& _searchArea, int _possibleOverlap, const QByteArray& _sequence, bool _isComplement)
    : Task(tr("Find Unwanted Islands Task"), TaskFlags_FOSCOE),
      searchArea(_searchArea),
      possibleOverlap(_possibleOverlap),
      sequence(_sequence),
      isComplement(_isComplement) {}

void FindUnwantedIslandsTask::run() {
    taskLog.details(tr("Searching of unwanted islands and areas between them "
                       "in the region \"%1\" (+ %2 nucleotides to the %3, deep into the amplified fragment) has been started")
                        .arg(regionToString(searchArea))
                        .arg(possibleOverlap)
                        .arg(isComplement ? tr("left") : tr("right")));
    taskLog.details(tr("The following unwanted parameters are used. Free Gibbs energy: %1 kj/mol, "
                       "melting temperature: %2 C, maximum dimer length: %3 nt")
                       .arg(UNWANTED_DELTA_G).arg(UNWANTED_MELTING_TEMPERATURE).arg(UNWANTED_MAX_LENGTH));
    /**
     * Index of the left nucleotide in the searching area.
     */
    int leftNucleotide = searchArea.startPos;
    /**
     * Index of the right nucleotide in the searching area.
     * @settings.overlapLength.maxValue is the area extending deep into the amplified fragment.
     */
    const int rightNucleotide = searchArea.endPos() + possibleOverlap;
    auto text2LogAboutFoundRegion = [this](const U2Region& reg) {
        taskLog.details(tr("The region between unwanted islands has been found: %1").arg(regionToString(reg)));
    };

    int lengthBetweenIslands = 0;
    int startNucleotideNumber = leftNucleotide;
    regionsBetweenIslands.clear();
    stateInfo.setProgress(0);
    int progressZeroPoint = startNucleotideNumber;
    while (leftNucleotide /*+ ISLAND_LENGTH*/ <= rightNucleotide) {
        U2Region islandCandidate(leftNucleotide, ISLAND_LENGTH);
        bool isIsland = hasUnwantedConnections(islandCandidate);
        if (isIsland) {
            if (lengthBetweenIslands != 0) {
                U2Region newRegion(startNucleotideNumber, lengthBetweenIslands);
                text2LogAboutFoundRegion(newRegion);
                regionsBetweenIslands << newRegion;
            }
            startNucleotideNumber = leftNucleotide;
            lengthBetweenIslands = 0;
        } else {
            lengthBetweenIslands++;
        }
        leftNucleotide++;
        stateInfo.setProgress(100 - (double(rightNucleotide - leftNucleotide) / qMax((double)(rightNucleotide - progressZeroPoint), 1.0) * 100));
    }
    stateInfo.setProgress(100);
    U2Region newRegion(startNucleotideNumber, lengthBetweenIslands);
    text2LogAboutFoundRegion(newRegion);
    regionsBetweenIslands << U2Region(startNucleotideNumber, lengthBetweenIslands);

    if (!regionsBetweenIslands.isEmpty()) {
        QString regions;
        for (const auto& region : qAsConst(regionsBetweenIslands)) {
            regions += QString("%1,").arg(regionToString(region));
        }
        regions = regions.left(regions.size() - 1);
        taskLog.details(tr("The following regions are located between unwanted islands: %1").arg(regions));
    } else {
        taskLog.details(tr("The whole region is filled with unwanted islands, no regions between them has been found"));
    }

    // Sort sequence FROM the amplified fragment
    std::sort(regionsBetweenIslands.begin(), regionsBetweenIslands.end(),
        [](const U2Region& first, const U2Region& second) -> bool {
        return first.startPos > second.startPos;
    });
}

const QList<U2Region>& FindUnwantedIslandsTask::getRegionBetweenIslands() const {
    return regionsBetweenIslands;
}

bool FindUnwantedIslandsTask::hasUnwantedConnections(const U2Region& region) const {
    /**
     * Sequence to find the unwanted connections in.
     */
    QByteArray regionSequence = sequence.mid(region.startPos, region.length);
    /**
     * It's reverse complement representation.
     */
    //QByteArray revComRegionSequence = DNASequenceUtils::reverseComplement(regionSequence);
    bool isUnwantedSelfDimer = UnwantedConnectionsUtils::isUnwantedSelfDimer(regionSequence,
                                                                             UNWANTED_DELTA_G,
                                                                             UNWANTED_MELTING_TEMPERATURE,
                                                                             UNWANTED_MAX_LENGTH);

    //TODO: hairpins

    return isUnwantedSelfDimer;
}

QString FindUnwantedIslandsTask::regionToString(const U2Region& region) const {
    U2Region regionToLog = isComplement ? DNASequenceUtils::reverseComplementRegion(region, sequence.size()) : region;
    return QString("%1..%2").arg(regionToLog.startPos + 1).arg(regionToLog.endPos() + 1);
}


}
