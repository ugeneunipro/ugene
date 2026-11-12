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
#include <cctype>

#include <U2Core/U2AssemblyReadIterator.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const double U2AssemblyInsertionsFilter::DEFAULT_MIN_PERCENT = 2;
const int U2AssemblyInsertionsFilter::DEFAULT_MIN_READS = 10;

U2AssemblyInsertionsFilter U2AssemblyInsertionsFilter::getDefault() {
    return U2AssemblyInsertionsFilter(DEFAULT_MIN_PERCENT, DEFAULT_MIN_READS);
}

bool U2AssemblyInsertionsFilter::isValid() const {
    // Written so that NaN is rejected as well: every comparison with it is false.
    return minPercent >= 0 && minPercent <= 100 && minReads >= 0;
}

bool U2AssemblyInsertionsFilter::isEnabled() const {
    // Every insertion is found in at least one read, so a single read is no requirement at all.
    return minPercent > 0 || minReads > 1;
}

bool U2AssemblyInsertionsFilter::passes(qint64 reads, qint64 coverage) const {
    CHECK(reads >= minReads, false);
    // A small tolerance keeps the exact threshold passing, e.g. 1 read of 3 at 33.333333%, where
    // the percent typed in by the user is a rounded decimal and the product is not exact either.
    static const double EPSILON = 1e-9;
    return reads * 100.0 >= minPercent * coverage - EPSILON * coverage;
}

const int U2AssemblyInsertionsMap::MAX_SITE_WIDTH = 20;
const char U2AssemblyInsertionsMap::GAP_CHAR = '-';

void U2AssemblyInsertionsMap::clear() {
    region = U2Region();
    filter = U2AssemblyInsertionsFilter();
    sites.clear();
    totalWidth = 0;
    cumulativeWidths.clear();
    widthByRefPos.clear();
    siteIndexByRefPos.clear();
    letterFrequencies.clear();
    readsByInsertion.clear();
}

namespace {

/** Part of the region the read covers, empty when the read has nothing inside the region. */
U2Region getVisibleBases(const U2AssemblyRead& read, const U2Region& region) {
    CHECK(read.data() != nullptr, U2Region());
    U2Region readBases(read->leftmostPos, U2AssemblyUtils::getEffectiveReadLength(read));
    return readBases.intersect(region);
}

/**
 * Calls 'callback(refPos, insertion)' for every position of the region the read has an insertion in
 * front of. The iterator only reports an insertion once the base before it has been walked through,
 * and it does not report the ones it skips while seeking to the start: so the walk starts one base
 * earlier than the region, otherwise a read starting before the region would lose the insertion in
 * front of the very first position of it. That extra base is only walked through, never reported.
 */
template <class Callback>
void forEachInsertion(const U2AssemblyRead& read, const U2Region& visibleBases, Callback callback) {
    qint64 firstWalkedPos = qMax(read->leftmostPos, visibleBases.startPos - 1);
    U2AssemblyReadIterator it(read->readSequence, read->cigar, firstWalkedPos - read->leftmostPos);
    for (qint64 refPos = firstWalkedPos; refPos < visibleBases.endPos() && it.hasNext(); refPos++) {
        it.nextLetter();
        const QByteArray& insertion = it.getInsertionBeforeLastLetter();
        if (refPos >= visibleBases.startPos && !insertion.isEmpty()) {
            callback(refPos, insertion);
        }
    }
}

/** Index of the first site at or after the reference position, sites.size() when there is none. */
int findFirstSiteAt(const QList<U2AssemblyInsertionSite>& sites, qint64 refPos) {
    auto it = std::lower_bound(sites.constBegin(), sites.constEnd(), refPos, [](const U2AssemblyInsertionSite& site, qint64 pos) {
        return site.refPos < pos;
    });
    return (int)(it - sites.constBegin());
}

}  // namespace

void U2AssemblyInsertionsMap::build(const QList<U2AssemblyRead>& reads, const U2Region& regionToBuild, const U2AssemblyInsertionsFilter& filterToApply) {
    startBuild(regionToBuild, filterToApply);
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        addSitesOfRead(read);
    }
    finishSites();
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        addStatisticsOfRead(read);
    }
    finishStatistics();
}

void U2AssemblyInsertionsMap::startBuild(const U2Region& regionToBuild, const U2AssemblyInsertionsFilter& filterToApply) {
    clear();
    region = regionToBuild;
    SAFE_POINT(filterToApply.isValid(), "Invalid insertions filter, every insertion is shown", );
    filter = filterToApply;
}

void U2AssemblyInsertionsMap::addSitesOfRead(const U2AssemblyRead& read) {
    CHECK(!region.isEmpty(), );

    U2Region visibleBases = getVisibleBases(read, region);
    CHECK(!visibleBases.isEmpty(), );

    // A read with no CIGAR is shown as a plain match, so it has nothing to contribute here.
    CHECK(!read->cigar.isEmpty(), );

    bool countInsertions = filter.isEnabled();
    forEachInsertion(read, visibleBases, [this, countInsertions](qint64 refPos, const QByteArray& insertion) {
        // The sites are only provisional here, as wide as the longest insertion: whether an
        // insertion passes the filter depends on the coverage, which is counted in the second pass.
        int width = qMin(insertion.size(), MAX_SITE_WIDTH);
        widthByRefPos[refPos] = qMax(widthByRefPos.value(refPos), width);
        if (countInsertions) {
            readsByInsertion[refPos][insertion.toUpper()]++;
        }
    });
}

void U2AssemblyInsertionsMap::finishSites() {
    sites.reserve(widthByRefPos.size());
    letterFrequencies.resize(widthByRefPos.size());
    for (auto it = widthByRefPos.constBegin(); it != widthByRefPos.constEnd(); ++it) {
        U2AssemblyInsertionSite site(it.key(), it.value());
        site.letterCounts.fill(0, site.width);
        letterFrequencies[sites.size()].resize(site.width);
        sites << site;
    }
    widthByRefPos.clear();
    indexSites();
}

void U2AssemblyInsertionsMap::indexSites() {
    totalWidth = 0;
    cumulativeWidths.clear();
    cumulativeWidths.reserve(sites.size());
    siteIndexByRefPos.clear();
    siteIndexByRefPos.reserve(sites.size());
    for (int i = 0; i < sites.size(); i++) {
        siteIndexByRefPos.insert(sites.at(i).refPos, i);
        totalWidth += sites.at(i).width;
        cumulativeWidths << totalWidth;
    }
}

void U2AssemblyInsertionsMap::applyFilter() {
    CHECK(filter.isEnabled(), );

    QList<U2AssemblyInsertionSite> passedSites;
    QVector<QVector<QMap<char, qint64>>> passedFrequencies;
    for (int siteIndex = 0; siteIndex < sites.size(); siteIndex++) {
        U2AssemblyInsertionSite site = sites.at(siteIndex);
        // Every insertion is judged on its own: the reads of 'C' do not support 'CC'.
        int passedWidth = 0;
        const QHash<QByteArray, qint64>& insertions = readsByInsertion.value(site.refPos);
        for (auto it = insertions.constBegin(); it != insertions.constEnd(); ++it) {
            if (filter.passes(it.value(), site.coverage)) {
                passedWidth = qMax(passedWidth, qMin<int>(it.key().size(), site.width));
            }
        }
        CHECK_CONTINUE(passedWidth > 0);

        // The letters of the columns that are left were counted for every read, the ones of the
        // insertions that did not pass included: they are shown in those columns all the same.
        site.width = passedWidth;
        site.letterCounts.resize(passedWidth);
        passedSites << site;
        passedFrequencies << letterFrequencies.at(siteIndex).mid(0, passedWidth);
    }
    sites = passedSites;
    letterFrequencies = passedFrequencies;
    readsByInsertion.clear();
    indexSites();
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

    forEachInsertion(read, visibleBases, [this](qint64 refPos, const QByteArray& insertion) {
        int siteIndex = siteIndexByRefPos.value(refPos, -1);
        CHECK(siteIndex >= 0, );

        U2AssemblyInsertionSite& site = sites[siteIndex];
        // The letters a truncated site has no column for are not shown, so they do not vote either.
        int countedLength = qMin<int>(insertion.size(), site.width);
        for (int offset = 0; offset < countedLength; offset++) {
            site.letterCounts[offset]++;
            // The same letter of another case is the same vote, as it is the same insertion for the filter.
            letterFrequencies[siteIndex][offset][char(std::toupper((unsigned char)insertion.at(offset)))]++;
        }
    });
}

void U2AssemblyInsertionsMap::finishStatistics() {
    // The coverage the filter judges by is only known now, after the second pass.
    applyFilter();
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

QVector<U2Region> U2AssemblyInsertionsMap::getRegionsToAdd(const U2Region& newRegion) const {
    QVector<U2Region> result;
    CHECK(!newRegion.isEmpty(), result);
    U2Region kept = region.intersect(newRegion);
    if (kept.isEmpty()) {
        result << newRegion;
        return result;
    }
    if (newRegion.startPos < kept.startPos) {
        result << U2Region(newRegion.startPos, kept.startPos - newRegion.startPos);
    }
    if (kept.endPos() < newRegion.endPos()) {
        result << U2Region(kept.endPos(), newRegion.endPos() - kept.endPos());
    }
    return result;
}

void U2AssemblyInsertionsMap::moveTo(const U2Region& newRegion, const QList<U2AssemblyInsertionsMap>& addedParts) {
    const QVector<U2Region> regionsToAdd = getRegionsToAdd(newRegion);
    SAFE_POINT(addedParts.size() == regionsToAdd.size(), "Insertions map parts do not match the region", );
    for (int i = 0; i < addedParts.size(); i++) {
        SAFE_POINT(addedParts.at(i).getRegion() == regionsToAdd.at(i) && addedParts.at(i).getFilter() == filter,
                   "Insertions map part is built for another region or filter", );
    }

    QList<U2AssemblyInsertionSite> movedSites;
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        if (newRegion.contains(site.refPos)) {
            movedSites << site;
        }
    }
    for (const U2AssemblyInsertionsMap& part : qAsConst(addedParts)) {
        movedSites << part.getSites();
    }
    std::sort(movedSites.begin(), movedSites.end(), [](const U2AssemblyInsertionSite& a, const U2AssemblyInsertionSite& b) {
        return a.refPos < b.refPos;
    });
    region = newRegion;
    sites = movedSites;
    indexSites();
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
