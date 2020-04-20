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

#ifndef _U2_SQLITE_ASSEMBLY_DBI_H_
#define _U2_SQLITE_ASSEMBLY_DBI_H_

#include <U2Core/U2SqlHelpers.h>

#include "SQLiteDbi.h"
#include "util/AssemblyAdapter.h"
#include "util/AssemblyPackAlgorithm.h"

namespace U2 {

class SQLiteWriteQuery;
class AssemblyAdapter;

class SQLiteAssemblyDbi : public U2AssemblyDbi, public SQLiteChildDBICommon {

public:
    SQLiteAssemblyDbi(SQLiteDbi* dbi);
    ~SQLiteAssemblyDbi();

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
    */
    virtual void createAssemblyObject(U2Assembly& assembly, const QString& folder,  U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os);

    /**
        Creates indexes for reads tables.
        This method mustn't be called inside transaction.
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
        Calculates coverage information for the given region. Saves result to 'c.coverage' vector.
        Note: Coverage window size depends on 'c.coverage' vector size passed to the method call.
    */
    virtual void calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os);

    virtual void initSqlSchema(U2OpStatus& os);
    virtual void shutdown(U2OpStatus& os);

    static QString getCreateAssemblyTableQuery(const QString &tableAlias = "Assembly");

private:
    virtual void addReads(AssemblyAdapter* a, U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os);

    void removeTables(const U2DataId &assemblyId, U2OpStatus& os);
    void removeAssemblyEntry(const U2DataId &assemblyId, U2OpStatus& os);

    /** Return assembly storage adapter for the given assembly */
    AssemblyAdapter* getAdapter(const U2DataId& assemblyId, U2OpStatus& os);

    /** Adapters by database assembly id */
    QHash<qint64, AssemblyAdapter*> adaptersById;
};


class SQLiteAssemblyAdapter : public AssemblyAdapter {
public:
    SQLiteAssemblyAdapter(const U2DataId& assemblyId, const AssemblyCompressor* compressor, DbRef* ref) :
        AssemblyAdapter(assemblyId, compressor), db(ref) {}

protected:
    DbRef*                      db;
};


/** Compression method for assembly data */
enum SQLiteAssemblyDataMethod {
    /** Merges Name, Sequence, Cigar and Quality values into single byte array separated by '\n' character. Merge prefix is '0'*/
    SQLiteAssemblyDataMethod_NSCQ = 1
};

class SQLiteAssemblyUtils {
public:
    static QByteArray packData(SQLiteAssemblyDataMethod method, const U2AssemblyRead &read, U2OpStatus& os);

    static void unpackData(const QByteArray& packed, U2AssemblyRead &read, U2OpStatus& os);

    static void calculateCoverage(SQLiteReadQuery& q, const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os);

    static void addToCoverage(U2AssemblyCoverageImportInfo& cii, const U2AssemblyRead& read);
};

class SQLiteAssemblyNameFilter : public SQLiteResultSetFilter<U2AssemblyRead> {
public:
    SQLiteAssemblyNameFilter(const QByteArray& expectedName) : name (expectedName) {}
    virtual bool filter(const U2AssemblyRead& r)  {return name == r->name;}
protected:
    QByteArray name;

};

class SimpleAssemblyReadLoader: public SQLiteResultSetLoader<U2AssemblyRead> {
public:
    U2AssemblyRead load(SQLiteQuery* q);
};

class SimpleAssemblyReadPackedDataLoader : public SQLiteResultSetLoader<PackAlgorithmData> {
public:
    virtual PackAlgorithmData load(SQLiteQuery* q);
};



} //namespace

#endif
