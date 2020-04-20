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

#ifndef _U2_SQLITE_ASSEMBLY_PACK_ALGORITHM_DBI_H_
#define _U2_SQLITE_ASSEMBLY_PACK_ALGORITHM_DBI_H_

#include <U2Core/U2SqlHelpers.h>

namespace U2 {

class U2AssemblyPackStat;
class U2Region;

class PackAlgorithmData {
public:
    PackAlgorithmData() : leftmostPos(0), effectiveLen(0){}

    U2DataId readId;
    qint64   leftmostPos;
    qint64   effectiveLen;
};

class PackAlgorithmAdapter {
public:
    virtual U2DbiIterator<PackAlgorithmData>* selectAllReads(U2OpStatus& os) = 0;
    virtual void assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) = 0;
    virtual ~PackAlgorithmAdapter(){}
};

#define PACK_TAIL_SIZE 50000

class PackAlgorithmContext {
public:
    PackAlgorithmContext();

    int     maxProw;
    qint64  nReads;

    // used to assign prow for reads when TAIL_SIZE is not enough
    qint64          peakEnd;
    qint64          peakRow;
    QVector<qint64> tails;
};

class AssemblyPackAlgorithm {
public:
    static void pack(PackAlgorithmAdapter& adapter, U2AssemblyPackStat& stat, U2OpStatus& os);
    static int packRead(const U2Region& reg, PackAlgorithmContext& ctx, U2OpStatus& os);
};


} //namespace

#endif
