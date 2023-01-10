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

#include "EntropyAlgorithm.h"
#include <math.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/TextUtils.h>

#include "DNAGraphPackPlugin.h"

/* TRANSLATOR U2::EntropyGraphFactory */

/**
 * entropy = sum (p * log(2) p) where p - frequency of dif codons
 * entropy is a measure of randomness, it mean that coding sequences
 * have entropy lower than random sequence
 **/

namespace U2 {

static QString nameByType() {
    return EntropyGraphFactory::tr("Informational Entropy");
}

EntropyGraphFactory::EntropyGraphFactory(QObject* p)
    : GSequenceGraphFactory(nameByType(), p) {
}

#define MAX_CHARS_IN_ALPHABET 7

bool EntropyGraphFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic() && al->getAlphabetChars().size() <= MAX_CHARS_IN_ALPHABET;
}

QList<QSharedPointer<GSequenceGraphData>> EntropyGraphFactory::createGraphs(GSequenceGraphView* view) {
    assert(isEnabled(view->getSequenceObject()));
    return {QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(view, graphName, new EntropyGraphAlgorithm()))};
}

//////////////////////////////////////////////////////////////////////////
// EntropyGraphAlgorithm

void EntropyGraphAlgorithm::calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) {
    U2Region vr(0, sequenceObject->getSequenceLength());
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, window, step);
    result.reserve(nSteps);

    QByteArray seq = sequenceObject->getWholeSequenceData(os);
    CHECK_OP(os, );
    const DNAAlphabet* al = sequenceObject->getAlphabet();

    // prepare index -> TODO: make it once and cache!
    IndexedMapping3To1<int> index(al->getAlphabetChars(), 0);
    int* mapData = index.mapData();
    int indexSize = index.getMapSize();

    // algorithm
    float log10_2 = log10(2.0);
    const char* seqStr = seq.constData();
    for (int i = 0; i < nSteps; i++) {
        int start = vr.startPos + i * step;
        int end = start + window;
        for (int x = start; x < end - 2; x++) {
            int& val = index.mapNC(seqStr + x);
            val++;
        }
        // derive entropy from triplets and zero them
        float total = end - start - 2;
        float ent = 0;
        for (int j = 0; j < indexSize; j++) {
            CHECK_OP(os, );
            int ifreq = mapData[j];
            if (ifreq == 0) {
                continue;
            }
            mapData[j] = 0;  // zero triplets
            float freq = ifreq / total;
            ent -= freq * log10(freq) / log10_2;
        }
        result.append(ent);
    }
}

}  // namespace U2
