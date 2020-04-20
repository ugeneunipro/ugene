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

#ifndef _U2_REGION_H_
#define _U2_REGION_H_

//small hack, may be bad
#ifndef LLONG_MAX
#ifdef Q_OS_LINUX
#include <limits.h>
#endif
#endif

#include <QObject>
#include <QVector>

#include <U2Core/global.h>

namespace U2 {

/** Predefined region - acts as hint for a function that all possible range must be processed */
#define U2_REGION_MAX U2Region(0, LLONG_MAX)

/**
    Linear 64bit region
*/
class U2CORE_EXPORT U2Region {
public:
    U2Region(): startPos(0), length(0){}
    U2Region(qint64 s, qint64 l): startPos(s), length(l){}

    /** Region start position. */
    qint64 startPos;

    /** Region length. */
    qint64 length;

    ////////////////////////// Member functions and operators ////////////////////

    /** Region end position, exclusive. */
    qint64 endPos() const {return startPos + length;}

    /** Central point of the region. */
    qint64 center() const { return startPos + length / 2 ;}

    /** Checks if this region has zero length. */
    bool isEmpty() const {return length == 0; }

    /** Checks whether the specified point falls inside this region. */
    bool contains(qint64 pos) const {return pos>=startPos && pos < endPos();}

    /** Checks whether the specified region fits inside this region or is equal. */
    bool contains(const U2Region& r) const {return r.startPos >= startPos && r.endPos() <= endPos();}

    /** Checks whether the specified region has common points with this region. */
    bool intersects(const U2Region& r) const;

    /** Returns the intersection between 2 regions, or empty value if regions do not intersect. */
    U2Region intersect(const U2Region& r) const;

    /**
        Checks whether this region has common points with any region in the specified list.
        Returns the index of the first region found or -1 if no matches found
    */
    int findIntersectedRegion(const QVector<U2Region>& rs) const;

    /**
        Checks whether this region has common points with any region in the specified list.
        Returns the index of the first region found or -1 if no matches found
    */
    bool intersects(const QVector<U2Region>& rs) const {return findIntersectedRegion(rs) != -1;}

    /**
        Checks whether this region is located inside of any region in the specified list.
        Returns the index of the first region found or -1 if no matches found
    */
    int findOverlappingRegion(const QVector<U2Region>& regions) const;

    /** Checks if the specified region is equal to this region. */
    bool operator== ( const U2Region & r ) const { return r.startPos == startPos && r.length == length; }

    /** Checks whether the specified region is not equal to this region. */
    bool operator!= ( const U2Region & r ) const { return r.startPos != startPos || r.length != length; }

    /** Compares 2 regions by start position.
    Returns true if this region starts strictly earlier than the specified one. */
    bool operator<(const U2Region &r) const {return startPos < r.startPos;}

/** Compares 2 regions by start position.
    Returns true if this region starts strictly later than the specified one. */
    bool operator>(const U2Region &r) const {return startPos > r.startPos;}

    /**
        String formats for U2Region::toString:
            FormatBrackets  "[100, 200)"
            FormatDash      "100 - 200"
            FormatPlusMinus "150 &plusmn; 50" - for html only
            FormatDots      "100..199"
    */
    enum Format {
        FormatBrackets,
        FormatDash,
        FormatPlusMinus,
        FormatDots
    };

    /** Converts region to its string represenation using given format. */
    QString toString(Format format = FormatBrackets) const;

    ///////////////////////// Class functions /////////////////////////////////

    /** Returns least common region which contains both of the 2 specified regions. */
    static U2Region containingRegion(const U2Region& r1, const U2Region& r2);

    /** Returns least common region which contains all of the specified regions. */
    static U2Region containingRegion(const QVector<U2Region>& regions);

    /** Returns least coverage of the regions. Sequence is considered to be circular. */
    static QVector<U2Region> circularContainingRegion(QVector<U2Region>& regions, int seqLen);

    /** Normalizes the specified list by joining overlapping regions.
    This function sorts regions by starting position then
    iterates through them and replaces all groups of intersecting regions by containing regions.
    */
    static QVector<U2Region> join(QVector<U2Region>& regions);

    /** Fixes start & len for all regions to ensure that the result region
        has startPos >= minPos & endPos <= maxPos
     */
    static void bound(qint64 minPos, qint64 maxPos, QVector<U2Region>& regions);

    /** mirrors regions by mirror pos: startPos = mirrorPos - startPos */
    static void mirror(qint64 mirrorPos, QVector<U2Region>& regions);

    /** divides regions by  div : startPos = startPos / div */
    static void divide(qint64 div, QVector<U2Region>& regions);

    /** multiplies regions by  mult : startPos = startPos * mult */
    static void multiply(qint64 mult, QVector<U2Region>& regions);

    /** Reverses order of regions in the list */
    static void reverse(QVector<U2Region>& regions);

    /** shifts regions by offset pos: startPos = startPos + offset */
    static void shift(qint64 offset, QVector<U2Region>& regions);

    /** Splits region into the set of regions of the given block size. */
    static QList<U2Region> split(const U2Region& region, qint64 blockSize);

    /** Returns sum of region lengths. */
    static qint64 sumLength(const QVector<U2Region>& regions);

private:
    static bool registerMeta;
};

//////////////////// inline implementations ///////////////////////////

inline bool U2Region::intersects(const U2Region& r) const {
    qint64 sd = startPos - r.startPos;
    return (sd >= 0) ? (sd < r.length) : (-sd < length);
}

inline U2Region U2Region::intersect(const U2Region& r) const {
    qint64 newStart = qMax(startPos, r.startPos);
    qint64 newEnd = qMin(endPos(), r.endPos());
    if (newStart > newEnd) {
        return U2Region();
    }
    return U2Region(newStart, newEnd - newStart);
}


inline U2Region U2Region::containingRegion(const U2Region& r1, const U2Region& r2) {
    qint64 newStart = qMin(r1.startPos, r2.startPos);
    qint64 newEnd = qMax(r1.endPos(), r2.endPos());
    return U2Region(newStart, newEnd - newStart);
}

inline U2Region U2Region::containingRegion(const QVector<U2Region>& regions) {
    assert(!regions.isEmpty());

    U2Region res = regions.first();
    foreach(const U2Region& r, regions) {
        res = containingRegion(res, r);
    }
    return res;
}

U2CORE_EXPORT QDataStream &operator<<(QDataStream &out, const U2Region &myObj);
U2CORE_EXPORT QDataStream &operator>>(QDataStream &in, U2Region &myObj);

}//namespace

Q_DECLARE_TYPEINFO(U2::U2Region, Q_PRIMITIVE_TYPE);
Q_DECLARE_METATYPE(U2::U2Region)
Q_DECLARE_METATYPE(QVector< U2::U2Region >)

#endif
