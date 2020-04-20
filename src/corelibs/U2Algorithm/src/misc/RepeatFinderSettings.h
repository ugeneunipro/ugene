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

#ifndef _U2_REPEAT_FINDER_SETTINGS_H_
#define _U2_REPEAT_FINDER_SETTINGS_H_

#include <U2Core/Task.h>
#include <QString>

namespace U2 {

class DNAAlphabet;

enum RFAlgorithm {
    RFAlgorithm_Auto,
    RFAlgorithm_Diagonal,
    RFAlgorithm_Suffix
};

enum RepeatsFilterAlgorithm
{
    DisjointRepeats,
    NoFiltering,
    UniqueRepeats
};



class RFResult
{
public:
    RFResult() : x(0), y(0), l(0), c(0){}

    RFResult(int _x, int _y, int _len, int _c = 0)
        : x(_x), y(_y), l(_len)
    {   //if not specified, repeats have no mismatches
        if(_c==0) c=l; else c = _c;
    }

    RFResult(int _x, int _y, int _len, int _c, QString _fragment)
        : fragment(_fragment), x(_x), y(_y), l(_len), c(_c)
    {
        if(_c==0) c=l; else c = _c;
    }

    bool operator==(const RFResult& r) const {return x == r.x && y == r.y && l == r.l;}
    bool operator!=(const RFResult& r) const {return !(*this == r);}
    bool operator <(const RFResult& r) const {return (x != r.x) ? x < r.x : (y != r.y) ? y < r.y: (l < r.l);}

    QString fragment;
    int x;
    int y;
    int l;
    int c; //matches
};

class RFResultsListener {
public:
    virtual void onResult(const RFResult& r) = 0;
    virtual void onResults(const QVector<RFResult>& v) = 0;
};

struct RepeatFinderSettings {
    RepeatFinderSettings()
    :   l(NULL), seqX(NULL), sizeX(0), inverted(false),
        seqY(NULL), sizeY(0),
        al(NULL), w(0), mismatches(0),
        alg(RFAlgorithm_Auto), nThreads(0)
    {}

    RepeatFinderSettings(
        RFResultsListener* _l, const char *_seqX, int _sizeX, bool _inverted,
        const char *_seqY, int _sizeY, const DNAAlphabet *_al, int _w, int _mismatches,
        RFAlgorithm _alg, int _nThreads = MAX_PARALLEL_SUBTASKS_AUTO
    )
    :   l(_l), seqX(_seqX), sizeX(_sizeX), inverted(_inverted),
        seqY(_seqY), sizeY(_sizeY),
        al(_al), w(_w), mismatches(_mismatches),
        alg(_alg), nThreads(_nThreads)
    {}

    RFResultsListener*  l;
    const char *        seqX;
    int                 sizeX;
    bool                inverted;
    const char *        seqY;
    int                 sizeY;
    const DNAAlphabet *       al;
    int                 w;
    int                 mismatches;
    RFAlgorithm         alg;
    int                 nThreads;

    bool operator==(const RepeatFinderSettings& op) const {
        return  l == op.l &&
                seqX == op.seqX &&
                sizeX == op.sizeX &&
                inverted == op.inverted &&
                seqY == op.seqY &&
                sizeY == op.sizeY &&
                al == op.al &&
                w == op.w &&
                mismatches == op.mismatches &&
                alg == op.alg &&
                nThreads == op.nThreads;
    }
};

} // namespace

#endif
