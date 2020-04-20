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

#ifndef _U2_ASSEMBLY_DBI_H_
#define _U2_ASSEMBLY_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Assembly.h>

namespace U2 {

/** Class used to iterate huge amount of assembly reads and optimize assembly import operation to DBI */
class U2AssemblyReadsIterator {
public:
    /** returns true if there are more reads to iterate*/
    virtual bool hasNext() = 0;

    /** returns next read or error */
    virtual U2AssemblyRead next(U2OpStatus& os) = 0;
};

class U2AssemblyCoverageImportInfo {
public:
    U2AssemblyCoverageImportInfo() : computeCoverage(false), coverageBasesPerPoint(1) {}

    /** Specifies if assembly coverage is needed to be computed at import time*/
    bool computeCoverage;

    /** Place to save coverage info */
    U2AssemblyCoverageStat coverage;

    /** Shows how many real bases are in 1 coverage point */
    double coverageBasesPerPoint;
};

/** Additional reads info used during reads import into assembly */
class U2AssemblyReadsImportInfo {
public:
    U2AssemblyReadsImportInfo(U2AssemblyReadsImportInfo *parentInfo = NULL) : nReads(0), packed(false), parentInfo(parentInfo) {}
    virtual ~U2AssemblyReadsImportInfo() {}

    /** Number of reads added during import */
    qint64 nReads;

    /** Specifies if assembly was packed at import time*/
    bool packed;

    /* Place where to save pack statistics */
    U2AssemblyPackStat packStat;

    U2AssemblyCoverageImportInfo coverageInfo;

    virtual void onReadImported() {
        if (NULL != parentInfo) {
            parentInfo->onReadImported();
        }
    }

private:
    U2AssemblyReadsImportInfo *parentInfo;
};

/**
    An interface to obtain  access to assembly data
*/
class U2AssemblyDbi : public U2ChildDbi {
protected:
    U2AssemblyDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi) {}

public:
    /**
        Reads assembly object by its id.
        If there is no assembly object with the specified id returns a default constructed assembly object.
    */
    virtual U2Assembly getAssemblyObject(const U2DataId& id, U2OpStatus& os) = 0;

    /**
        Returns number of reads located near or intersecting the region.
        The region should be a valid region within alignment bounds, i.e. non-negative and less than alignment length.

        If there is no assembly object with the specified id returns -1.

        Note: 'near' here means that DBI is not forced to return precise number of reads that intersects the region
        and some deviations is allowed in order to apply performance optimizations.

        Note2: Use U2_ASSEMBLY_REGION_MAX to count all reads in assembly in effective way

    */
    virtual qint64 countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) = 0;

    /**
        Returns reads that intersect given region.
        If there is no assembly object with the specified id returns NULL.

        Note: iterator instance must be deallocated by caller method
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint = false) = 0;

    /**
        Returns reads with packed row value bounded by 'minRow' and 'maxRow' that intersect given region.
        If there is no assembly object with the specified id returns NULL.

        Note: iterator instance must be deallocated by caller method
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) = 0;

    /**
        Returns reads with a specified name. Used to find paired reads that must have equal names.
        If there is no assembly object with the specified id returns NULL.

        Note: iterator instance must be deallocated by caller method.
    */
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os) = 0;

    /**
        Returns maximum packed row value of reads that intersect 'r'.
        'Intersect' here means that region(leftmost pos, rightmost pos) intersects with 'r'
        If there is no assembly object with the specified id returns -1.
    */
    virtual qint64 getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) = 0;


    /**
        Count 'length of assembly' - position of the rightmost base of all reads.
        If there is no assembly object with the specified id returns -1.
    */
    virtual qint64 getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os) = 0;


    /**
        Creates new empty assembly object. Reads iterator can be NULL.
        If iterator is not NULL adapter can automatically try to pack reads. If pack is performed, the corresponding
        structure is filled with  pack statistics. Assembly object gets its id assigned.
        Folder 'folder' must exist in database.
        The created object must be finalized.

        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void createAssemblyObject(U2Assembly& assembly, const QString& folder,
        U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& importInfo, U2OpStatus& os) = 0;

    /**
        Does some additional actions that should be done after object creating and reads adding.
        The set of actions is provider-dependent
        In common case this method shouldn't be called inside transaction.

        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void finalizeAssemblyObject(U2Assembly &assembly, U2OpStatus &os) = 0;

    /**
        Removes all assembly data and tables.
        Does not remove entry from the 'Object' table.
        Requires: U2DbiFeature_WriteAssembly feature support
    */
    virtual void removeAssemblyData(const U2DataId &assemblyId, U2OpStatus& os) = 0;

    /**
        Updates assembly object fields.

        Requires: U2DbiFeature_WriteAssembly feature support.
    */
    virtual void updateAssemblyObject(U2Assembly& assembly, U2OpStatus& os) = 0;

    /**
        Removes reads from assembly.
        Automatically removes affected sequences that are not anymore accessible from folders.

        Requires: U2DbiFeature_WriteAssembly feature support.
    */
    virtual void removeReads(const U2DataId& assemblyId, const QList<U2DataId>& readIds, U2OpStatus& os) = 0;

    /**
        Adds sequences to assembly.
        Reads got their ids assigned.

        Requires: U2DbiFeature_WriteAssembly feature support.
    */
    virtual void addReads(const U2DataId& assemblyId, U2DbiIterator<U2AssemblyRead>* it, U2OpStatus& os) = 0;

    /**
        Packs assembly rows: assigns packedViewRow value (i.e. read's vertical position in view)
        for every read in assembly so that reads do not overlap.

        Requires: U2DbiFeature_WriteAssembly and U2DbiFeature_AssemblyReadsPacking features support
    */
    virtual void pack(const U2DataId& assemblyId, U2AssemblyPackStat& stats, U2OpStatus& os) = 0;

    /**
        Calculates coverage information for the given region and stores it in 'coverage' vector.

        U2Region 'region' passed to the method is split into N sequential windows of equal length,
        where N is 'coverage' vector size. Number of reads intersecting each window is guaranteed
        to be in range stored at corresponding index in vector 'coverage'.
    */
    virtual void calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) = 0;

};

} //namespace

#endif
