/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "U2AssemblyInsertionsMap.h"

#include <algorithm>

#include <U2Core/U2AssemblyReadIterator.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const int U2AssemblyInsertionsMap::MAX_SITE_WIDTH = 20;
const char U2AssemblyInsertionsMap::GAP_CHAR = '-';

void U2AssemblyInsertionsMap::clear() {
    region = U2Region();
    sites.clear();
    totalWidth = 0;
    cumulativeWidths.clear();
    widthByRefPos.clear();
    siteIndexByRefPos.clear();
    letterFrequencies.clear();
}

namespace {

/** Part of the region the read covers, empty when the read has nothing inside the region. */
U2Region getVisibleBases(const U2AssemblyRead& read, const U2Region& region) {
    CHECK(read.data() != nullptr, U2Region());
    U2Region readBases(read->leftmostPos, U2AssemblyUtils::getEffectiveReadLength(read));
    return readBases.intersect(region);
}

/** Index of the first site at or after the reference position, sites.size() when there is none. */
int findFirstSiteAt(const QList<U2AssemblyInsertionSite>& sites, qint64 refPos) {
    auto it = std::lower_bound(sites.constBegin(), sites.constEnd(), refPos, [](const U2AssemblyInsertionSite& site, qint64 pos) {
        return site.refPos < pos;
    });
    return (int)(it - sites.constBegin());
}

}  // namespace

void U2AssemblyInsertionsMap::build(const QList<U2AssemblyRead>& reads, const U2Region& regionToBuild) {
    startBuild(regionToBuild);
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        addSitesOfRead(read);
    }
    finishSites();
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        addStatisticsOfRead(read);
    }
    finishStatistics();
}

void U2AssemblyInsertionsMap::startBuild(const U2Region& regionToBuild) {
    clear();
    region = regionToBuild;
}

void U2AssemblyInsertionsMap::addSitesOfRead(const U2AssemblyRead& read) {
    CHECK(!region.isEmpty(), );

    U2Region visibleBases = getVisibleBases(read, region);
    CHECK(!visibleBases.isEmpty(), );

    // A read with no CIGAR is shown as a plain match, so it has nothing to contribute here.
    CHECK(!read->cigar.isEmpty(), );

    U2AssemblyReadIterator it(read->readSequence, read->cigar, visibleBases.startPos - read->leftmostPos);
    for (qint64 refPos = visibleBases.startPos; refPos < visibleBases.endPos() && it.hasNext(); refPos++) {
        it.nextLetter();
        int insertionLength = it.getInsertionBeforeLastLetter().size();
        CHECK_CONTINUE(insertionLength > 0);

        int width = qMin(insertionLength, MAX_SITE_WIDTH);
        widthByRefPos[refPos] = qMax(widthByRefPos.value(refPos), width);
    }
}

void U2AssemblyInsertionsMap::finishSites() {
    sites.reserve(widthByRefPos.size());
    cumulativeWidths.reserve(widthByRefPos.size());
    letterFrequencies.resize(widthByRefPos.size());
    for (auto it = widthByRefPos.constBegin(); it != widthByRefPos.constEnd(); ++it) {
        U2AssemblyInsertionSite site(it.key(), it.value());
        site.letterCounts.fill(0, site.width);
        letterFrequencies[sites.size()].resize(site.width);
        siteIndexByRefPos.insert(site.refPos, sites.size());
        sites << site;
        totalWidth += site.width;
        cumulativeWidths << totalWidth;
    }
    widthByRefPos.clear();
}

void U2AssemblyInsertionsMap::addStatisticsOfRead(const U2AssemblyRead& read) {
    CHECK(!sites.isEmpty(), );

    U2Region visibleBases = getVisibleBases(read, region);
    CHECK(!visibleBases.isEmpty(), );

    // Every read covering the position votes in the consensus of its columns: the ones that have
    // no letter there are exactly the gaps the extra columns are filled with.
    for (int i = findFirstSiteAt(sites, visibleBases.startPos); i < sites.size() && sites.at(i).refPos < visibleBases.endPos(); i++) {
        sites[i].coverage++;
    }

    CHECK(!read->cigar.isEmpty(), );

    U2AssemblyReadIterator it(read->readSequence, read->cigar, visibleBases.startPos - read->leftmostPos);
    for (qint64 refPos = visibleBases.startPos; refPos < visibleBases.endPos() && it.hasNext(); refPos++) {
        it.nextLetter();
        const QByteArray& insertion = it.getInsertionBeforeLastLetter();
        CHECK_CONTINUE(!insertion.isEmpty());

        int siteIndex = siteIndexByRefPos.value(refPos, -1);
        CHECK_CONTINUE(siteIndex >= 0);

        U2AssemblyInsertionSite& site = sites[siteIndex];
        // The letters a truncated site has no column for are not shown, so they do not vote either.
        int countedLength = qMin<int>(insertion.size(), site.width);
        for (int offset = 0; offset < countedLength; offset++) {
            site.letterCounts[offset]++;
            letterFrequencies[siteIndex][offset][insertion.at(offset)]++;
        }
    }
}

void U2AssemblyInsertionsMap::finishStatistics() {
    for (int siteIndex = 0; siteIndex < sites.size(); siteIndex++) {
        U2AssemblyInsertionSite& site = sites[siteIndex];
        site.consensus = QByteArray(site.width, GAP_CHAR);
        for (int offset = 0; offset < site.width; offset++) {
            // The reads that cover the position but put nothing into the column vote for a gap,
            // and win a tie: the column is only worth a letter when the letter really prevails.
            qint64 bestCount = site.coverage - site.letterCounts[offset];
            const QMap<char, qint64>& frequencies = letterFrequencies[siteIndex][offset];
            for (auto it = frequencies.constBegin(); it != frequencies.constEnd(); ++it) {
                if (it.value() > bestCount) {
                    bestCount = it.value();
                    site.consensus[offset] = it.key();
                }
            }
        }
    }
    letterFrequencies.clear();
}

const U2AssemblyInsertionSite* U2AssemblyInsertionsMap::getSiteBefore(qint64 refPos) const {
    int siteIndex = siteIndexByRefPos.value(refPos, -1);
    return siteIndex < 0 ? nullptr : &sites.at(siteIndex);
}

int U2AssemblyInsertionsMap::getInsertionWidthBefore(qint64 refPos) const {
    const U2AssemblyInsertionSite* site = getSiteBefore(refPos);
    return site == nullptr ? 0 : site->width;
}

QByteArray U2AssemblyInsertionsMap::getInsertionConsensus(qint64 refPos) const {
    const U2AssemblyInsertionSite* site = getSiteBefore(refPos);
    return site == nullptr ? QByteArray() : site->consensus;
}

qint64 U2AssemblyInsertionsMap::getInsertionCoverage(qint64 refPos, int offset) const {
    const U2AssemblyInsertionSite* site = getSiteBefore(refPos);
    CHECK(site != nullptr && offset >= 0 && offset < site->letterCounts.size(), 0);
    return site->letterCounts.at(offset);
}

qint64 U2AssemblyInsertionsMap::getColumnOfRefPos(qint64 refPos) const {
    // Every site at or before the position pushes it that many columns to the right.
    int sitesBefore = findFirstSiteAt(sites, refPos + 1);
    qint64 extraColumns = sitesBefore == 0 ? 0 : cumulativeWidths.at(sitesBefore - 1);
    return refPos - region.startPos + extraColumns;
}

qint64 U2AssemblyInsertionsMap::getColumnCount() const {
    return region.length + totalWidth;
}

qint64 U2AssemblyInsertionsMap::getRefPosOfColumn(qint64 column) const {
    qint64 refPos = region.startPos;
    qint64 columnsLeft = column;
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        qint64 referenceColumns = site.refPos - refPos;
        if (columnsLeft < referenceColumns) {
            break;
        }
        columnsLeft -= referenceColumns;
        refPos = site.refPos;
        if (columnsLeft < site.width) {
            return refPos;
        }
        columnsLeft -= site.width;
    }
    return refPos + columnsLeft;
}

int U2AssemblyInsertionsMap::getInsertionOffsetOfColumn(qint64 column) const {
    qint64 refPos = region.startPos;
    qint64 columnsLeft = column;
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        qint64 referenceColumns = site.refPos - refPos;
        if (columnsLeft < referenceColumns) {
            break;
        }
        columnsLeft -= referenceColumns;
        refPos = site.refPos;
        if (columnsLeft < site.width) {
            return (int)columnsLeft;
        }
        columnsLeft -= site.width;
    }
    return -1;
}

}  // namespace U2
