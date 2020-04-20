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

#include "MSADistanceAlgorithmHamming.h"

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

MSADistanceAlgorithmFactoryHamming::MSADistanceAlgorithmFactoryHamming(QObject* p)
: MSADistanceAlgorithmFactory(BuiltInDistanceAlgorithms::HAMMING_ALGO, DistanceAlgorithmFlags_NuclAmino, p)
{

}

QString MSADistanceAlgorithmFactoryHamming::getDescription() const {
    return tr("Based on Hamming distance between two sequences");
}

QString MSADistanceAlgorithmFactoryHamming::getName() const {
    return tr("Hamming dissimilarity");
}


MSADistanceAlgorithm* MSADistanceAlgorithmFactoryHamming::createAlgorithm(const MultipleSequenceAlignment& ma, QObject* ) {
    MSADistanceAlgorithm* res = new MSADistanceAlgorithmHamming(this, ma);
     if(flags.testFlag(DistanceAlgorithmFlag_ExcludeGaps)){
        res->setExcludeGaps(true);
    }else{
         res->setExcludeGaps(false);
     }
    return res;
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

void MSADistanceAlgorithmHamming::run() {
    int nSeq = ma->getNumRows();
    for (int i = 0; i < nSeq; i++) {
        for (int j = i; j < nSeq; j++) {
            int sim = 0;
            for (int k = 0; k < ma->getLength(); k++) {
                if (isCanceled()) {
                    return;
                }
                bool dissimilar = (ma->charAt(i, k) != ma->charAt(j, k));

                if(!excludeGaps){
                    if (dissimilar) sim++;
                }else{
                    if (dissimilar && (ma->charAt(i, k)!=U2Msa::GAP_CHAR && ma->charAt(j, k)!=U2Msa::GAP_CHAR)) sim++;
                }
            }
            lock.lock();
            setDistanceValue(i, j, sim);
            lock.unlock();
        }
        stateInfo.setProgress(i * 100 / nSeq);
    }
}

} //namespace
