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

#ifndef _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_SETTINGS_H_
#define _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_SETTINGS_H_

#include <QString>

#include <U2Core/U2Range.h>
#include <U2Core/U2Region.h>

namespace U2 {

struct PCRPrimerDesignForDNAAssemblyTaskSettings {
    //User primers
    QString forwardUserPrimer;
    QString reverseUserPrimer;

    //Parameters of priming
    U2Range<int> gibbsFreeEnergy;
    U2Range<int> meltingPoint;
    U2Range<int> overlapLength;

    //Parameters to exclude in whole primers
    int gibbsFreeEnergyExclude = 0;
    int meltingPointExclude = 0;
    int complementLengthExclude = 0;

    //Areas fpr priming search
    //Insert to backbone bearings
    enum class BackboneBearings {
        Backbone5,
        Backbone3
    };
    BackboneBearings insertTo = BackboneBearings::Backbone5;
    int bachbone5Length = 0;
    int bachbone3Length = 0;

    //Areas
    U2Region leftArea;
    U2Region rightArea;

    //Backbone sequence
    QString backboneSequenceUrl;

    //Other sequences in PCR
    QString otherSequencesInPcrUrl;
};

}

#endif
