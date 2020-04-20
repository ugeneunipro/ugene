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

#include <U2Core/U2AssemblyUtils.h>

#include "MysqlAssemblyAdapter.h"
#include "MysqlAssemblyUtils.h"

namespace U2 {

MysqlAssemblyAdapter::MysqlAssemblyAdapter(const U2DataId& assemblyId,
                                           const AssemblyCompressor* compressor,
                                           MysqlDbRef* ref) :
    AssemblyAdapter(assemblyId, compressor),
    db(ref)
{
}

MysqlAssemblyNameFilter::MysqlAssemblyNameFilter(const QByteArray& expectedName) :
    name (expectedName)
{
}

bool MysqlAssemblyNameFilter::filter(const U2AssemblyRead& r) {
    return name == r->name;
}

U2AssemblyRead MysqlSimpleAssemblyReadLoader::load(U2SqlQuery* q) {
    U2AssemblyRead read(new U2AssemblyReadData());

    read->id = q->getDataId(0, U2Type::AssemblyRead);
    read->packedViewRow = q->getInt64(1);
    if (q->hasError()) {
        return U2AssemblyRead();
    }

    read->leftmostPos = q->getInt64(2);
    read->effectiveLen = q->getInt64(3);
    read->flags = q->getInt64(4);
    read->mappingQuality = (quint8)q->getInt32(5);
    QByteArray data = q->getBlob(6);
    if (q->hasError()) {
        return U2AssemblyRead();
    }

    MysqlAssemblyUtils::unpackData(data, read, q->getOpStatus());
    if (q->hasError()) {
        return U2AssemblyRead();
    }

#ifdef _DEBUG
    //additional check to ensure that db stores correct info
    qint64 effectiveLengthFromCigar = read->readSequence.length() + U2AssemblyUtils::getCigarExtraLength(read->cigar);
    assert(effectiveLengthFromCigar == read->effectiveLen);
#endif

    return read;
}

PackAlgorithmData MysqlSimpleAssemblyReadPackedDataLoader::load(U2SqlQuery* q) {
    PackAlgorithmData data;
    data.readId = q->getDataId(0, U2Type::AssemblyRead);
    data.leftmostPos = q->getInt64(1);
    data.effectiveLen = q->getInt64(2);
    return data;
}

}   // namespace U2
