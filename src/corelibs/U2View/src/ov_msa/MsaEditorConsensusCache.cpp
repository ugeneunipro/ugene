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

#include "MsaEditorConsensusCache.h"

#include <U2Algorithm/MSAConsensusAlgorithm.h>

#include <U2Core/MsaObject.h>
#include <U2Core/U2SafePoints.h>

#include "MaEditor.h"

namespace U2 {

MsaEditorConsensusCache::MsaEditorConsensusCache(QObject* p, MsaObject* o, MSAConsensusAlgorithmFactory* factory)
    : QObject(p), curCacheSize(0), aliObj(o), algorithm(nullptr) {
    setConsensusAlgorithm(factory);

    connect(aliObj, SIGNAL(si_alignmentChanged(const Msa&, const MaModificationInfo&)), SLOT(sl_alignmentChanged()));
    connect(aliObj, SIGNAL(si_invalidateAlignmentObject()), SLOT(sl_invalidateAlignmentObject()));

    curCacheSize = aliObj->getLength();
    updateMap.resize(curCacheSize);
    cache.resize(curCacheSize);
}

MsaEditorConsensusCache::~MsaEditorConsensusCache() {
    delete algorithm;
    algorithm = nullptr;
}

void MsaEditorConsensusCache::setConsensusAlgorithm(MSAConsensusAlgorithmFactory* factory) {
    delete algorithm;
    algorithm = nullptr;
    bool ignoreTrailingLeadingGaps = aliObj->getGObjectType() == GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT;
    algorithm = factory->createAlgorithm(aliObj->getAlignment(), ignoreTrailingLeadingGaps);
    connect(algorithm, SIGNAL(si_thresholdChanged(int)), SLOT(sl_thresholdChanged(int)));
    updateMap.fill(false);
}

QByteArray MsaEditorConsensusCache::getConsensusLine(const U2Region& region, bool withGaps) {
    QByteArray res;
    for (int i = static_cast<int>(region.startPos), n = static_cast<int>(region.endPos()); i < n; i++) {
        char c = getConsensusChar(i);
        if (c != U2Msa::GAP_CHAR || withGaps) {
            res.append(c);
        }
    }
    return res;
}

void MsaEditorConsensusCache::sl_alignmentChanged() {
    algorithm->reinitializeData(aliObj->getAlignment());

    if (curCacheSize != aliObj->getLength()) {
        curCacheSize = aliObj->getLength();
        updateMap.resize(curCacheSize);
        cache.resize(curCacheSize);

        emit si_cacheResized(curCacheSize);
    }
    updateMap.fill(false);
}

void MsaEditorConsensusCache::updateCacheItem(int pos) {
    if (!updateMap.at(pos) && aliObj != nullptr) {
        const Msa ma = aliObj->getAlignment();

        QString errorMessage = "Can not update consensus cache item";
        SAFE_POINT(pos >= 0 && pos < curCacheSize, errorMessage, );
        SAFE_POINT(curCacheSize == ma->getLength(), errorMessage, );

        CacheItem& ci = cache[pos];
        int count = 0;
        int nSeq = ma->getRowCount();
        SAFE_POINT(nSeq != 0, errorMessage, );

        ci.topChar = algorithm->getConsensusCharAndScore(ma, pos, count);
        ci.topPercent = (char)qRound(count * 100. / nSeq);
        assert(ci.topPercent >= 0 && ci.topPercent <= 100);
        updateMap.setBit(pos, true);

        emit si_cachedItemUpdated(pos, ci.topChar);
    }
}

char MsaEditorConsensusCache::getConsensusChar(int pos) {
    updateCacheItem(pos);
    const CacheItem& ci = cache[pos];
    return ci.topChar;
}

int MsaEditorConsensusCache::getConsensusCharPercent(int pos) {
    updateCacheItem(pos);
    const CacheItem& ci = cache[pos];
    return ci.topPercent;
}

QList<int> MsaEditorConsensusCache::getConsensusPercents(const U2Region& region) {
    QList<int> percents;
    for (qint64 column = region.startPos; column < region.endPos(); column++) {
        percents << getConsensusCharPercent(static_cast<int>(column));
    }
    return percents;
}

QByteArray MsaEditorConsensusCache::getConsensusLine(bool withGaps) {
    const Msa ma = aliObj->getAlignment();
    const U2Region region(0, ma->getLength());
    return getConsensusLine(region, withGaps);
}

void MsaEditorConsensusCache::sl_thresholdChanged(int newValue) {
    Q_UNUSED(newValue);
    updateMap.fill(false);
}

void MsaEditorConsensusCache::sl_invalidateAlignmentObject() {
    aliObj = nullptr;
}

}  // namespace U2
