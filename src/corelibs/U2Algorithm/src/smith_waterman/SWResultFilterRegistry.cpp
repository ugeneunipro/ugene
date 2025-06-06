/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "SWResultFilterRegistry.h"

#include <QMutexLocker>

namespace U2 {

SWResultFilterRegistry::SWResultFilterRegistry(QObject* pOwn)
    : QObject(pOwn) {
    registerFilter(new SWRF_EmptyFilter());

    SmithWatermanResultFilter* f = new SWRF_WithoutIntersect();
    registerFilter(f);
    defaultFilterId = f->getId();
}

SWResultFilterRegistry::~SWResultFilterRegistry() {
    QList<SmithWatermanResultFilter*> swFilters = filters.values();
    foreach (SmithWatermanResultFilter* swFilter, swFilters) {
        delete swFilter;
    }
}

QStringList SWResultFilterRegistry::getFiltersIds() const {
    return filters.keys();
}

SmithWatermanResultFilter* SWResultFilterRegistry::getFilter(const QString& id) {
    return filters.value(id, 0);
}

bool SWResultFilterRegistry::isRegistered(const QString& id) const {
    return filters.contains(id);
}

bool SWResultFilterRegistry::registerFilter(SmithWatermanResultFilter* filter) {
    QMutexLocker locker(&mutex);
    QString id = filter->getId();
    if (filters.contains(id)) {
        return false;
    }
    filters[id] = filter;
    return true;
}

}  // namespace U2
