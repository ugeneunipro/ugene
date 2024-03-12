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

#include "SamtoolsBasedAllReadsIterator.h"

namespace U2 {

namespace BAM {

SamtoolsBasedAllReadsIterator::SamtoolsBasedAllReadsIterator(int _assemblyId, SamtoolsBasedDbi& _dbi)
    : SamtoolsBasedReadsIterator(_assemblyId, _dbi) {
}

U2AssemblyRead SamtoolsBasedAllReadsIterator::next() {
    FAIL("This function shouldn't be called", U2AssemblyRead());

    return U2AssemblyRead();
}

qint64 SamtoolsBasedAllReadsIterator::calculateMaxEndPos() {
    auto read = SamtoolsBasedReadsIterator::next();
    qint64 result = 0;
    while (read != nullptr) {
        result = qMax(result, read->leftmostPos + read->effectiveLen);
        read = SamtoolsBasedReadsIterator::next();
    }
    return result;
}

void SamtoolsBasedAllReadsIterator::fetchReads() {
    int readsSize = reads.size();
    do {
        readsSize = reads.size();
        fetchNextChunk();
        applyNameFilter();
    } while (readsSize != reads.size());
}

}

}
