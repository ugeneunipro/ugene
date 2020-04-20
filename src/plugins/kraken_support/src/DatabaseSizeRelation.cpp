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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>

#include "DatabaseSizeRelation.h"

namespace U2 {
namespace LocalWorkflow {

DatabaseSizeRelation::DatabaseSizeRelation(const QString &relatedAttributeId)
    : ValuesRelation(relatedAttributeId, QVariantMap())
{

}

QVariant DatabaseSizeRelation::getAffectResult(const QVariant &influencingValue, const QVariant &dependentValue, DelegateTags *, DelegateTags *) const {
    const QString databaseUrl = influencingValue.toString();
    CHECK(!databaseUrl.isEmpty(), dependentValue);
    const QFileInfo databaseInfo(databaseUrl + "/database.kdb");
    CHECK(databaseInfo.exists(), dependentValue);

    const qint64 totalMemoryInMb = AppContext::getAppSettings()->getAppResourcePool()->getTotalPhysicalMemory();
    return databaseInfo.size() < totalMemoryInMb * 1024 * 1024;
}

DatabaseSizeRelation *DatabaseSizeRelation::clone() const {
    return new DatabaseSizeRelation(*this);
}

}   // namespace LocalWorkflow
}   // namespace U2
