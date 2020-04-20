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

#ifndef _U2_LOCATION_H_
#define _U2_LOCATION_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Region.h>

#include <QSharedData>


namespace U2 {

/**
    For annotation with multiple regions this flag indicates how to process joins
*/
enum U2LocationOperator {
    /** Location elements must be joined */
    U2LocationOperator_Join = 1,
    /** No need to join location elements */
    U2LocationOperator_Order = 2,
    /** Works like 'join', but has its own indicator 'bond' */
    U2LocationOperator_Bond = 3
};

/**
    Type of location region
*/
enum U2LocationRegionType {

    /** default */
    U2LocationRegionType_Default = 0,

    /** a site between indicated adjoining bases */
    U2LocationRegionType_Site = 1,

    /** a single base chosen from within a specified range of bases (not allowed for new entries) */
    U2LocationRegionType_SingleBase = 2
};


/** Annotation location */
class U2CORE_EXPORT U2LocationData : public QSharedData {
public:

    /** Location operator */
    U2LocationOperator      op;

    /**  Strand of the location. */
    U2Strand                strand;

    /**
        Descriptor of the region: no special info, site, ...
        Note that all non-default values are affective only for
        single region locations
    */
    U2LocationRegionType    regionType;

    /** Annotated regions coordinates */
    QVector<U2Region>         regions;

    /** Constructs empty location */
    U2LocationData() { reset(); }

    bool isEmpty() const {return regions.isEmpty();}

    /** Resets location to initial empty state */
    void reset();


    bool isOrder() const {return op == U2LocationOperator_Order;}

    bool isJoin() const {return op == U2LocationOperator_Join;}

    bool isBond() const {return op == U2LocationOperator_Bond;}

    bool isMultiRegion() const {return regions.size() > 1;}

    bool isSingleRegion() const {return regions.size() == 1;}

    bool operator==(const U2LocationData& l) const;

    bool operator!=(const U2LocationData& l) const {return !(*this == l);}
};

inline bool U2LocationData::operator==(const U2LocationData& l) const {
    bool res = regions == l.regions && strand == l.strand && op == l.op && regionType == l.regionType;
    return res;
}

/**
    U2Location is a shared pointer to U2LocationData
    The difference in behavior with normal shared pointer is that
    it creates default U2LocationData for non-arg constructor
    and compares LocationData by value for == and != operators
*/
class U2CORE_EXPORT U2Location {
public:
    U2Location() : d(new U2LocationData()){}
    U2Location( U2LocationData *l) : d(l){}

    U2LocationData&	operator*() {return *d;}
    const U2LocationData&	operator*() const {return *d;}

    U2LocationData*	operator->() {return d;}
    const U2LocationData*	operator->() const {return d;}

    U2LocationData *data() {return d.data(); }
    const U2LocationData *data() const {return d.constData();}

    inline operator U2LocationData *() { return d; }
    inline operator const U2LocationData *() const { return d; }

    U2Location&	operator=(const U2Location& other) {d = other.d; return *this;}

private:
    QSharedDataPointer<U2LocationData> d;
};

inline bool	operator!= ( const U2Location & ptr1, const U2Location & ptr2 ) {return *ptr1  != *ptr2;}
inline bool	operator!= ( const U2Location & ptr1, const U2LocationData * ptr2 ) {return *ptr1  != *ptr2;}
inline bool	operator!= ( const U2LocationData * ptr1, const U2Location & ptr2 ) {return *ptr1  != *ptr2;}
inline bool	operator== ( const U2Location & ptr1, const U2Location & ptr2 ) {return *ptr1  == *ptr2;}
inline bool	operator== ( const U2Location & ptr1, const U2LocationData * ptr2 ) {return *ptr1  == *ptr2;}
inline bool	operator== ( const U2LocationData * ptr1, const U2Location & ptr2 ) {return *ptr1 == *ptr2;}


//////////////////////////////////////////////////////////////////////////
// functions impl

inline void U2LocationData::reset() {
    regions.clear();
    strand = U2Strand::Direct;
    op = U2LocationOperator_Join;
    regionType = U2LocationRegionType_Default;
}

} // namespace

#endif
