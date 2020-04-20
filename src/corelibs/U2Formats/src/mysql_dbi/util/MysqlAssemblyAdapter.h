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

#ifndef _U2_MYSQL_ASSEMBLY_ADAPTER_H_
#define _U2_MYSQL_ASSEMBLY_ADAPTER_H_

#include <U2Core/U2Assembly.h>

#include "MysqlHelpers.h"
#include "util/AssemblyAdapter.h"
#include "util/AssemblyPackAlgorithm.h"

namespace U2 {

class U2OpStatus;
class U2AssemblyReadsImportInfo;

class MysqlAssemblyAdapter : public AssemblyAdapter {
public:
    MysqlAssemblyAdapter(const U2DataId& assemblyId, const AssemblyCompressor* compressor, MysqlDbRef* ref);

protected:
    MysqlDbRef* db;
};

class MysqlAssemblyNameFilter : public MysqlRSFilter<U2AssemblyRead> {
public:
    MysqlAssemblyNameFilter(const QByteArray& expectedName);
    virtual bool filter(const U2AssemblyRead& r);

protected:
    QByteArray name;
};

class MysqlSimpleAssemblyReadLoader: public MysqlRSLoader<U2AssemblyRead> {
public:
    U2AssemblyRead load(U2SqlQuery* q);
};

class MysqlSimpleAssemblyReadPackedDataLoader : public MysqlRSLoader<PackAlgorithmData> {
public:
    virtual PackAlgorithmData load(U2SqlQuery* q);
};

}   // namespace U2

#endif // _U2_MYSQL_ASSEMBLY_ADAPTER_H_
