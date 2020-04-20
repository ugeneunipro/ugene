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

#ifndef _U2_ASSEMBLY_ADAPTER_H_
#define _U2_ASSEMBLY_ADAPTER_H_

#include <U2Core/U2Assembly.h>
#include <U2Core/U2Region.h>

namespace U2 {

class U2AssemblyReadsImportInfo;
class U2OpStatus;

// reserved for future use;
class AssemblyCompressor {};

class AssemblyAdapter {
public:
    AssemblyAdapter(const U2DataId& assemblyId, const AssemblyCompressor* compressor) :
        assemblyId(assemblyId), compressor(compressor) {}
    virtual ~AssemblyAdapter() {}

    virtual void createReadsTables(U2OpStatus&) {}
    virtual void createReadsIndexes(U2OpStatus&) {}
    virtual void shutdown(U2OpStatus&) {}

    virtual qint64 countReads(const U2Region& r, U2OpStatus& os) = 0;

    virtual qint64 getMaxPackedRow(const U2Region& r, U2OpStatus& os) = 0;
    virtual qint64 getMaxEndPos(U2OpStatus& os) = 0;

    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2Region& r, U2OpStatus& os, bool sortedHint = false) = 0;
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) = 0;
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const QByteArray& name, U2OpStatus& os) = 0;

    virtual void addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) = 0;

    virtual void removeReads(const QList<U2DataId>& rowIds, U2OpStatus& os) = 0;
    virtual void dropReadsTables(U2OpStatus& os) = 0;

    virtual void pack(U2AssemblyPackStat& stat, U2OpStatus& os) = 0;

    virtual void calculateCoverage(const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) = 0;

protected:
    U2DataId                    assemblyId;
    const AssemblyCompressor*   compressor;
};

}   // namespace U2

#endif // _U2_ASSEMBLY_ADAPTER_H_
