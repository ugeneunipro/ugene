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

#include "SmithWatermanResultFilters.h"
#include <QtAlgorithms>

namespace U2 {

/***************************************
 * SWRF_EmptyFilter
 ***************************************/
const QString SWRF_EmptyFilter::ID = "none";

// Without any filtering
bool SWRF_EmptyFilter::applyFilter(QList<SmithWatermanResult>* /*lst*/) {
    return true;
}

SmithWatermanResultFilter* SWRF_EmptyFilter::clone() const {
    return new SWRF_EmptyFilter(*this);
}

bool SWRF_EmptyFilter::needErase(
                        const SmithWatermanResult& /*currItem*/,
                        const SmithWatermanResult& /*someItem*/) const {
    return false;
}

QString SWRF_EmptyFilter::getId() const {
    return ID;
}

/***************************************
* SWRF_WithoutIntersect
***************************************/
const QString SWRF_WithoutIntersect::ID = "filter-intersections";

//greater score goes first
static bool revScoreComparator(const SmithWatermanResult &s0, const SmithWatermanResult &s1) {
    bool res = false;
    if (s0.score < s1.score) {
        res = false;
    } else if (s0.score == s1.score) {
        U2Region r0 = s0.refSubseq;
        U2Region r1 = s1.refSubseq;
        res = r0 < r1;
    } else {
        res = true;
    }
    return res;
}
// Filtering all intersects result
bool SWRF_WithoutIntersect::applyFilter(QList<SmithWatermanResult>* lst) {
    QList<SmithWatermanResult>& results = *lst;

    qSort(results.begin(), results.end(), revScoreComparator);

    int i = 0;
    int size = results.size();
    while (i < size) {
        int j = i + 1;
        while (j < size) {
            const SmithWatermanResult& currItem = results[i];
            const SmithWatermanResult& someItem = results[j];
            if (needErase(currItem, someItem)) {
                results.removeAt(j);
                size--;
                continue;
            }
            j++;
        }
        i++;
    }
    return true;
}

SmithWatermanResultFilter* SWRF_WithoutIntersect::clone() const {
    return new SWRF_WithoutIntersect(*this);
}

bool SWRF_WithoutIntersect::needErase(
                        const SmithWatermanResult& currItem,
                        const SmithWatermanResult& someItem) const {
    U2Region currRegion = currItem.refSubseq;
    U2Region someRegion = someItem.refSubseq;
    if (currRegion.intersects(someRegion) &&
        currItem.strand == someItem.strand) {
        return true;
    }
    return false;
}

QString SWRF_WithoutIntersect::getId() const {
    return ID;
}

} // namesapce U2
