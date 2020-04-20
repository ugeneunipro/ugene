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

#include "ActorCfgFilterProxyModel.h"
#include "ActorCfgModel.h"

namespace U2 {

ActorCfgFilterProxyModel::ActorCfgFilterProxyModel(QObject *p)
    : QSortFilterProxyModel(p)
{
    setDynamicSortFilter(true);
}

bool ActorCfgFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex & /*sourceParent*/) const {
    ActorCfgModel *srcModel = qobject_cast<ActorCfgModel *>(sourceModel());
    SAFE_POINT(NULL != srcModel, "Invalid actor configuration model", true);

    Attribute *attr = srcModel->getAttributeByRow(sourceRow);
    SAFE_POINT(NULL != attr, "Invalid actor attribute", true);

    return srcModel->isVisible(attr);
}

bool ActorCfgFilterProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex & /*sourceParent*/) const {
    ActorCfgModel *srcModel = qobject_cast<ActorCfgModel *>(sourceModel());
    SAFE_POINT(NULL != srcModel, "Invalid actor configuration model", true);

    return srcModel->getScriptMode() || sourceColumn < 2;
}

} // namespace U2
