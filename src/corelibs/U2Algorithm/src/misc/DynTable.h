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

#ifndef _U2_DYN_TABLE_H_
#define _U2_DYN_TABLE_H_

#include "RollingMatrix.h"

namespace U2 {

struct U2ALGORITHM_EXPORT MatchScores
{
    int match;
    int mismatch;
    int ins;
    int del;
};

class U2ALGORITHM_EXPORT DynTable : public RollingMatrix {
public:
    enum FillStrategy
    {
        Strategy_Max,
        Strategy_Min
    };
    DynTable(int n, int m, bool _allowInsDel)
        : RollingMatrix(n, m), allowInsDel(_allowInsDel)
    {
        init();
        scores.match = DEFAULT_SCORE_MATCH;
        scores.ins = DEFAULT_SCORE_INS;
        scores.del = DEFAULT_SCORE_DEL;
        scores.mismatch = DEFAULT_SCORE_MISMATCH;
        strategy = Strategy_Min;
    }
    DynTable(): RollingMatrix(0,0), allowInsDel(false)
    {
    }
    DynTable( int n, int m, bool _allowInsDel, MatchScores _scores, FillStrategy _strategy = Strategy_Min )
        : RollingMatrix(n, m), allowInsDel(_allowInsDel), scores(_scores), strategy(_strategy)
    {
        init();
    }

    int getLast() const {
        return get_value(n-1, m-1);
    }

    int getLastLen() const {
        return getLen(n-1, m-1);
    }

    void match(int y, bool ok) {
        match( n-1, y, ok );
    }

    virtual int get(int x, int y) const {
        if (y<0) {return 0;}
        if (x<0) {return y+1;}
        return RollingMatrix::get(x, y);
    }

    static quint64 estimateTableSizeInBytes(const int n, const int m)
    {
        return RollingMatrix::getMatrixSizeInBytes(n, m);
    }

protected:
    void match( int x, int y, bool ok )
    {
        int d = get_value( x-1, y-1 );
        int res = d + (ok ? scores.match : scores.mismatch);
        if( allowInsDel ) {
            int u = get_value(x, y-1);
            int l = get_value(x-1, y);
            int insdelRes = 0;
            switch( strategy )
            {
            case Strategy_Min:
                insdelRes = qMin( l+scores.ins, u+scores.del );
                res = qMin( insdelRes, res );
                break;
            default:
                assert( false );
            }
        }
        set_pair( x, y, res, ok );
    }

    int getLen(int x, int y) const {
        if (y == -1) {
            return 0;
        }
        assert(x!=-1);

        if (!allowInsDel) {
            return 1 + getLen(x-1, y-1);
        }
        int v = get_value(x, y);
        bool match = get_flag( x, y );
        int d = get_value(x-1, y-1);
        int l = get_value(x-1, y);
        int u = get_value(x, y-1);

        if( match && v == d + scores.match ) {
            return 1 + getLen( x-1, y-1 );
        }
        if ( v == u + scores.del ) { //prefer deletion in X sequence to minimize result len
            return getLen(x, y-1);
        }
        if ( !match && v == d + scores.mismatch ) { // prefer mismatch instead of insertion into X sequence
            return 1 + getLen(x-1, y-1);
        }
        assert( v == l + scores.ins ); Q_UNUSED(l);
        return 1 + getLen(x-1, y); // this is insertion into X sequence
    }
private:
    void init()
    {
        for (int i=0; i<n; i++) {
            for (int j=0; j<m; j++) {
                set_pair(i, j, j+1, false);
            }
        }
    }
    void set_value( int x, int y, int val )
    {
        assert( !(val & MASK_FLAG_MISMATCH) );
        int oldval = get( x, y );
        set( x, y, (oldval & MASK_FLAG_MISMATCH) | val );
    }
    void set_mm_flag( int x, int y, bool flag )
    {
        int oldval = get( x, y );
        set( x, y, (oldval & MASK_VALUE)|(flag ? Flag_Match : Flag_Mismatch) );
    }
    void set_pair( int x, int y, int val, bool flag )
    {
        assert( !(val & MASK_FLAG_MISMATCH) );
        set( x, y, val | (flag ? Flag_Match : Flag_Mismatch) );
    }
    int get_value( int x, int y ) const
    {
        return get( x, y ) & MASK_VALUE;
    }
    bool get_flag( int x, int y ) const
    {
        return get( x, y ) & MASK_FLAG_MISMATCH;
    }
protected:
    bool allowInsDel;
private:
    const static int DEFAULT_SCORE_MATCH = 0;
    const static int DEFAULT_SCORE_MISMATCH = 1;
    const static int DEFAULT_SCORE_DEL = 1;
    const static int DEFAULT_SCORE_INS = 1;

    const static int MASK_VALUE = 0x7FFFFFFF;
    const static int MASK_FLAG_MISMATCH = 0x80000000;
    enum MismatchFlag
    {
        Flag_Mismatch = 0x00000000,
        Flag_Match = 0x80000000
    };

    MatchScores scores;
    FillStrategy strategy;
};

} //namespace

#endif
