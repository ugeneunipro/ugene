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
};

/**
 * An assembly is rendered with one column per reference position, so read insertions (CIGAR 'I')
 * have no column of their own and stay invisible. This map assigns extra columns to them, the same
 * way 'samtools tview' and the Sanger Reads Editor put a gap into the reference and into every read
 * that has no insertion there. Reference numbering is not affected: the extra columns are not
 * reference positions and are not counted as such.
 *
 * The map describes a single region and is meant to be rebuilt whenever the region changes, so
 * nothing has to be stored in the database.
 */
class U2CORE_EXPORT U2AssemblyInsertionsMap {
public:
    /** A longer insertion is truncated: a single abnormal read must not squeeze the whole view out. */
    static const int MAX_SITE_WIDTH;

    /** Collects the insertions the given reads have inside the region, drops the previous content. */
    void build(const QList<U2AssemblyRead>& reads, const U2Region& region);

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

    /** Number of extra columns rendered immediately before the reference position. */
    int getInsertionWidthBefore(qint64 refPos) const;

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

private:
    U2Region region;

    /** Sorted by refPos, every site is inside the region and has a non-zero width. */
    QList<U2AssemblyInsertionSite> sites;

    qint64 totalWidth = 0;
};

}  // namespace U2
