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

#pragma once

#include <QHash>
#include <QMap>

#include <U2Core/U2Assembly.h>
#include <U2Core/U2Region.h>

namespace U2 {

/**
 * Decides which read insertions are worth a column of their own. An alignment easily has lots of
 * insertions met in a single read or two: sequencing errors mostly, at a coverage of hundreds of
 * reads. Giving each of them a column litters the view with columns of nothing but gaps.
 *
 * Every distinct insertion of a position is counted separately: 'C' and 'CC' in front of the same
 * base are different insertions, so the reads of one do not support the other. An insertion is
 * shown when it is found in at least 'minReads' reads AND in at least 'minPercent' percent of the
 * reads covering the position. The filter only decides which columns are there: once a column is
 * reserved, every read shows the letters it has in it, whichever insertion they belong to.
 */
struct U2CORE_EXPORT U2AssemblyInsertionsFilter {
    U2AssemblyInsertionsFilter(double minPercent = 0, int minReads = 0)
        : minPercent(minPercent), minReads(minReads) {
    }

    /** The thresholds the Assembly Browser starts with. */
    static U2AssemblyInsertionsFilter getDefault();

    static const double DEFAULT_MIN_PERCENT;
    static const int DEFAULT_MIN_READS;

    /** True when the values are in range: a percent in [0, 100], a non-negative number of reads. */
    bool isValid() const;

    /** False when every insertion passes anyway, so there is nothing worth counting. */
    bool isEnabled() const;

    /** True if an insertion found in 'reads' of the 'coverage' reads covering its position passes. */
    bool passes(qint64 reads, qint64 coverage) const;

    bool operator==(const U2AssemblyInsertionsFilter& other) const {
        return minPercent == other.minPercent && minReads == other.minReads;
    }
    bool operator!=(const U2AssemblyInsertionsFilter& other) const {
        return !(*this == other);
    }

    /** Minimum share of the reads covering the position, in percent. */
    double minPercent;

    /** Minimum number of reads. */
    int minReads;
};

/** A reference position some of the reads have an insertion in front of. */
struct U2CORE_EXPORT U2AssemblyInsertionSite {
    U2AssemblyInsertionSite(qint64 refPos = 0, int width = 0)
        : refPos(refPos), width(width) {
    }

    /** Extra columns are rendered immediately BEFORE this reference position. */
    qint64 refPos;

    /**
     * Number of extra columns: the longest insertion of the reads the map was built from that
     * passes the filter of the map.
     */
    int width;

    /** Number of reads covering refPos: every one of them votes in the consensus of the columns. */
    qint64 coverage = 0;

    /** Number of reads that have a letter in the extra column, one entry per column. */
    QVector<qint64> letterCounts;

    /**
     * The most frequent character of every extra column, GAP_CHAR where the reads that have
     * nothing to put into the column outnumber every letter of it. Empty until the statistics
     * of the map are collected.
     */
    QByteArray consensus;
};

/**
 * An assembly is rendered with one column per reference position, so read insertions (CIGAR 'I')
 * have no column of their own and stay invisible. This map assigns extra columns to them, the same
 * way 'samtools tview' and the Sanger Reads Editor put a gap into the reference and into every read
 * that has no insertion there. Reference numbering is not affected: the extra columns are not
 * reference positions and are not counted as such.
 *
 * Apart from the layout the map keeps the statistics of the extra columns: how many reads have a
 * letter in a column and which character wins there, the gaps of the reads with no insertion
 * included. That is what the consensus and the coverage of an insertion column are made of.
 *
 * The insertions that do not pass the filter of the map get no column. A read can therefore have
 * more letters in front of a position than there are columns there: getInsertionWidthBefore() is
 * what tells the caller so.
 *
 * The map describes a single region and is rebuilt, or moved with moveTo(), whenever the region
 * changes, so nothing has to be stored in the database.
 */
class U2CORE_EXPORT U2AssemblyInsertionsMap {
public:
    /** A longer insertion is truncated: a single abnormal read must not squeeze the whole view out. */
    static const int MAX_SITE_WIDTH;

    /** Consensus of a column no read has enough letters for. */
    static const char GAP_CHAR;

    /**
     * Collects the insertions the given reads have inside the region, drops the previous content.
     * Only the insertions that pass the filter get columns, the default one lets every insertion pass.
     */
    void build(const QList<U2AssemblyRead>& reads, const U2Region& region, const U2AssemblyInsertionsFilter& filter = U2AssemblyInsertionsFilter());

    /**
     * The same as build(), for the callers that cannot hold every read of the region in memory:
     * startBuild(), addSitesOfRead() for every read, finishSites(), addStatisticsOfRead() for
     * every read, finishStatistics(). The reads have to be walked twice because the statistics of
     * a column can only be counted once it is known which columns are there. The sites are only
     * final after finishStatistics(): the filter needs the coverage of a position to judge it.
     */
    void startBuild(const U2Region& region, const U2AssemblyInsertionsFilter& filter = U2AssemblyInsertionsFilter());
    void addSitesOfRead(const U2AssemblyRead& read);
    void finishSites();
    void addStatisticsOfRead(const U2AssemblyRead& read);
    void finishStatistics();

    /**
     * Parts of 'newRegion' the map has nothing for: 'newRegion' without getRegion(), left to
     * right. That is 0, 1 or 2 regions, or the whole 'newRegion' when the two do not overlap.
     */
    QVector<U2Region> getRegionsToAdd(const U2Region& newRegion) const;

    /**
     * Moves the finished map to 'newRegion' without walking again the reads of the positions it
     * keeps: whatever the map has for a position is counted from the reads covering that very
     * position, so it does not depend on the rest of the region. The new positions come from
     * 'addedParts': finished maps built with the same filter for getRegionsToAdd(newRegion), in
     * the same order. A scroll by a few bases thus only costs the reads of those few bases.
     */
    void moveTo(const U2Region& newRegion, const QList<U2AssemblyInsertionsMap>& addedParts);

    void clear();

    bool isEmpty() const {
        return sites.isEmpty();
    }

    const QList<U2AssemblyInsertionSite>& getSites() const {
        return sites;
    }

    const U2Region& getRegion() const {
        return region;
    }

    const U2AssemblyInsertionsFilter& getFilter() const {
        return filter;
    }

    /** The insertion site of the reference position, nullptr when there is no insertion there. */
    const U2AssemblyInsertionSite* getSiteBefore(qint64 refPos) const;

    /** Number of extra columns rendered immediately before the reference position. */
    int getInsertionWidthBefore(qint64 refPos) const;

    /**
     * Consensus of the extra columns rendered immediately before the reference position: one
     * character per column, GAP_CHAR where the gaps of the reads without an insertion win.
     */
    QByteArray getInsertionConsensus(qint64 refPos) const;

    /**
     * Number of reads that have a letter in the extra column, 0 based offset inside the site.
     * This is the coverage of the column: the reads with no insertion here show a gap, not a base.
     */
    qint64 getInsertionCoverage(qint64 refPos, int offset) const;

    /** Column the reference position is rendered in, 0 based, relative to the region start. */
    qint64 getColumnOfRefPos(qint64 refPos) const;

    /** Number of columns the whole region takes, including the extra ones. */
    qint64 getColumnCount() const;

    /**
     * Reference position rendered in the column, 0 based, relative to the region start.
     * An extra column belongs to the reference position it precedes, because every read that
     * has letters there covers that position as well.
     */
    qint64 getRefPosOfColumn(qint64 column) const;

    /**
     * Offset of the column inside the insertion site it belongs to, -1 for a column of a real
     * reference position. Tells an extra column from a reference one, e.g. to number it after the
     * position on its LEFT: an extra column is not the position it is rendered in front of.
     */
    int getInsertionOffsetOfColumn(qint64 column) const;

private:
    /**
     * Drops the sites none of the insertions of which passes the filter and narrows the rest to
     * the longest insertion that does, then rebuilds the lookup structures from what is left.
     */
    void applyFilter();

    /** Fills the lookup structures from 'sites'. */
    void indexSites();

    U2Region region;

    U2AssemblyInsertionsFilter filter;

    /** Sorted by refPos, every site is inside the region and has a non-zero width. */
    QList<U2AssemblyInsertionSite> sites;

    qint64 totalWidth = 0;

    /** Total width of the sites up to and including the one with the same index. */
    QVector<qint64> cumulativeWidths;

    /** Widths collected by addSitesOfRead(), turned into the sites by finishSites(). */
    QMap<qint64, int> widthByRefPos;

    /** Index in 'sites' of the site of the reference position, filled by finishSites(). */
    QHash<qint64, int> siteIndexByRefPos;

    /** Letters counted by addStatisticsOfRead(): [site index][column offset][character]. */
    QVector<QVector<QMap<char, qint64>>> letterFrequencies;

    /**
     * Number of reads of every distinct insertion of a position: [refPos][upper-cased letters].
     * Collected by addSitesOfRead() only when the filter is enabled, the letters are the whole
     * insertion, not truncated to MAX_SITE_WIDTH: two long insertions only sharing the beginning
     * are still different insertions.
     */
    QHash<qint64, QHash<QByteArray, qint64>> readsByInsertion;
};

}  // namespace U2
