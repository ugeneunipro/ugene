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

/** A reference position some of the reads have an insertion in front of. */
struct U2CORE_EXPORT U2AssemblyInsertionSite {
    U2AssemblyInsertionSite(qint64 refPos = 0, int width = 0)
        : refPos(refPos), width(width) {
    }

    /** Extra columns are rendered immediately BEFORE this reference position. */
    qint64 refPos;

    /** Number of extra columns: the longest insertion of the reads the map was built from. */
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
 * The map describes a single region and is meant to be rebuilt whenever the region changes, so
 * nothing has to be stored in the database.
 */
class U2CORE_EXPORT U2AssemblyInsertionsMap {
public:
    /** A longer insertion is truncated: a single abnormal read must not squeeze the whole view out. */
    static const int MAX_SITE_WIDTH;

    /** Consensus of a column no read has enough letters for. */
    static const char GAP_CHAR;

    /** Collects the insertions the given reads have inside the region, drops the previous content. */
    void build(const QList<U2AssemblyRead>& reads, const U2Region& region);

    /**
     * The same as build(), for the callers that cannot hold every read of the region in memory:
     * startBuild(), addSitesOfRead() for every read, finishSites(), addStatisticsOfRead() for
     * every read, finishStatistics(). The reads have to be walked twice because the statistics of
     * a column can only be counted once it is known which columns are there.
     */
    void startBuild(const U2Region& region);
    void addSitesOfRead(const U2AssemblyRead& read);
    void finishSites();
    void addStatisticsOfRead(const U2AssemblyRead& read);
    void finishStatistics();

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
    U2Region region;

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
};

}  // namespace U2
