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
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/MysqlDbiUtils.h>

#include "MysqlUpgradeTask.h"

namespace U2 {

MysqlUpgradeTask::MysqlUpgradeTask(const U2DbiRef &dbiRef) :
    Task(tr("Upgrade mysql database"), TaskFlag_None),
    dbiRef(dbiRef)
{
    SAFE_POINT_EXT(MYSQL_DBI_ID == dbiRef.dbiFactoryId, setError(QString("Unexpected dbi factory id: expect '%1', got '%2'").arg(MYSQL_DBI_ID).arg(dbiRef.dbiFactoryId)), );
}

void MysqlUpgradeTask::run() {
    AppContext::getDbiRegistry()->getGlobalDbiPool()->closeAllConnections(dbiRef, stateInfo);
    MysqlDbiUtils::upgrade(dbiRef, stateInfo);
}

const U2DbiRef &MysqlUpgradeTask::getDbiRef() const {
    return dbiRef;
}



}   // namespace U2
