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

#ifndef _U2_MYSQL_ASSEMBLY_DBI_H_
#define _U2_MYSQL_ASSEMBLY_DBI_H_

#include <U2Core/U2SqlHelpers.h>

#include "MysqlDbi.h"
#include "util/MysqlHelpers.h"

namespace U2 {

class U2SqlQuery;
class MysqlAssemblyAdapter;

class MysqlAssemblyDbi : public U2AssemblyDbi, public MysqlChildDbiCommon {

public:
    MysqlAssemblyDbi(MysqlDbi* dbi);
    ~MysqlAssemblyDbi();

    virtual void initSqlSchema(U2OpStatus& os);
    virtual void shutdown(U2OpStatus& os);

    /** Reads assembly objects by id */
    virtual U2Assembly getAssemblyObject(const U2DataId& assemblyId, U2OpStatus& os);

    /**
        Return number of reads in assembly that intersect given region
        'Intersect' here means that region(leftmost pos, rightmost pos) intersects with 'r'
    */
    virtual qint64 countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os);

    /**
        Return reads that intersect given region
        Note: iterator instance must be deallocated by caller method
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint = false);

    /**
        Return reads with packed row value >= min, <= max that intersect given region
        Note: iterator instance must be deallocated by caller method
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os);

    /**
        Return reads with a specified name. Used to find paired reads that must have equal names
        Note: iterator instance must be deallocated by caller method
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os);

    /**
        Return max packed row at the given coordinate
        'Intersect' here means that region(leftmost pos, rightmost pos) intersects with 'r'
    */
    virtual qint64 getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os);

    /** Count 'length of assembly' - position of the rightmost base of all reads */
    virtual qint64 getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os);

    /**
        Creates new empty assembly object. Reads iterator can be NULL
        Requires: U2DbiFeature_WriteAssembly feature support
        WARNING: this function will commit your transaction right before reads will be inserted.
    */
    virtual void createAssemblyObject(U2Assembly& assembly, const QString& folder,  U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os);

    /**
        Corrects the object type in the 'Object' table or removes it if @os has an error or a cancel flag.
        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void finalizeAssemblyObject(U2Assembly &assembly, U2OpStatus &os);

    /**
        Removes all assembly data and tables.
        Does not remove entry from the 'Object' table.
        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void removeAssemblyData(const U2DataId &assemblyId, U2OpStatus& os);

    /**
        Updates assembly object fields
        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void updateAssemblyObject(U2Assembly&, U2OpStatus& os);

    /**
        Removes sequences from assembly
        Automatically removes affected sequences that are not anymore accessible from folders
    */
    virtual void removeReads(const U2DataId& assemblyId, const QList<U2DataId>& rowIds, U2OpStatus& os);

    /**
        Adds sequences to assembly
        Reads got their ids assigned.
    */
    virtual void addReads(const U2DataId& assemblyId, U2DbiIterator<U2AssemblyRead>* it, U2OpStatus& os);

    /**  Packs assembly rows: assigns packedViewRow value for every read in assembly */
    virtual void pack(const U2DataId& assemblyId, U2AssemblyPackStat& stat, U2OpStatus& os);

    /**
        Calculates coverage information for the given region. Saves result to 'coverage' vector.
        Note: Coverage window size depends on 'coverage' vector size passed to the method call.
    */
    virtual void calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os);

private:
    virtual void addReads(MysqlAssemblyAdapter* a, U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os);

    void removeTables(const U2DataId &assemblyId, U2OpStatus& os);
    void removeAssemblyEntry(const U2DataId &assemblyId, U2OpStatus& os);
    void correctAssemblyType(U2Assembly &assembly, U2OpStatus &os);

    /** Return assembly storage adapter for the given assembly */
    MysqlAssemblyAdapter* getAdapter(const U2DataId& assemblyId, U2OpStatus& os);

    /** Adapters by database assembly id */
    QHash<qint64, MysqlAssemblyAdapter*> adaptersById;
};

}   // namespace U2

#endif  // _U2_MYSQL_ASSEMBLY_DBI_H_
