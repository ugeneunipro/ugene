/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MsaDistanceAlgorithm.h"

#include <U2Core/DNAAlphabet.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Factory

MsaDistanceAlgorithmFactory::MsaDistanceAlgorithmFactory(const QString& algoId, DistanceAlgorithmFlags _flags, QObject* p)
    : QObject(p), algorithmId(algoId), flags(_flags) {
}

DistanceAlgorithmFlags MsaDistanceAlgorithmFactory::getAlphabetFlags(const DNAAlphabet* al) {
    if (al->getType() == DNAAlphabet_AMINO) {
        return DistanceAlgorithmFlag_Amino;
    } else if (al->getType() == DNAAlphabet_NUCL) {
        return DistanceAlgorithmFlag_Nucleic;
    }
    assert(al->getType() == DNAAlphabet_RAW);
    return DistanceAlgorithmFlag_Raw;
}

void MsaDistanceAlgorithmFactory::setFlag(DistanceAlgorithmFlag flag) {
    flags |= flag;
}

void MsaDistanceAlgorithmFactory::resetFlag(DistanceAlgorithmFlag flag) {
    if (flags.testFlag(flag)) {
        flags ^= flag;
    }
}

//////////////////////////////////////////////////////////////////////////
// Algorithm

MsaDistanceAlgorithm::MsaDistanceAlgorithm(MsaDistanceAlgorithmFactory* _factory, const Msa& _ma)
    : Task(tr("MSA distance algorithm \"%1\" task").arg(_factory->getName()), TaskFlag_None), factory(_factory), ma(_ma->getCopy()), excludeGaps(true), isSimilarity(true) {
    int rowsNumber = ma->getRowCount();
    qint64 requiredMemory = sizeof(int) * rowsNumber * rowsNumber / 2 + sizeof(QVarLengthArray<int>) * rowsNumber;
    bool memoryAcquired = memoryLocker.tryAcquire(requiredMemory);
    CHECK_EXT(memoryAcquired, setError(QString("There is not enough memory to calculating distances matrix, required %1 megabytes").arg(requiredMemory / 1024 / 1024)), );
    distanceMatrix = MsaDistanceMatrix(ma, getExcludeGapsFlag(), false);
}

int MsaDistanceAlgorithm::getSimilarity(int row1, int row2, bool _usePercents) {
    lock.lock();
    int res = distanceMatrix.getSimilarity(row1, row2, _usePercents);
    lock.unlock();
    return res;
}

const MsaDistanceMatrix& MsaDistanceAlgorithm::getMatrix() const {
    QMutexLocker locker(&lock);
    return distanceMatrix;
}

void MsaDistanceAlgorithm::setDistanceValue(int row1, int row2, int distance) {
    if (row2 > row1) {
        distanceMatrix.table[row2][row1] = distance;
    } else {
        distanceMatrix.table[row1][row2] = distance;
    }
}

void MsaDistanceAlgorithm::fillTable() {
    int nSeq = ma->getRowCount();
    for (int i = 0; i < nSeq; i++) {
        for (int j = i; j < nSeq; j++) {
            if (isCanceled()) {
                return;
            }
            int sim = calculateSimilarity(i, j);
            lock.lock();
            setDistanceValue(i, j, sim);
            lock.unlock();
        }
    }
}

void MsaDistanceAlgorithm::setExcludeGaps(bool _excludeGaps) {
    excludeGaps = _excludeGaps;
    distanceMatrix.excludeGaps = _excludeGaps;
}

MsaDistanceMatrix::MsaDistanceMatrix()
    : usePercents(true), excludeGaps(false), alignmentLength(0) {
}

MsaDistanceMatrix::MsaDistanceMatrix(const Msa& ma, bool _excludeGaps, bool _usePercents)
    : usePercents(_usePercents), excludeGaps(_excludeGaps), alignmentLength(ma->getLength()) {
    int nSeq = ma->getRowCount();
    table.reserve(nSeq);
    for (int i = 0; i < nSeq; i++) {
        table.append(QVarLengthArray<int>(i + 1));
        memset(table[i].data(), 0, (i + 1) * sizeof(int));
        seqsUngappedLenghts.append(ma->getRow(i)->getUngappedLength());
    }
}

int MsaDistanceMatrix::getSimilarity(int row1, int row2) const {
    return getSimilarity(row1, row2, usePercents);
}

int MsaDistanceMatrix::getSimilarity(int refRow, int row, bool _usePercents) const {
    if (refRow >= table.size() || row >= table.size()) {
        return -1;
    }
    if (_usePercents) {
        int refSeqLength = excludeGaps ? seqsUngappedLenghts.at(refRow) : alignmentLength;
        if (refRow > row) {
            return qRound((double)table[refRow][row] * 100 / refSeqLength);
        } else {
            return qRound((double)table[row][refRow] * 100 / refSeqLength);
        }
    } else {
        if (refRow > row) {
            return table[refRow][row];
        } else {
            return table[row][refRow];
        }
    }
}

}  // namespace U2
