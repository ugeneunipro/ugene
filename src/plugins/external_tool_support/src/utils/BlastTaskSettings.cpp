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

#include "BlastTaskSettings.h"

namespace U2 {

BlastTaskSettings::BlastTaskSettings() {
    offsInGlobalSeq = 0;
    aobj = NULL;
    alphabet = NULL;
    isNucleotideSeq = false;
    numberOfHits = 0;
    isDefaultMatrix = true;
    isDefautScores = true;
    isDefaultCosts = true;
    isGappedAlignment = true;
    isDefaultThreshold = true;
    isSequenceCircular = false;
    reset();
}

void BlastTaskSettings::reset() {
    programName = "";
    databaseNameAndPath = "";
    expectValue = 10.0;
    megablast = false;
    wordSize = 11;
    gapOpenCost = 0;
    gapExtendCost = 0;
    mismatchPenalty = 0;
    matchReward = 0;
    matrix = "";
    queryFile = "";
    outputType = 0;
    outputResFile = "";
    numberOfProcessors = 1;
    needCreateAnnotations = true;
    strandSource = QueryFrame;
    filter = "";
    isGappedAlignment = true;
    xDropoffGA = 0;
    xDropoffUnGA = 0;
    xDropoffFGA = 0;
    windowSize = 0;
    threshold = 0;
    isDefaultThreshold = true;
    isSequenceCircular = false;
    directStrand = TriState_Unknown;
    querySequenceObject = NULL;
}

}//namespace
