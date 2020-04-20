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

#ifndef _U2_MYSQL_ASSEMBLY_UTILS_H_
#define _U2_MYSQL_ASSEMBLY_UTILS_H_

#include <U2Core/U2Assembly.h>
#include <U2Core/U2Region.h>

namespace U2 {

class U2AssemblyCoverageImportInfo;
class U2OpStatus;
class U2SqlQuery;

/** Compression method for assembly data */
enum MysqlAssemblyDataMethod {
    /** Merges Name, Sequence, Cigar and Quality values into single byte array separated by '\n' character. Merge prefix is '0'*/
    MysqlAssemblyDataMethod_NSCQ = 1
};

class MysqlAssemblyUtils {
public:
    static QByteArray packData(MysqlAssemblyDataMethod method, const U2AssemblyRead &read, U2OpStatus& os);

    static void unpackData(const QByteArray& packed, U2AssemblyRead &read, U2OpStatus& os);

    static void calculateCoverage(U2SqlQuery& q, const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os);

    static void addToCoverage(U2AssemblyCoverageImportInfo& cii, const U2AssemblyRead& read);
};

}   // namespace U2

#endif // _U2_MYSQL_ASSEMBLY_UTILS_H_
