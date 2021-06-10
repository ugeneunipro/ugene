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

namespace U2 {

FindUnwantedIslandsTask::FindUnwantedIslandsTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings, const QByteArray& _sequence)
    : Task("", TaskFlags_FOSCOE),
      settings(_settings),
      sequence(_sequence) {}

void FindUnwantedIslandsTask::run() {
    /**
     * Index of the left nucleotide in the searching area.
     */
    int leftNucleotide = settings.leftArea.startPos;
    /**
     * Index of the right nucleotide in the searching area.
     * @settings.overlapLength.maxValue is the area extending deep into the amplified fragment.
     */
    const int rightNucleotide = settings.leftArea.endPos() + settings.overlapLength.maxValue;
    int lengthBetweenIslands = 0;
    int startNucleotideNumber = leftNucleotide;
    regionsBetweenIslands.clear();
    while (leftNucleotide /*+ ISLAND_LENGTH*/ <= rightNucleotide) {
        U2Region islandCandidate(leftNucleotide, ISLAND_LENGTH);
        bool isIsland = hasUnwantedConnections(islandCandidate);
        if (isIsland) {
            //The obvious limit - we don't need regions which couldn't fit the primer
            if (lengthBetweenIslands != 0/*>= overlap.minValue*/) {
                regionsBetweenIslands << U2Region(startNucleotideNumber, lengthBetweenIslands);
            }
            startNucleotideNumber = leftNucleotide;
            lengthBetweenIslands = 0;
        } else {
            lengthBetweenIslands++;
        }
        leftNucleotide++;
    }
    regionsBetweenIslands << U2Region(startNucleotideNumber, lengthBetweenIslands);
    // Sort sequence FROM the amplified fragment
    std::sort(regionsBetweenIslands.begin(), regionsBetweenIslands.end(),
        [](const U2Region& first, const U2Region& second) -> bool {
        return first.startPos > second.startPos;
    });
}

const QList<U2Region>& FindUnwantedIslandsTask::getRegionBetweenIslands() const {
    return regionsBetweenIslands;
}

bool FindUnwantedIslandsTask::isUnwantedSelfDimer(const QByteArray& forwardSequence) {
    PrimerStatisticsCalculator calc(forwardSequence, PrimerStatisticsCalculator::Direction::DoesntMatter, UNWANTED_DELTA_G);
    auto dimersInfo = calc.getDimersInfo();
    if (dimersInfo.dimersOverlap.isEmpty()) { // Self dimers aren't found
        return false;
    }

    double dimerMeltingTemp = PrimerStatistics::getMeltingTemperature(dimersInfo.dimer.toLocal8Bit());
    int dimerLength = dimersInfo.dimer.length();
    bool goodMeltingTemperature = dimerMeltingTemp < UNWANTED_MELTING_TEMPERATURE;
    bool goodLength = dimerLength < UNWANTED_MAX_LENGTH;

    return dimersInfo.canBeFormed && goodMeltingTemperature && goodLength;
}

bool FindUnwantedIslandsTask::hasUnwantedConnections(const U2Region& region) const {
    /**
     * Sequence to find the unwanted connections in.
     */
    QByteArray regionSequence = sequence.mid(region.startPos, region.length);
    /**
     * It's reverse complement representation.
     */
    QByteArray revComRegionSequence = DNASequenceUtils::reverseComplement(regionSequence);

    bool isSelfDimer = isUnwantedSelfDimer(regionSequence);

    return isSelfDimer;

    /*for (int i = 0; i < region.length - NUCLEOTIDE_PAIR_LENGTH; i++) {
        const auto& possibleConnection = regionSequence.mid(i, NUCLEOTIDE_PAIR_LENGTH);
        for (int j = 0; j < region.length - NUCLEOTIDE_PAIR_LENGTH; j++) {
            CHECK_CONTINUE(i != region.length - (j + NUCLEOTIDE_PAIR_LENGTH));

            const auto& revComppossibleConnection = revComRegionSequence.mid(j, NUCLEOTIDE_PAIR_LENGTH);
            if (possibleConnection == revComppossibleConnection) {
                return true;
            }
        }
    */
    //return false;
}


}