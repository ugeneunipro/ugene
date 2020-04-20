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

#include <U2Core/AppContext.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>

#include "DbiConnection.h"

namespace U2 {

DbiConnection::DbiConnection(const U2DbiRef& ref,  U2OpStatus& os) : dbi(NULL) {
    open(ref, os);
}

DbiConnection::DbiConnection(const U2DbiRef& ref,  bool create, U2OpStatus& os, const QHash<QString, QString> &properties) : dbi(NULL) {
    open(ref, create, os, properties);
}

DbiConnection::DbiConnection(const DbiConnection& dbiConnection) {
    copy(dbiConnection);
}

DbiConnection::~DbiConnection() {
    U2OpStatus2Log os;
    close(os);
}

void DbiConnection::open(const U2DbiRef& ref,  U2OpStatus& os)  {
    open(ref, false, os);
}

namespace {
U2DbiPool * getDbiPool(U2OpStatus &os) {
    U2DbiRegistry *dbiReg = AppContext::getDbiRegistry();
    CHECK_EXT(NULL != dbiReg, os.setError("DBI registry is not initialized"), NULL);

    U2DbiPool *pool = dbiReg->getGlobalDbiPool();
    CHECK_EXT(NULL != pool, os.setError("DBI pool is not initialized"), NULL);
    return pool;
}
}

void DbiConnection::open(const U2DbiRef& ref,  bool create, U2OpStatus& os, const QHash<QString, QString> &properties) {
    SAFE_POINT_EXT(!isOpen(), os.setError(QString("Connection is already opened! %1").arg(dbi->getDbiId())), );
    U2DbiPool *pool = getDbiPool(os);
    SAFE_POINT_OP(os, );
    dbi = pool->openDbi(ref, create, os, properties);
}

void DbiConnection::close(U2OpStatus& os) {
    if (dbi != NULL) {
        U2DbiPool *pool = getDbiPool(os);
        SAFE_POINT_OP(os, );
        pool->releaseDbi(dbi, os);
        dbi = NULL;
    }
}

bool DbiConnection::isOpen() const {
    return dbi != NULL;
}

DbiConnection & DbiConnection::operator=(DbiConnection const& dbiConnection) {
    if (this == &dbiConnection) {
        return *this;
    }

    U2OpStatus2Log os;
    close(os);
    copy(dbiConnection);
    return *this;
}

void DbiConnection::copy(DbiConnection const& dbiConnection) {
    dbi = dbiConnection.dbi;
    if (dbiConnection.dbi != NULL) {
        U2OpStatus2Log os;
        U2DbiPool *pool = getDbiPool(os);
        SAFE_POINT_OP(os, );
        pool->addRef(dbi, os);
    }
}

DbiConnection::DbiConnection() : dbi(NULL) {

}

}   // namespace U2
