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

#ifndef _U2_PMATRIX_SERIALIZE_UTILS_H_
#define _U2_PMATRIX_SERIALIZE_UTILS_H_

#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

template<class Serializer, class Matrix>
class PMatrixSerializeUtils {
public:
    static U2EntityRef      commit(const Matrix &matrix, const QString &objectName,
                                const U2DbiRef &dbiRef, const QString &dstFolder, U2RawData &object, U2OpStatus &os);
    static void             retrieve(const U2EntityRef &entityRef, Matrix &matrix, U2OpStatus &os);
};

//////////////////////////////////////////////////////////////////////////
/// Implementation ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class Serializer, class Matrix>
U2EntityRef PMatrixSerializeUtils<Serializer, Matrix>::commit(const Matrix &matrix,
    const QString &objectName, const U2DbiRef &dbiRef, const QString &dstFolder, U2RawData &object, U2OpStatus &os)
{
    object.visualName = objectName;
    object.serializer = Serializer::ID;

    RawDataUdrSchema::createObject(dbiRef, dstFolder, object, os);
    CHECK_OP(os, U2EntityRef());

    const U2EntityRef entRef(dbiRef, object.id);
    const QByteArray data = Serializer::serialize(matrix);
    RawDataUdrSchema::writeContent(data, entRef, os);
    return entRef;
}

template<class Serializer, class Matrix>
void PMatrixSerializeUtils<Serializer, Matrix>::retrieve(const U2EntityRef &entityRef,
    Matrix &matrix, U2OpStatus &os)
{
    const QString serializer = RawDataUdrSchema::getObject(entityRef, os).serializer;
    CHECK_OP(os, );
    SAFE_POINT(Serializer::ID == serializer, "Unknown serializer id", );

    const QByteArray data = RawDataUdrSchema::readAllContent(entityRef, os);
    CHECK_OP(os, );
    matrix = Serializer::deserialize(data, os);
}

}

#endif // _U2_PMATRIX_SERIALIZE_UTILS_H_
