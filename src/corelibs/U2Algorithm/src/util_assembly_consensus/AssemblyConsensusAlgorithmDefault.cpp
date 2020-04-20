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

#include "AssemblyConsensusAlgorithmDefault.h"
#include "BuiltInAssemblyConsensusAlgorithms.h"
#include "AssemblyConsensusUtils.h"

#include <U2Core/Log.h>
#include <U2Core/U2AssemblyReadIterator.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

AssemblyConsensusAlgorithmFactoryDefault::AssemblyConsensusAlgorithmFactoryDefault()
    : AssemblyConsensusAlgorithmFactory(BuiltInAssemblyConsensusAlgorithms::DEFAULT_ALGO)
{}

QString AssemblyConsensusAlgorithmFactoryDefault::getName() const {
    return tr("Default");
}

QString AssemblyConsensusAlgorithmFactoryDefault::getDescription() const {
    return tr("Returns simply the most frequent base and 'N' are no reads intersecting this position");
}

AssemblyConsensusAlgorithm* AssemblyConsensusAlgorithmFactoryDefault::createAlgorithm() {
    return new AssemblyConsensusAlgorithmDefault(this);
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

QByteArray AssemblyConsensusAlgorithmDefault::getConsensusRegion(const U2Region &region, U2DbiIterator<U2AssemblyRead> *reads, QByteArray /*referenceFragment*/, U2OpStatus &os) {
    AssemblyBasesFrequenciesStat s;
    s.frequencyInfos.resize(region.length);

    while(reads->hasNext()) {
        U2AssemblyRead r = reads->next();
        U2Region readRegion = U2Region(r->leftmostPos, r->effectiveLen);
        U2Region readCroppedRegion = readRegion.intersect(region);

        qint64 offsetInRead = readCroppedRegion.startPos - readRegion.startPos;
        qint64 offsetInArray = readCroppedRegion.startPos - region.startPos;
        qint64 length = readCroppedRegion.length;

        U2AssemblyReadIterator readIterator(r->readSequence, r->cigar, offsetInRead);

        for(int i = 0; i < length; ++i) {
            U2AssemblyBasesFrequenciesInfo &fi = s.frequencyInfos[offsetInArray + i];
            if(readIterator.hasNext()) {
                char c = readIterator.nextLetter();
                fi.addToCharFrequency(c);
            } else {
                break;
            }
        }
        // Support canceling
        if(os.isCoR()) {
            break;
        }
    }

    return s.getConsensusFragment();
}

} // namespace
