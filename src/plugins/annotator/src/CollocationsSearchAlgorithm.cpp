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

#include "CollocationsSearchAlgorithm.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

void CollocationsAlgorithm::find(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si, CollocationsAlgorithmListener* l, const CollocationsAlgorithmSettings& cfg) {
    cfg.st == NormalSearch ? findN(items, si, l, cfg.searchRegion, cfg.distance) : findP(items, si, l, cfg.searchRegion, cfg.distance);
}

void CollocationsAlgorithm::findN(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si, CollocationsAlgorithmListener* l, const U2Region& searchRegion, qint64 distance) {
    // todo: progress

    qint64 i = searchRegion.endPos();
    foreach (const CollocationsAlgorithmItem& item, items) {
        for (const U2Region& r : qAsConst(item.regions)) {
            SAFE_POINT(searchRegion.contains(r), "Region is not in the search region", );
            i = qMin(i, r.startPos);
        }
    }
    if (i == searchRegion.endPos()) {
        return;
    }

    U2Region prevResult;
    do {
        U2Region res;
        U2Region currentRegion(i, qMin(i + distance, searchRegion.endPos()) - i);
        bool onResult = true;
        qint64 nextI = currentRegion.endPos();
        for (const CollocationsAlgorithmItem& item : qAsConst(items)) {
            bool foundItem = false;
            qint64 nextItemStart = currentRegion.endPos();
            foreach (const U2Region& r, item.regions) {
                if (r.startPos > currentRegion.startPos) {
                    nextItemStart = qMin(nextItemStart, r.startPos);
                }
                if (onResult && currentRegion.contains(r)) {
                    foundItem = true;
                    res = res.length == 0 ? r : U2Region::containingRegion(res, r);
                }
            }
            nextI = qMin(nextI, nextItemStart);
            onResult = onResult && foundItem;
        }
        if (onResult && res.startPos == i) {
            SAFE_POINT(res.length > 0, "Result region have zero length.", );
            if (!prevResult.contains(res)) {
                SAFE_POINT(!res.contains(prevResult) || prevResult.length == 0, "Region nested with other.", );
                SAFE_POINT(prevResult.endPos() < res.endPos(), "Result regions order violated.", );
                l->onResult(res);
                prevResult = res;
            }
        }
        SAFE_POINT(nextI > i, "Regions order are violated.", );
        i = nextI;
        si.progress = int(100 * float(i - searchRegion.startPos) / searchRegion.length);
    } while (i + distance < searchRegion.endPos());
}

void averagingRes(U2Region& res, const U2Region& min, const U2Region& max, qint64 distance, const U2Region& searchRegion) {
    //?
    if (!min.intersects(max)) {
        res.startPos = min.endPos() - 1;
        res.length = max.startPos - min.endPos() + 2;
    } else {
        res.startPos = max.startPos;
        res.length = min.endPos() - max.startPos;
    }
    qint64 tmp = distance - res.length;
    res.startPos -= tmp * min.length / (max.length + min.length);
    if (res.startPos < 0)
        res.startPos = 0;
    res.length = distance;
    if (res.endPos() > searchRegion.endPos()) {
        res.startPos -= (res.endPos() - searchRegion.endPos());
    }
    if (res.endPos() > max.endPos()) {
        res.startPos -= (res.endPos() - max.endPos());
    }
    if (res.startPos < 0)
        res.startPos = 0;
}

void CollocationsAlgorithm::findP(const QList<CollocationsAlgorithmItem>& items, TaskStateInfo& si, CollocationsAlgorithmListener* l, const U2Region& searchRegion, qint64 distance) {
    qint64 i = searchRegion.endPos();
    for (const CollocationsAlgorithmItem& item : qAsConst(items)) {
        foreach (const U2Region& r, item.regions) {
            SAFE_POINT(searchRegion.contains(r), "Region is not in the search region", );
            if (i > r.endPos() - 1) {
                i = r.endPos() - 1;
            }
        }
    }
    if (i == searchRegion.endPos()) {
        return;
    }
    U2Region prevMax;
    U2Region prevResult;
    do {
        U2Region res;
        U2Region currentRegion(i, qMin(i + distance, searchRegion.endPos()) - i);
        U2Region min, max;
        min.startPos = searchRegion.endPos() - 1;
        max.startPos = 0;
        bool onResult = true;
        qint64 nextI = currentRegion.endPos();
        for (const CollocationsAlgorithmItem& item : qAsConst(items)) {
            bool foundItem = false;
            qint64 nextItemEnd = searchRegion.endPos();
            foreach (const U2Region& r, item.regions) {
                if (r.endPos() <= searchRegion.endPos() && r.endPos() - 1 > currentRegion.startPos && nextItemEnd > r.endPos() - 1) {
                    nextItemEnd = r.endPos() - 1;
                }
                if (onResult && currentRegion.intersects(r)) {
                    foundItem = true;
                    if (r.endPos() < min.endPos())
                        min = r;
                    if (max < r)
                        max = r;
                    res = res.length == 0 ? r : U2Region::containingRegion(res, r);
                }
            }
            nextI = qMin(nextI, nextItemEnd);
            onResult = onResult && foundItem;
        }
        // error mb use list of prev included anno?
        if (onResult && prevMax != max) {
            prevMax = max;

            if (res.length > distance) {
                averagingRes(res, min, max, distance, searchRegion);
            }
            if (!prevResult.contains(res)) {
                SAFE_POINT(!res.contains(prevResult) || prevResult.length == 0, "Region nested with other.", );
                SAFE_POINT(prevResult.endPos() < res.endPos(), "Result regions order violated.", );
                l->onResult(res);
                prevResult = res;
            }
        }
        SAFE_POINT(nextI > i, "Regions order are violated.", );
        i = nextI;
        si.progress = int(100 * float(i - searchRegion.startPos) / searchRegion.length);
    } while (i < searchRegion.endPos());
}

}  // namespace U2
