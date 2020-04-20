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

#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "U2Dbi.h"

namespace U2 {

const QString U2DbiOptions::APP_MIN_COMPATIBLE_VERSION(QString("Minimum version of ") + U2_PRODUCT_KEY + " compatible with the schema");

const int U2DbiOptions::U2_DBI_NO_LIMIT = -1;

const QString U2DbiOptions::U2_DBI_OPTION_URL("url");

const QString U2DbiOptions::U2_DBI_OPTION_CREATE("create");

const QString U2DbiOptions::U2_DBI_OPTION_PASSWORD("password");

const QString U2DbiOptions::U2_DBI_VALUE_ON("1");

const QString U2DbiOptions::U2_DBI_LOCKING_MODE("locking_mode");

//////////////////////////////////////////////////////////////////////////
// U2DbiFactory

U2DbiFactory::U2DbiFactory() {

}

U2DbiFactory::~U2DbiFactory() {

}

//////////////////////////////////////////////////////////////////////////
// U2Dbi

U2Dbi::~U2Dbi() {

}

U2DbiRef U2Dbi::getDbiRef() const {
    return U2DbiRef(getFactoryId(), getDbiId());
}

bool U2Dbi::isInitialized(U2OpStatus &) {
    return false;
}

void U2Dbi::populateDefaultSchema(U2OpStatus &) {

}

U2ObjectRelationsDbi * U2Dbi::getObjectRelationsDbi() {
    return NULL;
}

void U2Dbi::startOperationsBlock(U2OpStatus &) {

}

void U2Dbi::stopOperationBlock(U2OpStatus&) {

}

QMutex * U2Dbi::getDbMutex( ) const {
    return NULL;
}

void U2Dbi::setVersionProperties(const Version &minVersion, U2OpStatus &os) {
    setProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, minVersion.text, os);
}

//////////////////////////////////////////////////////////////////////////
// U2ChildDbi

U2ChildDbi::U2ChildDbi(U2Dbi *_rootDbi)
    : rootDbi (_rootDbi)
{

}

U2ChildDbi::~U2ChildDbi() {

}

U2Dbi * U2ChildDbi::getRootDbi() const {
    return rootDbi;
}

}
